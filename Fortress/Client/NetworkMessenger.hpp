#pragma once
#include <thread>

#include "../Common/Socket.hpp"
#include "../Common/deltatime.hpp"
#include <WinSock2.h>

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "Common.lib")

namespace Fortress::Network
{
	class NetworkMessenger
	{
	public:
		NetworkMessenger();
		virtual ~NetworkMessenger() = default;
		void send_alive();
		void join_lobby(LobbyInfo* out);
		bool check_lobby_update(LobbyInfo* out);
		void set_player_id(PlayerID id);
	private:
		Server::Socket m_soc {60901};
		SOCKADDR_IN m_server_info;
		std::thread m_receiver;
		PlayerID m_player_id;
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

	inline void NetworkMessenger::join_lobby(LobbyInfo* out)
	{
		send_alive();

		auto msg = create_network_message<LobbyJoin>(eMessageType::LobbyJoin, -1, m_player_id);
		
		LobbyInfo reply{};

   		m_soc.send_message(&msg, m_server_info);
  		m_soc.find_message<LobbyInfo>(&reply);
		
		std::memcpy(out, &reply, sizeof(LobbyInfo));
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
}
