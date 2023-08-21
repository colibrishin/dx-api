#pragma once
#include <thread>

#include "../Common/Socket.hpp"

#pragma comment (lib, "Common.lib")

namespace Fortress::Network
{
	constexpr unsigned int retry_time = 500;
	constexpr float tick_rate = 0.01f;

	class NetworkMessenger
	{
	public:
		NetworkMessenger();
		virtual ~NetworkMessenger() = default;

		void set_player_id(PlayerID id);
		void set_room_id(RoomID id);
		PlayerID get_player_id() const;
		RoomID get_room_id() const;

		void send_alive();
		void send_confirm(CRC32 previous_msg);

		void go_and_wait(const CRC32& last_message);

		void join_lobby(LobbyInfoMsg* out);
		bool check_lobby_update(LobbyInfoMsg* out);

		void join_room(RoomID room_id, RoomInfoMsg* out);
		bool check_room_update(RoomInfoMsg* out);

		void start_game(const eMapType& map, GameInitMsg* out);
		bool check_room_start(GameInitMsg* out);

		void call_loading_finished();
		void send_delta_time(float deltaTime);
		bool check_game_start(GameStartMsg& gsm);

		void send_character_position(Math::Vector2 position);
		bool get_updated_character_position(PlayerID player_id, PositionMsg* position);
		bool get_updated_projectile_position(PlayerID player_id, PositionMsg* position);

		void send_character(RoomID room_id, eCharacterType character);

	private:
		template <typename SendT, typename RecvT = Message>
		inline void send_and_retry(
			RecvT* reply,
			const SendT* msg,
			const SOCKADDR_IN& client_info)
		{
			m_soc.send_message<SendT>(msg, client_info);

			while(!m_soc.find_message<RecvT>(reply))
			{
				Sleep(retry_time);
				m_soc.send_message<SendT>(msg, client_info);
			}

			m_soc.flush_message(reply->type);
		}

		static bool check_delta_time();
		static void increase_delta_time();
		static void reset_delta_time();

		inline static float m_delta_time_ = 0.0f;

		Server::Socket m_soc {60901, false};
		SOCKADDR_IN m_server_info{};
		std::thread m_receiver;

		PlayerID m_player_id;
		RoomID m_rood_id_;
	public:
		template <typename T = Message, typename... Args>
		void send_message_within_tick_rate(eMessageType type, Args... args)
		{
			if(check_delta_time())
			{
				T msg = create_network_message<T>(
					type, m_rood_id_, m_player_id, args...);
				m_soc.send_message<T>(&msg, m_server_info);
				reset_delta_time();
			}

			increase_delta_time();
		}

		std::mutex& queue_mutex = m_soc.queue_lock;
		std::condition_variable& queue_event = m_soc.queue_event;
	};
}
