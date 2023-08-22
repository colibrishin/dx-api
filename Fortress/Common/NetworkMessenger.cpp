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

	void NetworkMessenger::wait_confirm()
	{
		GOMsg go{};

		std::unique_lock ql(m_soc.queue_lock);
		while(!m_soc.find_message<GOMsg>(eMessageType::GO, &go))
		{
			queue_event.wait(ql);
		}
	}

	void NetworkMessenger::join_lobby(LobbyInfoMsg* out)
	{
		auto msg = create_network_message<LobbyJoinMsg>(
			eMessageType::LobbyJoin, -1, m_player_id);

   		send_and_retry<LobbyJoinMsg, LobbyInfoMsg>(&msg, m_server_info, out, eMessageType::LobbyInfo);

		set_room_id(-1);
	}

	void NetworkMessenger::join_room(RoomID room_id, RoomInfoMsg* out)
	{
		auto msg = create_network_message<RoomJoinMsg>(
			eMessageType::RoomJoin, room_id, m_player_id);

		send_and_retry<RoomJoinMsg, RoomInfoMsg>(&msg, m_server_info, out, eMessageType::RoomInfo);

		set_room_id(room_id);
	}

	bool NetworkMessenger::check_room_update(RoomInfoMsg* out)
	{
		if(m_soc.find_message<RoomInfoMsg>(eMessageType::RoomInfo, out))
		{
			return true;
		}

		return false;
	}

	void NetworkMessenger::go_and_wait(const CRC32& last_message)
	{
		send_confirm(last_message);
		wait_confirm();
	}

	void NetworkMessenger::start_game(const eMapType& map, GameInitMsg* out)
	{
		const auto msg = create_network_message<RoomStartMsg>(
			eMessageType::RoomStart, m_rood_id_, m_player_id, map);

		send_and_retry<RoomStartMsg, GameInitMsg>(&msg, m_server_info, out, eMessageType::GameInit);
	}

	bool NetworkMessenger::check_room_start(GameInitMsg* out)
	{
		return m_soc.find_message<GameInitMsg>(eMessageType::GameInit, out);
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
		return m_soc.find_message<GameStartMsg>(eMessageType::GameStart, &gsm);
	}

	void NetworkMessenger::send_move_signal(Math::Vector2 position, Math::Vector2 offset)
	{
		const auto msg = create_network_message<PositionMsg>(
			eMessageType::Position, m_rood_id_, m_player_id, eObjectType::Character, position, offset);
		m_soc.send_message<PositionMsg>(&msg, m_server_info);
	}

	bool NetworkMessenger::get_move_signal(
		PlayerID player_id,
		PositionMsg* position)
	{
		return m_soc.find_message<PositionMsg>(eMessageType::Position, position) &&
			position->player_id == player_id &&
			position->object_type == eObjectType::Character &&
			position->room_id == m_rood_id_;
	}

	void NetworkMessenger::send_stop_signal(Math::Vector2 position, Math::Vector2 offset)
	{
		const auto msg = create_network_message<StopMsg>(
			eMessageType::Stop, m_rood_id_, m_player_id, eObjectType::Character, position, offset);
		m_soc.send_message<StopMsg>(&msg, m_server_info);
	}

	bool NetworkMessenger::get_stop_signal(
		PlayerID player_id,
		StopMsg* position)
	{
		if(m_soc.find_message<StopMsg>(eMessageType::Stop, position) &&
			position->player_id == player_id &&
			position->object_type == eObjectType::Character &&
			position->room_id == m_rood_id_)
		{
			return true;
		}

		return false;
	}

	void NetworkMessenger::send_projectile_select_signal(eProjectileType type)
	{
		const auto msg = create_network_message<ProjectileSelectMsg>(
			eMessageType::ProjectileSelect, m_rood_id_, m_player_id, type);
		m_soc.send_message<ProjectileSelectMsg>(&msg, m_server_info);
	}

	bool NetworkMessenger::get_projectile_select_signal(PlayerID player_id, ProjectileSelectMsg* projectile)
	{
		return m_soc.find_message<ProjectileSelectMsg>(eMessageType::ProjectileSelect, projectile) &&
			projectile->player_id == player_id &&
			projectile->room_id == m_rood_id_;
	}

	bool NetworkMessenger::get_updated_projectile_position(
		PlayerID player_id,
		PositionMsg* position)
	{
		return m_soc.find_message<PositionMsg>(eMessageType::Position, position) &&
			position->player_id == player_id &&
			position->object_type == eObjectType::Projectile &&
			position->room_id == m_rood_id_;
	}

	bool NetworkMessenger::check_lobby_update(LobbyInfoMsg* out)
	{
		if(m_soc.find_message<LobbyInfoMsg>(eMessageType::LobbyInfo, out))
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

	void NetworkMessenger::send_character(eCharacterType character)
	{
		auto msg = create_network_message<RoomSelectChMsg>(
			eMessageType::RoomSelectCh, m_rood_id_, m_player_id);

		msg.ch_type = character;

		m_soc.send_message<RoomSelectChMsg>(&msg, m_server_info);
	}

	void NetworkMessenger::send_item(eItemType item, unsigned index)
	{
		const auto msg = create_network_message<RoomSelectItMsg>(
			eMessageType::RoomSelectIt, m_rood_id_, m_player_id, index, item);
		m_soc.send_message<RoomSelectItMsg>(&msg, m_server_info);
	}

	void NetworkMessenger::send_firing_signal(Math::Vector2 position, Math::Vector2 offset)
	{
		auto msg = create_network_message<FiringMsg>(
			eMessageType::Firing, m_rood_id_, m_player_id, eObjectType::Character, position, offset);

		m_soc.send_message<FiringMsg>(&msg, m_server_info);
	}

	bool NetworkMessenger::get_firing_signal(
		PlayerID player_id,
		FiringMsg* firing)
	{
		return m_soc.find_message<FiringMsg>(eMessageType::Firing, firing) &&
			firing->player_id == player_id &&
			firing->object_type == eObjectType::Character &&
			firing->room_id == m_rood_id_;
	}

	void NetworkMessenger::send_fire_signal(
		Math::Vector2 position,
		Math::Vector2 offset,
		float charged)
	{
		const auto msg = create_network_message<FireMsg>(
			eMessageType::Fire, m_rood_id_, m_player_id, eObjectType::Character, position, offset, charged);

		m_soc.send_message<FireMsg>(&msg, m_server_info);
	}

	bool NetworkMessenger::get_fire_signal(
		PlayerID player_id,
		FireMsg* fire)
	{
		return m_soc.find_message<FireMsg>(eMessageType::Fire, fire) &&
			fire->player_id == player_id &&
			fire->room_id == m_rood_id_;
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
