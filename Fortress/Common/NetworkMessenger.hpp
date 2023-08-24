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
		void wait_confirm();

		void go_and_wait(const CRC32& last_message);

		void join_lobby(LobbyInfoMsg* out);
		bool check_lobby_update(LobbyInfoMsg* out);

		void join_room(RoomID room_id, RoomInfoMsg* out);
		bool check_room_update(RoomInfoMsg* out);
		void send_character(eCharacterType character);
		void send_item(eItemType item, unsigned index);

		void start_game(const eMapType& map, GameInitMsg* out);
		bool check_room_start(GameInitMsg* out);

		void call_loading_finished();
		void send_delta_time(float deltaTime);
		bool check_game_start(GameStartMsg& gsm);

		void get_wind_acceleration(RspWindMsg* wind);
		void send_turn_end();

		void send_move_signal(Math::Vector2 position, Math::Vector2 offset);
		bool get_move_signal(PlayerID player_id, PositionMsg* position);
		void send_stop_signal(Math::Vector2 position, Math::Vector2 offset);
		bool get_stop_signal(PlayerID player_id, StopMsg* position);
		void send_projectile_select_signal(eProjectileType type);
		bool get_projectile_select_signal(PlayerID player_id, ProjectileSelectMsg* projectile);
		void send_firing_signal(Math::Vector2 position, Math::Vector2 offset);
		bool get_firing_signal(PlayerID player_id, FiringMsg* firing);
		void send_fire_signal(Math::Vector2 position, Math::Vector2 offset, float charged);
		bool get_fire_signal(PlayerID player_id, CharacterFireMsg* fire);
		void send_item_signal(Math::Vector2 position, Math::Vector2 offset, unsigned index, eItemType item);
		bool get_item_signal(PlayerID player_id, ItemMsg* item);
		void send_item_fire_signal(Math::Vector2 position, Math::Vector2 offset, unsigned index, eItemType item, float charged);
		bool get_item_fire_signal(PlayerID player_id, eItemType type, ItemFireMsg* item);
		void send_hit_signal(eObjectType type, Math::Vector2 position);
		bool get_hit_signal(PlayerID player_id, ProjectileHitMsg* hit);

		template <typename T = Message>
		void send_message(const eMessageType type, const T& pre_packed)
		{
			auto msg = create_prewritten_network_message<T>(pre_packed);
			msg.room_id = m_rood_id_;
			msg.player_id = m_player_id;
			msg.type = type;
			m_soc.send_message<T>(&msg, m_server_info);
		}

		template <typename T = Message>
		bool pop_message(const eMessageType type, const PlayerID send_player_id, T* out, std::function<bool(const T*)> predicate)
		{
			return m_soc.find_message<T>(type, out) && out->room_id == m_rood_id_ && out->player_id == send_player_id && predicate(out);
		}

	private:
		template <typename SendT, typename RecvT = Message>
		inline void send_and_retry(
			const SendT* msg,
			const SOCKADDR_IN& client_info,
			RecvT* reply,
			const eMessageType reply_type)
		{
			std::unique_lock ql(queue_mutex);
			m_soc.send_message<SendT>(msg, client_info);
			queue_event.wait(ql);

			while(!m_soc.find_message<RecvT>(reply_type, reply))
			{
				m_soc.send_message<SendT>(msg, client_info);
				queue_event.wait(ql);
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
		std::recursive_mutex& queue_mutex = m_soc.queue_lock;
		std::condition_variable_any& queue_event = m_soc.queue_event;
	};
}
