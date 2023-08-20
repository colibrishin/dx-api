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
		PING = 0x10,
		PONG = 0x11,

		// Validation
		GO = 0x20,
		NOGO = 0x21,

		// interpolation
		DeltaTime = 0x30,
		ReqDeltaTime = 0x31,

		LobbyJoin = 0x40,
		LobbyInfo = 0x41,

		RoomJoin = 0x50,
		RoomInfo = 0x51,
		RoomStart = 0x53,
		RoomSelectCh = 0x54,
		RoomSelectIt = 0x55,

		GameInit = 0x60,
		LoadDone = 0x62,
		GameStart = 0x63,

		Position,
		Fire,
		Item,
		Hit,
		Destroyed,
		Wind,
	};

	enum class eCharacterType
	{
		None = 0x10,
		CannonCharacter = 0x11,
		MissileCharacter = 0x12,
		SecwindCharacter = 0x13,
	};

	enum class eItemType
	{
		None = 0x10,
		DoubleShot = 0x11,
		Teleport = 0x12,
		Repair = 0x13,
		DoubleDamage = 0x14
	};

	enum class eMapType
	{
		DesertMap = 0x10,
		SkyValleyMap = 0x11,
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
		float deltaTime;
	};

	struct ReqDeltaTimeMsg : Message
	{
		CRC32 last_message;
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

	struct RoomSelectCh : Message
	{
		eCharacterType ch_type{};
	};

	struct RoomSelectIt : Message
	{
		eItemType items[4]{};
	};

	struct RoomStart : Message
	{
		eMapType map_type;
	};

	struct GameInitMsg : Message
	{
		uint8_t player_count;
		PlayerID players[15];
		eMapType map_type;
		eCharacterType character_type[15];
		eItemType equied_item [15][4];
	};

	struct LoadDoneMsg : Message
	{
	};

	struct GameStartMsg : Message
	{
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
		ReqDeltaTimeMsg rdt;
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
		RoomSelectCh room_ch;
		RoomSelectIt room_it;
		GameInitMsg init;
		LoadDoneMsg load_done;
		GameStartMsg start;
		GameInitMsg game_init;
	};

	// UDP Packet size limit
	// @todo: send separately.
	static_assert(sizeof(Data) <= 65507);

	template <typename T, typename... Args>
	static T create_network_message(Args... args)
	{
		T msg{0, args...};
		msg.crc32 = get_crc32(msg);
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