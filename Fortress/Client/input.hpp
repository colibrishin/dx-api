#ifndef INPUT_H
#define INPUT_H
#pragma once

#include <map>
#include "framework.h"

namespace Fortress 
{
	enum class eKeyCode 
	{
		_START = 0, 
		Q,W,E,R,T,Y,U,I,O,P,
		A,S,D,F,G,H,J,K,L,
		Z,X,C,V,B,N,M,
		_ASCII_TRAILLING,
		SPACE,ENTER,
		UP,DOWN,LEFT,RIGHT,
		_TRAILLING
	};

	enum class eKeyState 
	{
		Down = 0,
		Up,
		Pressing,
		None,
	};

	constexpr static uint8_t ASCII_TABLE[]
	{
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
		'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
		'Z', 'X', 'C', 'V', 'B', 'N', 'M'
	};

	constexpr static uint8_t SPECIAL_KEY_TABLE[] 
	{
		VK_SPACE, VK_RETURN, VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT
	};

	class Input final
	{
	public:
		struct Key final
		{
			eKeyCode key;
			eKeyState state;
		};

		inline static void initialize();
		inline static void update();
		__forceinline static bool getKeyDown(const eKeyCode);
		__forceinline static bool getKeyUp(const eKeyCode);
		__forceinline static bool getKey(const eKeyCode);
	private:
		static std::map<eKeyCode, Key> m_keys;

		inline static void checkKeyState(const eKeyCode, const eKeyCode, const uint8_t*, const size_t);
	};

	// ** LINK2001 ERROR WARNING **
	// FORWARD DECLARATION SHOULD BE DONE BEFORE USED.
	std::map<Fortress::eKeyCode, Fortress::Input::Key> Fortress::Input::m_keys = {};

	void Fortress::Input::initialize()
	{
		for(size_t i = 0; i < (size_t)eKeyCode::_ASCII_TRAILLING; ++i) 
		{
			m_keys[(eKeyCode)i] = {(eKeyCode)i, eKeyState::None};
		}

		for(size_t i = (size_t)eKeyCode::_ASCII_TRAILLING + 1; i < (size_t)eKeyCode::_TRAILLING; ++i)
		{
			m_keys[(eKeyCode)i] = {(eKeyCode)i, eKeyState::None};
		}
	}

	void Fortress::Input::checkKeyState(const eKeyCode leading, const eKeyCode trailing, const uint8_t* table, const size_t table_size)
	{
		if((size_t)trailing - (size_t)leading != table_size) 
		{
			throw std::exception("input error: key state table mismatch");
		}

		int table_i = 0;

		// @todo: maybe parallel?
		for(size_t i = (size_t)leading; i < (size_t)trailing; ++i) 
		{
			Key& target = m_keys[(eKeyCode)i];
			USHORT winapi_state = GetAsyncKeyState(table[table_i]);

			switch(winapi_state) 
			{
			case 0x0000:
				target.state = eKeyState::None;
				break;
			case 0x0001:
				target.state = eKeyState::Up;
				break;
			case 0x8000:
				target.state = eKeyState::Down;
				break;
			case 0x8001:
				target.state = eKeyState::Pressing;
				break;
			default:
				throw std::exception("winapi error : unknown GetASyncKeyState return");
				break;
			}

			table_i++;
		}
	}

	void Fortress::Input::update()
	{
		eKeyCode exclude_leading = (eKeyCode)((size_t)eKeyCode::_START + 1);

		checkKeyState(exclude_leading, eKeyCode::_ASCII_TRAILLING, ASCII_TABLE, sizeof(ASCII_TABLE));

		eKeyCode exclude_trailing = (eKeyCode)((size_t)eKeyCode::_ASCII_TRAILLING + 1);

		checkKeyState(exclude_trailing, eKeyCode::_TRAILLING, SPECIAL_KEY_TABLE, sizeof(SPECIAL_KEY_TABLE));
	}

	bool Fortress::Input::getKeyDown(const eKeyCode code)
	{
		return m_keys[code].state == eKeyState::Down;
	}

	bool Fortress::Input::getKeyUp(const eKeyCode code)
	{
		return m_keys[code].state == eKeyState::Up;
	}

	bool Fortress::Input::getKey(const eKeyCode code)
	{
		return m_keys[code].state == eKeyState::Pressing;
	}
}

#endif