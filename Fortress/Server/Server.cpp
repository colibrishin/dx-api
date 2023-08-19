#include <cassert>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "../Common/Socket.hpp"
#include "../Common/message.hpp"
#include "../Common/vector2.hpp"
#include "../Common/message.hpp"

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "Common.lib")

namespace Fortress::Network::Server
{
	static Socket server_socket = Socket(51211);

	const sockaddr* extract_ip(const SOCKADDR_IN& client_info)
	{
		return reinterpret_cast<const sockaddr*>(&client_info);
	}

	struct Client
	{
		SOCKADDR_IN ip;
		PlayerID pid;
		std::time_t last_contact;
	};

	const std::map<PlayerID, std::wstring> client_names = {{1, L"Test1"}, {2, L"Test2"}};

	static std::list<Room> room_list = {};

	static std::mutex player_list_lock;
	static bool player_list[15]{ false, };


	static std::map<std::pair<RoomID, PlayerID>, Client> client_list = {};

	void add_client(RoomID id, PlayerID pid, const SOCKADDR_IN& client_info, const std::time_t time)
	{
		if (client_list.find({id, pid}) != client_list.end())
		{
			const auto& [eip, epid, etime] = client_list[{id, pid}];

			if(std::strncmp(
				reinterpret_cast<const char*>(&eip),
				reinterpret_cast<const char*>(extract_ip(client_info)),
				sizeof(sockaddr)) != 0)
			{
				std::cout << "Client IP changed. Removing from known list." << std::endl;
				client_list.erase({id, pid});
				return;
			}
		}

		client_list[{id, pid}] = {client_info, pid, time};
	}

	void reply_lobby_info(const SOCKADDR_IN& client_info)
	{
		wchar_t player_names[15][15]{};
		int pos = 0;

		{
			std::lock_guard _(player_list_lock);

			for(int i = 0; i < 15; ++i)
			{
				if(player_list[i])
				{
					std::memcpy(
						player_names[pos], 
						client_names.at(i).c_str(),
						client_names.at(i).length() * sizeof(wchar_t));
					pos++;
				}
			}
		}

		LobbyInfo li{{0, eMessageType::LobbyInfo, -1, -1},
			0, {}, pos, {}};

		std::memcpy(li.player_names, player_names, sizeof(player_names));

		auto reply = create_network_message<LobbyInfo>(li);
		server_socket.send_message(&reply, client_info);
	}

	void reply_ping(const SOCKADDR_IN& client_info)
	{
		auto reply = create_network_message<PongMsg>(
			eMessageType::PONG, -1, -1);
		server_socket.send_message<PongMsg>(&reply, client_info);
	}

	void broadcast_lobby_info()
	{
		std::cout << "Broadcasting..." << std::endl;
		for (int i = 0; i < 15; ++i)
		{
			if (client_list.find({-1, i}) != client_list.end())
			{
				reply_lobby_info(client_list.at({-1, i}).ip);
			}
		}
	}

	[[noreturn]] void lobby_info_schedule()
	{
		while(true)
		{
			broadcast_lobby_info();
			Sleep(3000);
		}
	}

	void add_player(PlayerID player_id)
	{
		std::cout << "Add to lobby player list..." << std::endl;
		{
			std::lock_guard _(player_list_lock);
			player_list[player_id] = true;
		}
	}

	[[noreturn]] void cleanup_bad_clients()
	{
		while(true)
		{
			{
				std::unique_lock list_lock(server_socket.bad_client_lock);
				server_socket.m_bad_client_event.wait(list_lock);

				while (!server_socket.m_bad_client_list.empty())
				{
					const auto bad_client = server_socket.m_bad_client_list.front();
					// There can be multiple room id + player id. This should be blocked or cleaned up somehow.
					std::pair<RoomID, PlayerID> target_key_pair = {-1, -1};

					for(const auto& [key_pair, client] : client_list)
					{
						if(std::memcmp(&client.ip, &bad_client, sizeof(sockaddr_in)) == 0)
						{
							target_key_pair = key_pair;
							break;
						}
					}

					if (target_key_pair.second != -1)
					{
						client_list.erase(target_key_pair);
					}

					server_socket.m_bad_client_list.pop_front();
				}
			}

			std::pair<RoomID, PlayerID> target_key_pair = {-1, -1};

			for(const auto& [key_pair, client] : client_list)
			{
				if(difftime(client.last_contact,get_time()) >= 60000)
				{
					target_key_pair = key_pair;
					break;
				}
			}

			if(target_key_pair.second != -1)
			{
				client_list.erase(target_key_pair);
			}
		}
	}

	[[noreturn]] void consume_message()
	{
		while(true) 
		{
			char buffer[max_packet_size + 1]{};
			sockaddr_in client_info{};
			std::time_t time;

			server_socket.get_any_message(&client_info, time, buffer);

			const Message* message = reinterpret_cast<Message*>(buffer);

			switch(message->type)
			{
			case eMessageType::GO:
			case eMessageType::NOGO:
			case eMessageType::LobbyInfo:
			case eMessageType::PONG: break; // Server side message.
			case eMessageType::PING:
				add_client(message->room_id, message->player_id, client_info, time);
				reply_ping(client_info);
				break;
			case eMessageType::DeltaTime:
				break;
			case eMessageType::Position:
				break;
			case eMessageType::Fire:
				break;
			case eMessageType::Item:
				break;
			case eMessageType::Hit:
				break;
			case eMessageType::Destroyed:
				break;
			case eMessageType::Wind:
				break;
			case eMessageType::LobbyJoin:
				add_client(message->room_id, message->player_id, client_info, time);
				add_player(message->player_id);
				std::cout << "New player joined. Broadcasting new lobby info." << std::endl;
				broadcast_lobby_info();
				break;
			default: break; // Unknown type of message
			}
		}
	}
}

int main()
{
    std::cout << "Start server...\n";

	std::thread receiving_task(
		&Fortress::Network::Server::Socket::receiving_message, 
		&Fortress::Network::Server::server_socket);
	std::thread consume_task(Fortress::Network::Server::consume_message);
	std::thread lobby_update_task(Fortress::Network::Server::lobby_info_schedule);
	std::thread client_purger(Fortress::Network::Server::cleanup_bad_clients);

	receiving_task.detach();
	consume_task.detach();
	lobby_update_task.detach();
	client_purger.detach();

	Sleep(0xffffffff);

	return 0;
}