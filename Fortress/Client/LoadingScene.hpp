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
#include <thread>

namespace Fortress::Scene
{
	template <typename MapName>
	class LoadingScene final : public Abstract::scene
	{
	public:
		LoadingScene() : scene(L"Loading Scene")
		{
			initialize();
		}

		void initialize() override;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;

		void load_scene() const;

		std::thread m_initialization_thread;
		std::weak_ptr<ImageWrapper> m_imBackground;
		std::weak_ptr<Resource::Sound> m_bgm;
	};

	template <typename MapName>
	void LoadingScene<MapName>::initialize()
	{
		scene::initialize();
		m_imBackground = Resource::ResourceManager::load<ImageWrapper>(
			L"Loading", "./resources/misc/loading/loading.png");
		m_bgm = Resource::ResourceManager::load<Resource::Sound>(
			L"Loading BGM", "./resources/sounds/loading.wav");

		// we are calling the member function, so this has to be given for the argument.
		m_initialization_thread = std::thread(&LoadingScene::load_scene, this);
	}

	template <typename MapName>
	void LoadingScene<MapName>::update()
	{
		scene::update();
		// using this as thread might be handy later on.
		if(m_initialization_thread.joinable())
		{
			m_initialization_thread.join();
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

	template <typename MapName>
	void LoadingScene<MapName>::load_scene() const
	{
		static float fixed_frame_wait = 0.0f;

		SceneManager::CreateScene<MapName>();

		while(fixed_frame_wait < 20.0f)
		{
			fixed_frame_wait += DeltaTime::get_deltaTime();
		}

		SceneManager::SetActive<MapName>();
		fixed_frame_wait = 0.0f;
	}
}

#endif
