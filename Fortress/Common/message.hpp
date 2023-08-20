#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <windows.h>
#include "Crc32.h"
#include "vector2.hpp"

#undef max

namespace Fortress::Network
{
	enum class eObjectType
	{
		Character,
		Projectile,
		Ground
	};

	enum class eMessageType
	{
		// Latency
		PING = 0x0,
		PONG = 0x1,

		// Validation
		GO = 0x10,
		NOGO = 0x11,

		// interpolation
		DeltaTime = 0x20,

		LobbyJoin = 0x30,
		LobbyInfo = 0x31,

		RoomJoin = 0x40,
		RoomInfo = 0x41,

		Position,
		Fire,
		Item,
		Hit,
		Destroyed,
		Wind,
	};

	enum class eItemType
	{
		DoubleShot,
		Teleport,
		Repair,
		DoubleDamage
	};

	using PlayerID = int;
	using RoomID = int;
	using CRC32 = uint32_t;

	struct Room
	{
		char name[15][5];
	};

	struct Message
	{
		CRC32 crc32;
		eMessageType type;
		RoomID room_id;
		PlayerID player_id;
	};

	struct PingMsg : Message
	{
	};

	struct PongMsg : Message
	{
	};

	struct DeltaTimeMsg : Message
	{
		LARGE_INTEGER deltaTime;
	};

	struct LobbyJoin : Message
	{
	};

	struct LobbyInfo : Message
	{
		int room_count;
		Room room_info[15];
		int player_count;
		wchar_t player_names[15][15];
	};

	struct RoomJoin : Message
	{
	};

	struct RoomInfo : Message
	{
		wchar_t room_name[15];
		int player_count;
		wchar_t player_names[15][15];
	};

	struct GOMsg : Message
	{
		CRC32 last_message;
	};

	struct NOGOMsg : Message
	{
		CRC32 last_message;
	};

	struct PositionMsg : Message
	{
		eObjectType object_type;
		Math::Vector2 position;
	};

	struct FireMsg : Message
	{
		eObjectType object_type;
		Math::Vector2 position;
	};

	struct ItemMsg : Message
	{
		eObjectType object_type;
		eItemType item_type;
	};

	struct HitMsg : Message
	{
		eObjectType object_type;
		PlayerID object_player_id;
		Math::Vector2 position;
		float damage;
	};

	struct DestroyedMsg : Message
	{
		eObjectType object_type;
		PlayerID object_player_id;
	};
	
	struct WindMsg : Message
	{
		float wind;
	};

	union Data final
	{
		PingMsg ping;
		PongMsg pong;
		DeltaTimeMsg dt;
		GOMsg go;
		NOGOMsg nogo;
		PositionMsg pos;
		FireMsg fire;
		ItemMsg item;
		HitMsg hit;
		DestroyedMsg destroyed;
		WindMsg wind;
		LobbyJoin lobby_join;
		LobbyInfo lobby_info;
		RoomJoin room_join;
		RoomInfo room_info;
	};

	// UDP Packet size limit
	// @todo: send separately.
	static_assert(sizeof(Data) <= 65507);

	template <typename T, typename... Args>
	static T create_network_message(Args... args)
	{
		T msg{0, args...};
		msg.crc32 = crc32_fast(reinterpret_cast<char*>(&msg) + sizeof(CRC32), sizeof(T) - sizeof(CRC32));
		return msg;
	}

	template <typename T>
	static CRC32 get_crc32(const T& msg)
	{
		return crc32_fast(
			reinterpret_cast<const char*>(&msg) + sizeof(CRC32), 
			sizeof(T) - sizeof(CRC32));
	}

	template <typename T, typename... Args>
	static T create_network_message(T& pre_written_message)
	{
		pre_written_message.crc32 = get_crc32(pre_written_message);
		return pre_written_message;
	}
	
	constexpr unsigned int max_packet_size = sizeof(Data);
}
#endif // MESSAGE_HPP
