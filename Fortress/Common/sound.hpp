#ifndef SOUND_HPP
#define SOUND_HPP
#include <cassert>

#include "common.h"
#include "resource.hpp"
#include <mmsystem.h>
#include <dsound.h>
#include <dinput.h>

#include "SoundManager.hpp"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dsound.lib")

namespace Fortress::Resource
{
	class Sound final : public Abstract::Resource
	{
	public:
		Sound(const std::wstring& name, const std::filesystem::path& file_path)
			: Resource(name, file_path), m_sound_buffer(nullptr), m_buffer_desc{}, m_volume(0), m_bPlaying(false)
		{
			initialize();
		}
		~Sound() override;
		void initialize();
		virtual bool load() override;
		void play(bool loop);
		void stop(bool reset);
		void set_position(float position, bool loop);
		void set_volume(float volume);
		int get_decibel(float volume) const;
		bool is_playing() const;

	private:
		bool load_wav_file(const std::filesystem::path& path);

		LPDIRECTSOUNDBUFFER	m_sound_buffer;
		DSBUFFERDESC m_buffer_desc;
		int m_volume;
		bool m_bPlaying;
	};

	inline Sound::~Sound()
	{
		stop(true);
		m_sound_buffer->Release();
	}

	inline void Sound::initialize()
	{
		load();
	}

	inline bool Sound::load()
	{
		if (nullptr == SoundManager::get_device())
			assert(nullptr);

		if (get_path().extension() == ".wav")
		{
			if (false == load_wav_file(get_path()))
				assert(nullptr);
		}
		else
			assert(nullptr);

		return true;
	}

	inline bool Sound::load_wav_file(const std::filesystem::path& path)
	{
		//CreateFile
		const HMMIO hFile = mmioOpen(
			const_cast<wchar_t*>(path.native().c_str()), 
			nullptr, 
			MMIO_READ);

		if (nullptr == hFile)
		{
			throw std::exception(
				(std::string("Unable to load a sound file ") + path.generic_string()).data());
		}

		MMCKINFO pParent{};
		pParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		mmioDescend(hFile, &pParent, NULL, MMIO_FINDRIFF);

		MMCKINFO pChild{};
		pChild.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioDescend(hFile, &pChild, &pParent, MMIO_FINDCHUNK);

		WAVEFORMATEX wft{};
		mmioRead(hFile, reinterpret_cast<char*>(&wft), sizeof(wft));

		mmioAscend(hFile, &pChild, 0);
		pChild.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mmioDescend(hFile, &pChild, &pParent, MMIO_FINDCHUNK);

		memset(&m_buffer_desc, 0, sizeof(DSBUFFERDESC));
		m_buffer_desc.dwBufferBytes = pChild.cksize;
		m_buffer_desc.dwSize = sizeof(DSBUFFERDESC);
		m_buffer_desc.dwFlags = DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLVOLUME;
		m_buffer_desc.lpwfxFormat = &wft;

		if (FAILED(SoundManager::get_device()->CreateSoundBuffer(&m_buffer_desc, &m_sound_buffer, NULL)))
		{
			throw std::exception("Unable to allocate the buffer for sound.");
		}

		void* pWrite1 = nullptr;
		void* pWrite2 = nullptr;
		DWORD dwlength1, dwlength2;

		m_sound_buffer->Lock(0, pChild.cksize, &pWrite1, &dwlength1
			, &pWrite2, &dwlength2, 0L);

		if (pWrite1 != nullptr)
			mmioRead(hFile, static_cast<char*>(pWrite1), dwlength1);
		if (pWrite2 != nullptr)
			mmioRead(hFile, static_cast<char*>(pWrite2), dwlength2);

		m_sound_buffer->Unlock(pWrite1, dwlength1, pWrite2, dwlength2);

		mmioClose(hFile, 0);

		set_volume(50.f);

		return true;
	}

	inline void Sound::play(bool loop)
	{
		m_sound_buffer->SetCurrentPosition(0);

		if (loop)
			m_sound_buffer->Play(0, 0, DSBPLAY_LOOPING);
		else
			m_sound_buffer->Play(0, 0, 0);

		m_bPlaying = true;
	}

	inline void Sound::stop(bool reset)
	{
		m_sound_buffer->Stop();

		if (reset)
			m_sound_buffer->SetCurrentPosition(0);

		m_bPlaying = false;
	}

	inline void Sound::set_position(float position, bool loop)
	{
		stop(true);

		const auto dwBytes = static_cast<DWORD>((position / 100.f) * static_cast<float>(m_buffer_desc.dwBufferBytes));
		m_sound_buffer->SetCurrentPosition(dwBytes);

		if (loop)
			m_sound_buffer->Play(0, 0, DSBPLAY_LOOPING);
		else
			m_sound_buffer->Play(0, 0, 0);

		m_bPlaying = true;
	}

	inline void Sound::set_volume(float volume)
	{
		m_volume = get_decibel(volume);
		m_sound_buffer->SetVolume(m_volume);
	}

	inline int Sound::get_decibel(float volume) const
	{
		if (volume > 100.f)
			volume = 100.f;
		else if (volume <= 0.f)
			volume = 0.00001f;

		int iVolume = (LONG)(-2000.0 * log10(100.f / volume));

		if (iVolume < -10000)
			iVolume = -10000;
		return iVolume;
	}

	inline bool Sound::is_playing() const
	{
		return m_bPlaying;
	}
}
#endif // SOUND_HPP
