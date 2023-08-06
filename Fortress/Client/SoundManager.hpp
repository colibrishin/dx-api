#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP
#include <mmsystem.h>
#include <dsound.h>
#include <dinput.h>
#include <exception>

#include "winapihandles.hpp"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dsound.lib")


namespace Fortress
{
	namespace Resource
	{
		class Sound;
	}

	class SoundManager
	{
	public:
		static LPDIRECTSOUND8 get_device()
		{
			return m_sound_device;
		}

		inline static bool initialize()
		{
			if (FAILED(DirectSoundCreate8(NULL, &m_sound_device, NULL)))
			{
				throw std::exception("Failed to create direct sound.");
			}
			
			if (FAILED(m_sound_device->SetCooperativeLevel(WinAPIHandles::get_hwnd(), DISCL_EXCLUSIVE))) // Flag�� ����
			{
				throw std::exception("Unable to set cooperative level.");
			}

			return true;
		}

		inline static void cleanup()
		{
			m_sound_device->Release();
		}

	private:
		inline static LPDIRECTSOUND8 m_sound_device = nullptr;
	};
}
#endif // SOUNDMANAGER_HPP
