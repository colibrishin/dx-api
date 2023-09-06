#pragma once
#pragma once
#ifndef LOADINGSCENE_H
#define LOADINGSCENE_H

#include "../Common/ImageWrapper.hpp"
#include "../Common/resourceManager.hpp"
#include "../Common/scene.hpp"
#include "../Common/sceneManager.hpp"
#include "../Common/sound.hpp"
#include "../Common/deltatime.hpp"
#include "../Common/debug.hpp"

namespace Fortress::Scene
{
	template <typename MapName>
	class LoadingScene final : public Abstract::scene
	{
	public:
		LoadingScene(const Network::GameInitMsg& game_info) :
		scene(L"Loading Scene"), m_game_init(game_info)
		{
			initialize();
		}

		void initialize() override;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;
		
		std::weak_ptr<ImageWrapper> m_imBackground;
		std::weak_ptr<Resource::Sound> m_bgm;
		Network::GameInitMsg m_game_init;
	};

	template <typename MapName>
	void LoadingScene<MapName>::initialize()
	{
		scene::initialize();
		m_imBackground = Resource::ResourceManager::load<ImageWrapper>(
			L"Loading", "./resources/misc/loading/loading.png");
		m_bgm = Resource::ResourceManager::load<Resource::Sound>(
			L"Loading BGM", "./resources/sounds/loading.wav");
	}

	template <typename MapName>
	void LoadingScene<MapName>::update()
	{
		scene::update();

		static float fixed_frame_wait = 0.0f;

		static bool load_finished = false;
		static bool handshake_finished = false;

		if(!load_finished)
		{
			SceneManager::CreateScene<MapName>(m_game_init);

			load_finished = true;
		}

		if(!handshake_finished)
		{
			if(fixed_frame_wait < 1.5f)
			{
				fixed_frame_wait += DeltaTime::get_deltaTime();
				return;
			}

			fixed_frame_wait = 0.0f;
			handshake_finished = true;

			EngineHandle::get_messenger()->call_loading_finished();
		}

		static Network::GameStartMsg gsm{};

		Debug::Log(L"Waiting for other clients...");

		if(EngineHandle::get_messenger()->check_game_start(gsm) || 
			gsm.type == Network::eMessageType::GameStart)
		{
			SceneManager::SetActive<MapName>();
			load_finished = false;
			handshake_finished = false;
			fixed_frame_wait = 0.0f;
		}
	}

	template <typename MapName>
	void LoadingScene<MapName>::render()
	{
		scene::render();

		m_imBackground.lock()->render({0, -20.f}, m_imBackground.lock()->get_hitbox());
	}

	template <typename MapName>
	void LoadingScene<MapName>::deactivate()
	{
		scene::deactivate();
		m_bgm.lock()->stop(true);
	}

	template <typename MapName>
	void LoadingScene<MapName>::activate()
	{
		scene::activate();
		m_bgm.lock()->play(true);
	}
}

#endif
