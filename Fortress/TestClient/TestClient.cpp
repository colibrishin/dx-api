#include <iostream>

#include "../Common/Socket.hpp"
#include "../Common/message.hpp"

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "Common.lib")

int main()
{
	Fortress::Network::Server::Socket soc{60901};

	sockaddr_in server
	{
		AF_INET,
		htons(51211)
	};

	inet_pton(PF_INET, "127.0.0.1", &server.sin_addr);

    while(true)
    {
	    auto message = Fortress::Network::create_network_message<Fortress::Network::PingMsg>(Fortress::Network::eMessageType::PONG, 0, 0);
	    soc.send_message<Fortress::Network::PingMsg>(&message, server);
		std::cout << "Sending Packets..." << std::endl;
		Sleep(0x000000ff);
    }
}