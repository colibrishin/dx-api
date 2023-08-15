#ifndef INPUT_H
#define INPUT_H
#pragma once

#include <execution>
#include "common.h"

namespace Fortress
{
	enum class eKeyCode
	{
		Q,
		W,
		E,
		R,
		T,
		Y,
		U,
		I,
		O,
		P,
		A,
		S,
		D,
		F,
		G,
		H,
		J,
		K,
		L,
		Z,
		X,
		C,
		V,
		B,
		N,
		M,
		SPACE,
		ENTER,
		UP,
		DOWN,
		LEFT,
		RIGHT,
		TAB,
		SHIFT,
		Zero,
		One,
		Two,
		Three,
		Four,
		Five,
		Six,
		Seven,
		Eight,
		Nine,
		F12,
	};

	/**
	 * \brief Internal code for key status. Use getKey series functions for get status.
	 */
	enum class _eKeyState
	{
		Down = 0,
		Up,
		Pressing,
		None,
	};

	constexpr static uint8_t KEY_TABLE[]
	{
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
		'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
		'Z', 'X', 'C', 'V', 'B', 'N', 'M',
		VK_SPACE, VK_RETURN, VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, VK_TAB, VK_SHIFT,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, /* 0 to 9*/
		VK_F12
	};

	/**
	 * \brief A static class to manage whether key is pressed or vice versa.
	 */
	class Input final
	{
	public:
		Input() = delete;
		Input& operator=(const Input& other) = delete;
		Input& operator=(Input&& other) = delete;
		~Input() = delete;
		Input(const Input& other) = delete;
		Input(const Input&& other) = delete;

		struct Key
		{
			uint8_t m_native_code;
			_eKeyState m_state;
			bool m_pressed;

			Key()
			{
				m_native_code = 0;
				m_state = _eKeyState::None;
				m_pressed = false;
			}

			explicit Key(const uint8_t native_code)
			{
				m_native_code = native_code;
				m_state = _eKeyState::None;
				m_pressed = false;
			}

			Key& operator=(const Key& other) = default;
		};

		inline static void initialize();
		inline static void update();
		__forceinline static bool getKeyDown(eKeyCode);
		__forceinline static bool getKeyUp(eKeyCode);
		__forceinline static bool getKey(eKeyCode);

	private:
		inline static std::vector<Key> m_keys;

		inline static void checkKeyState();
	};

	void Input::initialize()
	{
		// building initial key table
		for (unsigned char i : KEY_TABLE)
		{
			m_keys.emplace_back(i);
		}
	}

	/**
	 * \brief A key state update function.
	 * This function executed in parallel, so if any other parallel access is needed, then this requires a mutex lock.
	 */
	void Input::checkKeyState()
	{
		std::for_each(
			std::execution::par,
			m_keys.begin(),
			m_keys.end(),
			[](Key& key)
			{
				USHORT winapi_state = GetAsyncKeyState(key.m_native_code);

				if (winapi_state & 0x8001)
				{
					if (key.m_pressed)
					{
						key.m_state = _eKeyState::Pressing;
					}
					else
					{
						key.m_state = _eKeyState::Down;
						key.m_pressed = true;
					}
				}
				else
				{
					if (key.m_pressed)
					{
						key.m_state = _eKeyState::Up;
						key.m_pressed = false;
					}
					else
					{
						key.m_state = _eKeyState::None;
					}
				}
			});
	}

	/**
	 * \brief Updates the key input status.
	 */
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
