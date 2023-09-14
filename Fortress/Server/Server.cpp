#include <cassert>
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "../Common/Socket.hpp"
#include "../Common/message.hpp"
#include "../Common/vector2.hpp"
#include "../Common/message.hpp"

#include "../Common/TimerManager.hpp"
#include "../Common/deltatime.hpp"
#include "../Common/sceneManager.hpp"
#include "../Common/resourceManager.hpp"

#include "ClientSide.hpp"

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "Common.lib")

namespace Fortress::Network::Server
{
	static Socket server_socket = Socket(51211, true);

	const sockaddr* extract_ip(const SOCKADDR_IN& client_info)
	{
		return reinterpret_cast<const sockaddr*>(&client_info);
	}

	struct Client
	{
		SOCKADDR_IN ip;
		PlayerID pid;
		std::time_t last_contact;

		struct RoomInfo
		{
			eCharacterType character{};
			eItemType items[4]{};
			float delta_time;
		} room_info;
	};

	const std::map<PlayerID, std::wstring> client_names = {{1, L"Test1"}, {2, L"Test2"}};

	static std::mutex player_list_lock;
	static bool player_list[15]{ false, };
	static bool load_done[15][15] { false, };
	static bool turn_end[15][15] {false, };
	static bool double_damage_enabled[15][15] {false,};
	static int hit_count[15][15] {0, };

	static std::default_random_engine e;
	static std::uniform_real_distribution<float> dis{-50, 50};
	static float room_wind[15]{0.0f, };

	static std::map<std::pair<RoomID, PlayerID>, Client> client_list = {};

	int get_wind_acceleration(const RoomID room_id)
	{
		return static_cast<int>(room_wind[room_id] / 10.0f) * 10.0f;
	}

	unsigned int get_room_player_count(RoomID room_id)
	{
		int player_count = 0;

		for (int i = 0; i < 15; ++i)
		{
			if (client_list.find({room_id, i}) != client_list.end())
			{
				player_count++;
			}
		}

		return player_count;
	}

	std::vector<Client> get_room_client(RoomID room_id)
	{
		std::vector<Client> clients;

		for(int i = 0; i < 15; ++i)
		{
			if(client_list.find({room_id, i}) != client_list.end())
			{
				clients.push_back(client_list[{room_id, i}]);
			}
		}

		return clients;
	}

	std::vector<Client> get_lobby_client()
	{
		std::vector<Client> clients;

		for(int i = 0; i < 15; ++i)
		{
			if(client_list.find({-1, i}) != client_list.end())
			{
				clients.push_back(client_list[{-1, i}]);
			}
		}

		return clients;
	}

	void send_go(const Message* message, RoomID room_id)
	{
		const auto clients = get_room_client(room_id);

		const auto msg = create_network_message<GOMsg>(
			eMessageType::GO, message->room_id, -1, message->crc32);

		for(const auto& client : clients)
		{
			server_socket.send_message<GOMsg>(&msg, client.ip);
		}
	}

	void add_client(RoomID id, PlayerID pid, const SOCKADDR_IN& client_info, const std::time_t time)
	{
		if (client_list.find({id, pid}) != client_list.end())
		{
			const auto& [eip, epid, etime, _] = client_list[{id, pid}];

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

	void reply_lobby_info(const std::vector<Client>& clients, const SOCKADDR_IN& client_info)
	{
		wchar_t player_names[15][15]{};
		int pos = 0;

		{
			std::lock_guard _(player_list_lock);

			for(const auto& client : clients)
			{
				std::wmemcpy(
					player_names[pos],
					client_names.at(client.pid).c_str(),
					client_names.at(client.pid).length());
				pos++;
			}
		}

		LobbyInfoMsg li{{0, eMessageType::LobbyInfo, -1, -1},
			0, {}, pos, {}};

		std::wmemcpy(li.player_names[0], player_names[0], 15 * 15);

		auto reply = create_prewritten_network_message<LobbyInfoMsg>(li);
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
		const auto clients = get_lobby_client();
		for(const auto& client : clients)
		{
			reply_lobby_info(clients, client.ip);
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
					const auto bad_client = *server_socket.m_bad_client_list.begin();
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

					server_socket.m_bad_client_list.erase(
						server_socket.m_bad_client_list.begin());
				}
			}

			std::pair<RoomID, PlayerID> target_key_pair = {-1, -1};

			for(const auto& [key_pair, client] : client_list)
			{
				if(difftime(client.last_contact,get_time()) >= 60000)
				{
					// @todo: send challenge.
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

	void remove_client_from_lobby(PlayerID player_id)
	{
		std::cout << player_id << " Leaves Lobby" << std::endl;
		client_list.erase({-1, player_id});
	}

	void add_player_to_room(RoomID room_id, PlayerID player_id, const Client& client)
	{
		std::cout << player_id << " Joined " << room_id << std::endl;
		client_list[{room_id, player_id}] = client;
	}

	void send_room_info(const RoomID room_id, const sockaddr_in& client_info)
	{
		std::cout << "Sending Room info to new client" << std::endl;
		RoomInfoMsg rif{};

		int count = 0;
		const auto room_clients = get_room_client(room_id);

		for(const auto& client : room_clients)
		{
			std::wmemcpy(
				rif.player_names[count],
				client_names.at(client.pid).c_str(),
				client_names.at(client.pid).length());
			count++;
		}

		rif.type = eMessageType::RoomInfo;
		rif.player_count = count;
		rif.player_id = -1;
		rif.room_id = room_id;

		auto msg = create_prewritten_network_message<RoomInfoMsg>(rif);
		server_socket.send_message(&msg, client_info);
	}

	void notify_join(RoomID room_id)
	{
		std::cout << "Broadcasting new client to pre-existing clients" << std::endl;
		RoomInfoMsg rif{};

		int count = 0;
		const auto room_clients = get_room_client(room_id);

		for(const auto& client : room_clients)
		{
			std::wmemcpy(
				rif.player_names[count],
				client_names.at(client.pid).c_str(),
				client_names.at(client.pid).length());
			count++;
		}

		rif.type = eMessageType::RoomInfo;
		rif.player_count = count;
		rif.player_id = -1;
		rif.room_id = room_id;

		const auto msg = create_prewritten_network_message<RoomInfoMsg>(rif);

		for(int i = 0; i < 15; ++i)
		{
			if(client_list.find({room_id, i}) != client_list.end())
			{
				server_socket.send_message(&msg, client_list[{room_id, i}].ip);
			}
		}
	}

	void change_character(const Message* message)
	{
		const RoomID room_id = message->room_id;
		const PlayerID player_id = message->player_id;
		const eCharacterType ch_type = reinterpret_cast<const RoomSelectChMsg*>(message)->ch_type;

		client_list[{room_id, player_id}].room_info.character = ch_type;
	}

	void change_items(const Message* message)
	{
		std::cout << "Equip item : " << message->player_id;
		const RoomID room_id = message->room_id;
		const PlayerID player_id = message->player_id;
		const unsigned int it_idx = reinterpret_cast<const RoomSelectItMsg*>(message)->index;
		const eItemType it_type = reinterpret_cast<const RoomSelectItMsg*>(message)->item_type;
		std::cout << static_cast<int>(it_type) << std::endl;

		client_list[{room_id, player_id}].room_info.items[it_idx] = it_type;
	}

	void send_wind(const Message* message, const sockaddr_in& client_info)
	{
		const int wind = get_wind_acceleration(message->room_id);
		const auto msg = create_network_message<RspWindMsg>(
			eMessageType::RspWind, message->room_id, message->player_id, wind);
		server_socket.send_message<RspWindMsg>(&msg, client_info);
	}

	void reset_wind(const RoomID room_id)
	{
		room_wind[room_id] = dis(e);
	}

	void aggregate_send_room_info(const Message* message)
	{
		std::cout << "Room start received, Aggregates room info" << std::endl;
		const auto* recv_msg = reinterpret_cast<const RoomStartMsg*>(message);
		const RoomID room_id = recv_msg->room_id;
		const eMapType map = recv_msg->map_type;
		std::fill_n(hit_count[message->room_id], std::size(hit_count[message->room_id]), 0);

		GameInitMsg gi{};
		uint8_t pos = 0;
		const auto room_clients = get_room_client(room_id);

		for(const auto& client : room_clients)
		{
			gi.players[pos] = client.pid;
			if (client.room_info.character == eCharacterType::None)
			{
				gi.character_type[pos] = eCharacterType::CannonCharacter;
			}
			else
			{
				gi.character_type[pos] = client.room_info.character;
			}

			std::copy(std::begin(client.room_info.items), std::end(client.room_info.items), gi.equied_item[pos]);
			pos++;
		}

		gi.type = eMessageType::GameInit;
		gi.map_type = map;
		gi.player_count = pos;
		gi.player_id = -1;
		gi.room_id = room_id;
		
		std::cout << "Room ID: " << gi.room_id << std::endl;
		std::cout << "Player Count: " << static_cast<int>(gi.player_count) << std::endl;
		std::cout << "Map: " << static_cast<int>(gi.map_type) << std::endl;

		reset_wind(room_id);
		const auto msg = create_prewritten_network_message<GameInitMsg>(gi);

		for(const auto& client : room_clients)
		{
			server_socket.send_message<GameInitMsg>(&msg, client.ip);
		}
	}

	void request_deltatime(RoomID room_id, const Message* message, const sockaddr_in& client_info)
	{
		auto msg = create_network_message<ReqDeltaTimeMsg>(
			eMessageType::ReqDeltaTime, -1, room_id, message->crc32);
		server_socket.send_message<ReqDeltaTimeMsg>(&msg, client_info);
	}

	void set_delta_time(const Message* message)
	{
		const RoomID room_id = message->room_id;
		const PlayerID player_id = message->player_id;
		const float delta_time = reinterpret_cast<const DeltaTimeMsg*>(message)->deltaTime;

		client_list[{room_id, player_id}].room_info.delta_time = delta_time;
	}

	bool check_deltatime_aggregation(RoomID room_id)
	{
		const unsigned int player_count = get_room_player_count(room_id);
		const auto clients = get_room_client(room_id);

		unsigned int dt_count = 0;

		for(const auto& client : clients)
		{
			dt_count += (client.room_info.delta_time != 0.0f);
		}

		return player_count == dt_count;
	}

	void game_gtg(RoomID room_id)
	{
		std::cout << "All Players are ready, game is good to go" << std::endl;

		const auto clients = get_room_client(room_id);
		auto msg = create_network_message<GameStartMsg>(
			eMessageType::GameStart, -1, room_id);

		for(const auto& client : clients)
		{
			server_socket.send_message<GameStartMsg>(&msg, client.ip);
		}
	}

	bool check_priority(RoomID room_id, PlayerID player_id)
	{
		const auto room_clients = get_room_client(room_id);
		const auto min = std::min_element(room_clients.begin(), room_clients.end(), [](const Client& l, const Client& r)
		{
			return l.pid < r.pid;
		});

		if(min == room_clients.end())
		{
			return false;
		}

		return player_id == min->pid;
	}

	bool add_load_done(const Message* message)
	{
		const auto clients = get_room_client(message->room_id);
		int load_finished = 0;
		load_done[message->room_id][message->player_id] = true;

		for(const auto& client: clients)
		{
			if(load_done[message->room_id][client.pid])
			{
				load_finished++;
			}
		}

		return load_finished == clients.size();
	}

	template <typename T>
	void broadcast(const Message* message)
	{
		std::cout << message->room_id << " -> " << message->player_id << " : Received." << std::endl;
		const auto clients = get_room_client(message->room_id);
		const auto* casted_msg = reinterpret_cast<const T*>(message);

		for(const auto& client : clients)
		{
			if(client.pid != message->player_id)
			{
				server_socket.send_message<T>(casted_msg, client.ip);
			}
		}
	}

	bool check_turn_done(const RoomID room_id)
	{
		const auto clients = get_room_client(room_id);
		int load_finished = 0;

		for(const auto& client: clients)
		{
			if(turn_end[room_id][client.pid])
			{
				load_finished++;
			}
		}

		return load_finished == clients.size();
	}

	void set_turn_done(const Message* message)
	{
		std::fill_n(hit_count[message->room_id], std::size(hit_count[message->room_id]), 0);
		turn_end[message->room_id][message->player_id] = true;
	}

	void clear_turn_done(const RoomID room_id)
	{
		std::fill(std::begin(turn_end[room_id]), std::end(turn_end[room_id]), false);
	}

	void calculate_damage_and_reply(Message* message, const sockaddr_in& client_info)
	{
		auto* casted = reinterpret_cast<DamageMsg*>(message);
		auto& dd = double_damage_enabled[casted->room_id][casted->prj_owner_id];

		const float damage = calculate_damage(
			casted->shooter_type,
			casted->victim_type, 
			casted->prj_type, 
			hit_count[message->room_id][message->player_id],
			dd,
			Object::Property::projectile_hitbox_getter(),
			casted->prj_position,
			casted->ch_position);

		if(dd)
		{
			dd = false;
		}

		casted->damage = damage;
		const auto overwrite_message = create_prewritten_network_message<DamageMsg>(*casted);

		std::cout << " Damage : " << damage << std::endl;

		server_socket.send_message<DamageMsg>(&overwrite_message, client_info);
		broadcast<DamageMsg>(&overwrite_message);
	}

	void set_double_damage_flag(const Message* message)
	{
		auto* casted = reinterpret_cast<const ItemMsg*>(message);

		if(casted->item_type == eItemType::DoubleDamage)
		{
			double_damage_enabled[casted->room_id][casted->player_id] = true;
		}
	}

	[[noreturn]] void consume_message()
	{
		while(true) 
		{
			char buffer[max_packet_size + 1]{};
			sockaddr_in client_info{};
			std::time_t time;

			if(!server_socket.get_any_message(&client_info, time, buffer))
			{
				server_socket.block_until_queue_event();
			}

			const Message* message = reinterpret_cast<Message*>(buffer);
			auto* writable_message = reinterpret_cast<Message*>(buffer);

			switch(message->type)
			{
			case eMessageType::GO:
			case eMessageType::NOGO:
			case eMessageType::LobbyInfo:
			case eMessageType::RoomInfo:
			case eMessageType::GameInit:
			case eMessageType::ReqDeltaTime:
			case eMessageType::GameStart:
			case eMessageType::RspWind:
			case eMessageType::PONG: break;
			case eMessageType::PING:
				add_client(message->room_id, message->player_id, client_info, time);
				reply_ping(client_info);
				break;
			case eMessageType::Position:
				// @todo: verification?
				std::cout << "Message type: Position ";
				broadcast<PositionMsg>(message);
				break;
			case eMessageType::ProjectileSelect:
				std::cout << "Message type: Projectile Selection ";
				broadcast<ProjectileSelectMsg>(message);
				break;
			case eMessageType::Stop:
				std::cout << "Message type: Stop ";
				broadcast<StopMsg>(message);
				break;
			case eMessageType::Firing:
				std::cout << "Message type: Firing ";
				broadcast<FiringMsg>(message);
				break;
			case eMessageType::Fire:
				std::cout << "Message type: Fire ";
				broadcast<CharacterFireMsg>(message);
				break;
			case eMessageType::Item:
				std::cout << "Message type: Item ";
				set_double_damage_flag(message);
				broadcast<ItemMsg>(message);
				break;
			case eMessageType::ItemFire:
				std::cout << "Message type: Item Fire ";
				broadcast<ItemFireMsg>(message);
				break;
			case eMessageType::ProjectileFire:
				std::cout << "Message type : Projectile Fire";
				broadcast<ProjectileFireMsg>(message);
				break;
			case eMessageType::ProjectileFlying:
				std::cout << "Message type : Projectile Flying";
				broadcast<ProjectileFlyingMsg>(message);
				break;
			case eMessageType::ProjectileHit:
				std::cout << "Message type : Projectile Hit ";
				broadcast<ProjectileHitMsg>(message);
				break;
			case eMessageType::TurnEnd:
				// @todo: check previous message.
				std::cout << "Turn ended" << std::endl;
				set_turn_done(message);
				if(check_turn_done(message->room_id))
				{
					reset_wind(message->room_id);
					clear_turn_done(message->room_id);
					send_go(message, message->room_id);
				}
				break;
			case eMessageType::Damage:
				std::cout << "Message type : Damage ";
				calculate_damage_and_reply(writable_message, client_info);
				break;
			case eMessageType::ReqWind:
				std::cout << "Got wind request" << std::endl;
				send_wind(message, client_info);
				break;
			case eMessageType::Destroyed:
				break;
			case eMessageType::LobbyJoin:
				add_client(message->room_id, message->player_id, client_info, time);
				add_player(message->player_id);
				std::cout << "New player joined. Broadcasting new lobby info." << std::endl;
				broadcast_lobby_info();
				break;
			case eMessageType::RoomJoin:
				remove_client_from_lobby(message->player_id);
				broadcast_lobby_info();
				add_player_to_room(
					message->room_id, 
					message->player_id,
					Client 
					{
						client_info,
						message->player_id,
						time
					});
				send_room_info(message->room_id, client_info);
				notify_join(message->room_id);
				break;
			case eMessageType::RoomSelectCh:
				change_character(message);
				break;
			case eMessageType::RoomSelectIt:
				change_items(message);
				break;
			case eMessageType::RoomStart:
				if(check_priority(message->room_id, message->player_id))
				{
					aggregate_send_room_info(message);
				}
				break;
			case eMessageType::LoadDone:
				if(add_load_done(message))
				{
					game_gtg(message->room_id);
				}
				break;
			case eMessageType::DeltaTime:
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