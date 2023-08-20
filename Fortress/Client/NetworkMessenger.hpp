#pragma once
#include <thread>

#include "../Common/Socket.hpp"
#include "../Common/deltatime.hpp"
#include <WinSock2.h>

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "Common.lib")

namespace Fortress::Network
{
	constexpr unsigned int retry_time = 500;

	class NetworkMessenger
	{
	public:
		NetworkMessenger();
		virtual ~NetworkMessenger() = default;
		void send_alive();
		void send_confirm(RoomID room_id, CRC32 previous_msg);
		bool check_confirm();

		void go_and_wait(RoomID room_id, const CRC32& last_message);

		void join_lobby(LobbyInfo* out);
		bool check_lobby_update(LobbyInfo* out);

		void join_room(RoomID room_id, RoomInfo* out);
		bool check_room_update(RoomInfo* out);

		void start_game(RoomID room_id, eMapType map, GameInitMsg* out);
		bool check_room_start(GameInitMsg* out);
		void call_loading_finished(RoomID room_id);
		void send_deltaTime(RoomID room_id, float deltaTime);
		bool check_game_start();

		void sync_game(RoomID room_id);

		void set_player_id(PlayerID id);
		void send_character(RoomID room_id, eCharacterType character);
	private:
		template <typename T>
		void send_and_retry(
			T* reply,
			const Message* msg,
			const SOCKADDR_IN& client_info);

		Server::Socket m_soc {60901};
		SOCKADDR_IN m_server_info;
		std::thread m_receiver;
		PlayerID m_player_id;

	public:
		std::mutex& queue_mutex = m_soc.queue_lock;
		std::condition_variable& queue_event = m_soc.queue_event;
	};

	inline NetworkMessenger::NetworkMessenger() : m_player_id(1)
	{
		m_server_info = 
		{
			AF_INET,
			htons(51211)
		};

		inet_pton(PF_INET, "127.0.0.1", &m_server_info.sin_addr);
		m_receiver = std::thread(&Server::Socket::receiving_message, &m_soc);
		m_receiver.detach();
	}

	inline void NetworkMessenger::send_alive()
	{
		constexpr float alive_interval = 0.1f;

		static float counter = 0.0f;

		if(alive_interval <= counter)
		{
			auto msg = create_network_message<PingMsg>(eMessageType::PING, -1, m_player_id);
			m_soc.send_message(&msg, m_server_info);
			counter = 0.0f;
		}

		counter += DeltaTime::get_deltaTime();
	}

	inline void NetworkMessenger::send_confirm(RoomID room_id, CRC32 previous_msg)
	{
		auto confirm_msg = create_network_message<GOMsg>(
			eMessageType::RoomStart, room_id, m_player_id, previous_msg);
		m_soc.send_message(&confirm_msg, m_server_info);
	}

	inline void NetworkMessenger::join_lobby(LobbyInfo* out)
	{
		auto msg = create_network_message<LobbyJoin>(
			eMessageType::LobbyJoin, -1, m_player_id);

   		send_and_retry(out, &msg, m_server_info);
	}

	inline void NetworkMessenger::join_room(RoomID room_id, RoomInfo* out)
	{
		auto msg = create_network_message<RoomJoin>(
			eMessageType::RoomJoin, room_id, m_player_id);

		send_and_retry(out, &msg, m_server_info);
	}

	inline bool NetworkMessenger::check_room_update(RoomInfo* out)
	{
		if(m_soc.find_message<RoomInfo>(out))
		{
			return true;
		}

		return false;
	}

	inline void NetworkMessenger::go_and_wait(
		RoomID room_id, const CRC32& last_message)
	{
		send_confirm(room_id, last_message);
		GOMsg go{};
		// @todo: recursive mutex is needed but conditional variable does not support in C++17.
		while(!m_soc.find_message<GOMsg>(&go)) {}
	}

	inline void NetworkMessenger::start_game(
		RoomID room_id, eMapType map, GameInitMsg* out)
	{
		auto msg = create_network_message<RoomStart>(
			eMessageType::RoomStart, room_id, m_player_id, map);

		send_and_retry(out, &msg, m_server_info);
	}

	inline bool NetworkMessenger::check_room_start(GameInitMsg* out)
	{
		return m_soc.find_message<GameInitMsg>(out);
	}

	inline void NetworkMessenger::call_loading_finished(RoomID room_id)
	{
		auto msg = create_network_message<LoadDoneMsg>(
			eMessageType::LoadDone, room_id, m_player_id);

		ReqDeltaTimeMsg reply{};
		send_and_retry(&reply, &msg, m_server_info);

		send_deltaTime(room_id, DeltaTime::get_deltaTime());
	}

	inline void NetworkMessenger::send_deltaTime(RoomID room_id, float deltaTime)
	{
		auto msg = create_network_message<DeltaTimeMsg>(
			eMessageType::DeltaTime, room_id, m_player_id, deltaTime);
		m_soc.send_message(&msg, m_server_info);
	}

	inline bool NetworkMessenger::check_game_start()
	{
		GameStartMsg gsm{};

		if(m_soc.find_message<GameStartMsg>(&gsm))
		{
			return true;
		}

		return false;
	}

	inline bool NetworkMessenger::check_lobby_update(LobbyInfo* out)
	{
		if(m_soc.find_message<LobbyInfo>(out))
		{
			return true;
		}

		return false;
	}

	inline void NetworkMessenger::set_player_id(const PlayerID id)
	{
		m_player_id = id;
	}

	inline void NetworkMessenger::send_character(
		RoomID room_id,
		eCharacterType character)
	{
		auto msg = create_network_message<RoomSelectCh>(
			eMessageType::RoomSelectCh, room_id, m_player_id);

		msg.ch_type = character;

		m_soc.send_message<RoomSelectCh>(&msg, m_server_info);
	}

	template <typename T>
	inline void NetworkMessenger::send_and_retry(
		T* reply,
		const Message* msg,
		const SOCKADDR_IN& client_info)
	{
		m_soc.send_message(msg, client_info);

		while(!m_soc.find_message<T>(reply))
		{
			Sleep(retry_time);
			m_soc.send_message(msg, client_info);
		}
	}
}
