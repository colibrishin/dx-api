#pragma once
#ifndef SUMMARYSCENE_H
#define SUMMARYSCENE_H

#include "ImageWrapper.hpp"
#include "Round.hpp"
#include "sceneManager.hpp"
#include "sound.hpp"

namespace Fortress::Scene
{
	class SummaryScene final : public Abstract::scene
	{
	public:
		SummaryScene(std::shared_ptr<Round>& round) : scene(L"Summary Scene")
		{
			m_round = std::move(round);
		}

		void initialize() override;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;

		std::weak_ptr<ImageWrapper> m_imBackground;
		std::weak_ptr<Resource::Sound> m_bgm;
		std::shared_ptr<Round> m_round;
	};

	inline void SummaryScene::initialize()
	{
		scene::initialize();
		m_imBackground = Resource::ResourceManager::load<ImageWrapper>(
			L"Game Summary", "./resources/misc/summary/summary.png");
		m_bgm = Resource::ResourceManager::load<Resource::Sound>(
			L"Summary BGM", "./resources/sounds/game_end.wav");
	}

	inline void SummaryScene::update()
	{
		scene::update();

		if(Input::getKeyDown(eKeyCode::ENTER))
		{
			SceneManager::SetActive(L"Room Scene");
			SceneManager::remove_scene<SummaryScene>();
		}
	}

	inline void SummaryScene::render()
	{
		scene::render();
		m_imBackground.lock()->render({0, -20.f}, m_imBackground.lock()->get_hitbox());
	}

	inline void SummaryScene::deactivate()
	{
		scene::deactivate();
		m_bgm.lock()->stop(true);
	}

	inline void SummaryScene::activate()
	{
		m_bgm.lock()->play(false);
		scene::activate();
	}
}

#endif
