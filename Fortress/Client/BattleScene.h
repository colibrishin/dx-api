#pragma once
#ifndef CHARACTERSCENE_H
#define CHARACTERSCENE_H

#include "character.hpp"
#include "ground.hpp"
#include "Round.hpp"
#include "sceneManager.hpp"

namespace Fortress::Scene
{
	class BattleScene final : public Abstract::scene
	{
	public:
		BattleScene() : scene(L"Character Scene"), m_round()
		{
		}

		void initialize() override;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;

	private:
		std::weak_ptr<ObjectBase::character> m_home_object;
		std::weak_ptr<ObjectBase::character> m_away_object;
		std::weak_ptr<Object::Ground> m_ground;
		std::weak_ptr<ImageWrapper> m_hud;
		std::weak_ptr<ImageWrapper> m_background;
		Round m_round;
	};
}

#endif
