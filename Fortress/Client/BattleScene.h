#pragma once
#ifndef CHARACTERSCENE_H
#define CHARACTERSCENE_H

#include "character.hpp"
#include "ground.hpp"
#include "Radar.hpp"
#include "Round.hpp"
#include "sceneManager.hpp"
#include "sound.hpp"

namespace Fortress::Scene
{
	class BattleScene final : public Abstract::scene
	{
	public:
		BattleScene() : scene(L"Character Scene"), m_round(), m_radar()
		{
		}

		void initialize() override;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;
		const Round& get_round_status();

	private:
		std::weak_ptr<ObjectBase::character> m_home_object;
		std::weak_ptr<ObjectBase::character> m_away_object;
		std::weak_ptr<Object::Ground> m_ground;
		std::weak_ptr<Resource::Sound> m_bgm;
		std::weak_ptr<ImageWrapper> m_hud;
		std::weak_ptr<ImageWrapper> m_background;
		Round m_round;
		Radar m_radar;
	};
}

#endif
