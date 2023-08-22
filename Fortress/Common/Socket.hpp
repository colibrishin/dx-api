#ifndef SOCKET_HPP
#define SOCKET_HPP
#include "pch.h"
#include <cassert>
#include <iostream>
#include <ws2tcpip.h>
#include <array>
#include <vector>
#include <mutex>
#include <winsock2.h>
#include <condition_variable>

#include "hash_fnv1.hpp"
#include "../Common/message.hpp"

#pragma comment (lib, "ws2_32.lib")

namespace Fortress::Network::Server
{
	class Socket
	{
		using QueuePair = std::pair<unsigned int, std::vector<char>>;

	public:
		using MessageTuple = std::tuple<SOCKADDR_IN, time_t, const Message*>;

		Socket(const u_short listen, bool check_bad_client) : m_b_check_bad_client(check_bad_client)
		{
			initialize(listen);
		}
		~Socket()
		{
			if(m_socket_data)
			{
				WSACleanup();
				m_socket_data.reset();
			}
			if(m_socket != INVALID_SOCKET)
			{
				closesocket(m_socket);
			}

			m_bIsRunning = false;

			std::unique_lock bcl(bad_client_lock);
			if(!bcl.owns_lock())
			{
				m_bad_client_event.wait(bcl);
			}

			std::unique_lock rl(receiving_lock);
			if(!rl.owns_lock())
			{
				receive_event.wait(rl);
			}
		}

		void block_until_queue_event()
		{
			std::unique_lock ql(queue_lock);
			queue_event.wait(ql);
		}

		bool get_any_message(SOCKADDR_IN* info_out, std::time_t& time_out, char* message_out)
		{
			std::lock_guard _(queue_lock);

			if(m_message_queue_.empty())
			{
				return false;
			}

			auto [info, time, val] = m_message_queue_.front();
			m_message_queue_.pop_front();
			std::memcpy(info_out, &info, sizeof(sockaddr_in));
			time_out = time;
			std::memcpy(message_out, val, max_packet_size + 1);

			delete[] reinterpret_cast<const char*>(val);

			return true;
		}

		template <typename T = Message>
		bool find_message(const eMessageType type, T* out)
		{
			{
				int pos = 0;
				bool found = false;
				std::lock_guard _(queue_lock);

				if(m_message_queue_.empty())
				{
					return false;
				}

				for(auto& [info, time, msg] : m_message_queue_)
				{
					const T* casted_msg = reinterpret_cast<const T*>(msg);
					const unsigned int reeval_crc = get_crc32<T>(*casted_msg);

					if (casted_msg->crc32 == reeval_crc && msg->type == type)
					{
						std::memcpy(out, msg, sizeof(T));
						delete[] reinterpret_cast<const char*>(msg);
						found = true;
						break;
					}

					pos++;
				}

				if(found)
				{
					m_message_queue_.erase(m_message_queue_.begin() + pos);
					return true;
				}
			}

			return false;
		}

		void receiving_message()
		{
			while (m_bIsRunning)
			{
				std::pair<int, sockaddr_in> recv_info;

				{
					std::lock_guard rl(receiving_lock);

					recv_info = wait_for_recv();
					receive_event.notify_all();
				}

				if ((recv_info.first <= 0 || recv_info.first > max_packet_size + 1) && m_b_check_bad_client)
				{
					add_bad_client(recv_info.second);
					continue;
				}

				{
					std::lock_guard ql(queue_lock);

					char* buffer = new char[max_packet_size + 1]{};

					retrieve_message(buffer);

					m_message_queue_.emplace_back(
						recv_info.second,
						get_time(),
						reinterpret_cast<const Message*>(buffer));

					queue_event.notify_all();
				}
			}
		}

		void flush_message(const eMessageType type)
		{
			std::lock_guard ql(queue_lock);

			m_message_queue_.erase(
				std::remove_if(m_message_queue_.begin(), m_message_queue_.end(), [&](const MessageTuple& t)
				{
					return std::get<2>(t)->type == type;
				}),
				m_message_queue_.end()
			);
		}

		template <typename T>
		void send_message(const T* message, const SOCKADDR_IN& client_info)
		{
			if(sizeof(T) > max_packet_size)
			{
				return;
			}

			char buffer[max_packet_size + 1]{};
			const unsigned int send_size = sizeof(T);
			std::memcpy(buffer, reinterpret_cast<const char*>(message), send_size);

			const int sent_size = sendto(
				m_socket, 
				buffer,
				sizeof(T),
				0,
				reinterpret_cast<const sockaddr*>(&client_info), 
				sizeof(client_info));

			if(sent_size == -1)
			{
				const int errcode = WSAGetLastError();
				std::cout << errcode << std::endl;

				if(errcode != 0)
				{
					add_bad_client(client_info);
				}
			}
		}

		void add_bad_client(const sockaddr_in& client_info)
		{
			std::unique_lock _(bad_client_lock);
			m_bad_client_list.insert(client_info);
			m_bad_client_event.notify_all();
		}

		[[nodiscard]] bool is_message_available() const
		{
			return m_message_queue_.empty();
		}

		void initialize(const u_short listen)
		{
			m_socket_data = std::make_unique<WSADATA>();

			if(const int sock_start = WSAStartup(0x202, m_socket_data.get());
				sock_start == SOCKET_ERROR)
			{
				std::cout << WSAGetLastError() << std::endl;
				assert(sock_start != SOCKET_ERROR);
			}

			std::cout << "Allocate socket...\n";
			m_socket = socket(AF_INET, SOCK_DGRAM, 0);

			if(m_socket == INVALID_SOCKET)
			{
				std::cout << WSAGetLastError() << std::endl;
				assert(m_socket != INVALID_SOCKET);
			}

			m_local_info = 
			{
				AF_INET,
				htons(listen),
			};

			if(const int ip_conversion_check = inet_pton(AF_INET, "127.0.0.1", &m_local_info.sin_addr); 
				ip_conversion_check != 1)
			{
				std::cout << WSAGetLastError() << std::endl;
				assert(ip_conversion_check == 1);
			}

			unsigned int retry = 0;

			while(bind(m_socket, reinterpret_cast<sockaddr*>(&m_local_info), sizeof(m_local_info)) == SOCKET_ERROR)
			{
				retry++;
				std::cout << "Opening port for " + std::to_string(listen + retry) + "...\n";
				m_local_info.sin_port = htons(listen + retry);
			}

			std::cout << "Opened port for " + std::to_string(listen) + "...\n";

			m_bIsRunning = true;
		}

	private:
		struct FNV
		{
			bool operator()(const SOCKADDR_IN& left, const SOCKADDR_IN& right) const
			{
				const uint64_t lh = hash_64_fnv1a(&left, sizeof(SOCKADDR_IN));
				const uint64_t rh = hash_64_fnv1a(&right, sizeof(SOCKADDR_IN));

				return lh < rh;
			}
		};

		void retrieve_message(char* out) const
		{
 			std::memcpy(out, m_buffer, max_packet_size + 1);
		}

		std::pair<int, SOCKADDR_IN> wait_for_recv()
		{
			SOCKADDR_IN recv_info{};
			int addr_size = sizeof(recv_info);
			std::fill_n(m_buffer, max_packet_size + 1, 0);

			const int recv = recvfrom(
				m_socket, 
				m_buffer, 
				max_packet_size + 1,
				0,
				reinterpret_cast<SOCKADDR*>(&recv_info), 
				&addr_size);

			if(recv == -1)
			{
				const int errcode = WSAGetLastError();
				std::cout << errcode << std::endl;
			}

			return {recv, recv_info};
		}

		std::unique_ptr<WSADATA> m_socket_data;
		SOCKADDR_IN m_local_info{};
		SOCKET m_socket = INVALID_SOCKET;

		bool m_b_check_bad_client;

		std::deque<MessageTuple> m_message_queue_{};

		char m_buffer[max_packet_size + 1]{};
		
		std::atomic<bool> m_bIsRunning;
	public:
		std::condition_variable_any m_bad_client_event;
		std::set<SOCKADDR_IN, FNV> m_bad_client_list{};

		std::condition_variable_any receive_event;
		std::condition_variable_any queue_event;

		std::recursive_mutex queue_lock;
		std::mutex bad_client_lock;
		std::mutex receiving_lock;
	};
}
#endif // SOCKET_HPP
