#ifndef INPUT_H
#define INPUT_H
#pragma once

#include <execution>
#include "framework.h"

namespace Fortress
{
	enum class eKeyCode
	{
		Q,W,E,R,T,Y,U,I,O,P,
		A,S,D,F,G,H,J,K,L,
		Z,X,C,V,B,N,M,
		SPACE,ENTER,
		UP,DOWN,LEFT,RIGHT,
	};

	enum class _eKeyState
	{
		Down = 0,
		Up,
		Pressing,
		None,
	};

	constexpr static uint8_t _KEY_TABLE[]
	{
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
		'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
		'Z', 'X', 'C', 'V', 'B', 'N', 'M',
		VK_SPACE, VK_RETURN, VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT,
	};

	class Input final
	{
	public:
		struct Key
		{
			std::mutex _m_lock;
			uint8_t m_native_code;
			_eKeyState m_state;

			Key()
			{
				m_native_code = 0;
				m_state = _eKeyState::None;
			}

			explicit Key(const uint8_t native_code)
			{
				m_native_code = native_code;
				m_state = _eKeyState::None;
			}

			Key& operator=(const Key& other)
			{
				m_native_code = other.m_native_code;
				m_state = other.m_state;

				return *this;
			}
		};

		inline static void initialize();
		inline static void update();
		__forceinline static bool getKeyDown(eKeyCode);
		__forceinline static bool getKeyUp(eKeyCode);
		__forceinline static bool getKey(eKeyCode);

	private:
		static Key m_keys[sizeof _KEY_TABLE];

		inline static void checkKeyState();
	};


	// ** LINK2001 ERROR WARNING **
	// FORWARD DECLARATION SHOULD BE DONE BEFORE USED.
	Input::Key Input::m_keys[sizeof _KEY_TABLE];

	void Input::initialize()
	{
		// building initial key table
		for (size_t i = 0; i < sizeof _KEY_TABLE; ++i)
		{
			m_keys[i] = Key(_KEY_TABLE[i]);
		}
	}

	void Input::checkKeyState()
	{
		std::for_each(
			std::execution::par,
			std::begin(m_keys),
			std::end(m_keys),
			[](Key& key)
			{
				std::lock_guard lock_guard(key._m_lock);
				USHORT winapi_state = GetAsyncKeyState(key.m_native_code);

				switch (winapi_state)
				{
				case 0x0000:
					key.m_state = _eKeyState::None;
					break;
				case 0x0001:
					key.m_state = _eKeyState::Up;
					break;
				case 0x8000:
					key.m_state = _eKeyState::Down;
					break;
				case 0x8001:
					key.m_state = _eKeyState::Pressing;
					break;
				default:
					throw std::exception("winapi error : unknown GetASyncKeyState return");
				}
			});
	}

	void Input::update()
	{
		checkKeyState();
	}

	bool Input::getKeyDown(const eKeyCode code)
	{
		return m_keys[static_cast<size_t>(code)].m_state == _eKeyState::Down;
	}

	bool Input::getKeyUp(const eKeyCode code)
	{
		return m_keys[static_cast<size_t>(code)].m_state == _eKeyState::Up;
	}

	bool Input::getKey(const eKeyCode code)
	{
		return m_keys[static_cast<size_t>(code)].m_state == _eKeyState::Pressing;
	}
}

#endif
