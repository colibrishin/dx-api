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
	class BattleScene : public Abstract::scene
	{
	public:
		BattleScene(const std::wstring& name) : scene(L"Battle Scene " + name), m_round(), m_radar()
		{
		}

		virtual void initialize() override;
		virtual void pre_initialize() = 0;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;
		const Round& get_round_status();

		virtual void set_bgm() = 0;
		virtual void set_background_img() = 0;
		virtual void set_grounds() = 0;
		virtual void set_characters() = 0;
		virtual void set_client_character() = 0;

	protected:
		std::weak_ptr<ObjectBase::character> m_self;
		std::vector<std::weak_ptr<ObjectBase::character>> m_characters;
		std::vector<std::weak_ptr<Object::Ground>> m_grounds;
		std::weak_ptr<Resource::Sound> m_bgm;
		std::weak_ptr<ImageWrapper> m_hud;
		std::weak_ptr<ImageWrapper> m_background;
		Round m_round;
		Radar m_radar;
	};
}

#endif
