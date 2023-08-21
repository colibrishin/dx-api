#include "pch.h"
#include "NetworkMessenger.hpp"

#include "deltatime.hpp"
#include "message.hpp"

namespace Fortress::Network
{
	NetworkMessenger::NetworkMessenger() : m_player_id(1), m_rood_id_(-1)
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

	void NetworkMessenger::send_alive()
	{
		constexpr float alive_interval = 0.1f;

		static float counter = 0.0f;

		if(alive_interval <= counter)
		{
			const auto msg = create_network_message<PingMsg>(eMessageType::PING, -1, m_player_id);
			m_soc.send_message<PingMsg>(&msg, m_server_info);
			counter = 0.0f;
		}

		counter += DeltaTime::get_deltaTime();
	}

	void NetworkMessenger::send_confirm(CRC32 previous_msg)
	{
		const auto confirm_msg = create_network_message<GOMsg>(
			eMessageType::RoomStart, m_rood_id_, m_player_id, previous_msg);
		m_soc.send_message<GOMsg>(&confirm_msg, m_server_info);
	}

	void NetworkMessenger::join_lobby(LobbyInfoMsg* out)
	{
		auto msg = create_network_message<LobbyJoinMsg>(
			eMessageType::LobbyJoin, -1, m_player_id);

   		send_and_retry<LobbyJoinMsg, LobbyInfoMsg>(out, &msg, m_server_info);

		set_room_id(-1);
	}

	void NetworkMessenger::join_room(RoomID room_id, RoomInfoMsg* out)
	{
		auto msg = create_network_message<RoomJoinMsg>(
			eMessageType::RoomJoin, room_id, m_player_id);

		send_and_retry<RoomJoinMsg, RoomInfoMsg>(out, &msg, m_server_info);

		set_room_id(room_id);
	}

	bool NetworkMessenger::check_room_update(RoomInfoMsg* out)
	{
		if(m_soc.find_message<RoomInfoMsg>(out))
		{
			return true;
		}

		return false;
	}

	void NetworkMessenger::go_and_wait(const CRC32& last_message)
	{
		send_confirm(last_message);
		GOMsg go{};
		// @todo: recursive mutex is needed but conditional variable does not support in C++17.
		while(!m_soc.find_message<GOMsg>(&go)) {}
	}

	void NetworkMessenger::start_game(const eMapType& map, GameInitMsg* out)
	{
		const auto msg = create_network_message<RoomStartMsg>(
			eMessageType::RoomStart, m_rood_id_, m_player_id, map);

		send_and_retry<RoomStartMsg, GameInitMsg>(out, &msg, m_server_info);
	}

	bool NetworkMessenger::check_room_start(GameInitMsg* out)
	{
		return m_soc.find_message<GameInitMsg>(out);
	}

	void NetworkMessenger::call_loading_finished()
	{
		const auto msg = create_network_message<LoadDoneMsg>(
			eMessageType::LoadDone, m_rood_id_, m_player_id);

		m_soc.send_message<LoadDoneMsg>(&msg, m_server_info);
	}

	void NetworkMessenger::send_delta_time(float deltaTime)
	{
		auto msg = create_network_message<DeltaTimeMsg>(
			eMessageType::DeltaTime, m_rood_id_, m_player_id, deltaTime);
		m_soc.send_message(&msg, m_server_info);
	}

	bool NetworkMessenger::check_game_start(GameStartMsg& gsm)
	{
		return m_soc.find_message<GameStartMsg>(&gsm);
	}

	void NetworkMessenger::send_character_position(Math::Vector2 position)
	{
		const auto msg = create_network_message<PositionMsg>(
			eMessageType::Position, m_rood_id_, m_player_id, eObjectType::Character, position);
		m_soc.send_message<PositionMsg>(&msg, m_server_info);
	}

	bool NetworkMessenger::get_updated_character_position(
		PlayerID player_id,
		PositionMsg* position)
	{
		return m_soc.find_message<PositionMsg>(position) &&
			position->player_id == player_id &&
			position->object_type == eObjectType::Character &&
			position->room_id == m_rood_id_;
	}

	bool NetworkMessenger::get_updated_projectile_position(
		PlayerID player_id,
		PositionMsg* position)
	{
		return m_soc.find_message<PositionMsg>(position) &&
			position->player_id == player_id &&
			position->object_type == eObjectType::Projectile &&
			position->room_id == m_rood_id_;
	}

	bool NetworkMessenger::check_lobby_update(LobbyInfoMsg* out)
	{
		if(m_soc.find_message<LobbyInfoMsg>(out))
		{
			return true;
		}

		return false;
	}

	void NetworkMessenger::set_player_id(const PlayerID id)
	{
		m_player_id = id;
	}

	void NetworkMessenger::set_room_id(const RoomID id)
	{
		m_rood_id_ = id;
	}

	PlayerID NetworkMessenger::get_player_id() const
	{
		return m_player_id;
	}

	RoomID NetworkMessenger::get_room_id() const
	{
		return m_rood_id_;
	}

	void NetworkMessenger::send_character(
		RoomID room_id,
		eCharacterType character)
	{
		auto msg = create_network_message<RoomSelectChMsg>(
			eMessageType::RoomSelectCh, room_id, m_player_id);

		msg.ch_type = character;

		m_soc.send_message<RoomSelectChMsg>(&msg, m_server_info);
	}

	bool NetworkMessenger::check_delta_time()
	{
		return m_delta_time_ >= tick_rate;
	}

	void NetworkMessenger::increase_delta_time()
	{
		m_delta_time_ += DeltaTime::get_deltaTime();
	}

	void NetworkMessenger::reset_delta_time()
	{
		m_delta_time_ = 0.0f;
	}
}
