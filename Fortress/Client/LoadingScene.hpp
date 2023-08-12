#pragma once
#pragma once
#ifndef LOADINGSCENE_H
#define LOADINGSCENE_H

#include "ImageWrapper.hpp"
#include "resourceManager.hpp"
#include "scene.hpp"
#include "sceneManager.hpp"
#include "sound.hpp"

namespace Fortress::Scene
{
	template <typename MapName>
	class LoadingScene final : public Abstract::scene
	{
	public:
		LoadingScene() : scene(L"Loading Scene")
		{
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
		m_initialization_thread.join();
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
		SceneManager::CreateScene<MapName>();
		SceneManager::SetActive<MapName>();
	}
}

#endif