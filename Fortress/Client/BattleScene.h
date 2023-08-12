#pragma once
#ifndef CHARACTERSCENE_H
#define CHARACTERSCENE_H

#include "common.h"
#include "character.hpp"
#include "ground.hpp"
#include "Radar.hpp"
#include "Round.h"
#include "sound.hpp"
#include "scene.hpp"

namespace Fortress::Scene
{
	class BattleScene : public Abstract::scene
	{
	public:
		BattleScene(const std::wstring& name, const Math::Vector2& map_size) :
			scene(L"Battle Scene " + name),
			m_map_size(map_size),
			m_round(std::make_shared<Round>()),
			m_radar(map_size)
		{
		}

		virtual void initialize() override;
		virtual void pre_initialize() = 0;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;
		std::weak_ptr<Round> get_round_status();
		const Math::Vector2& get_map_size() const;

		virtual void set_bgm() = 0;
		virtual void set_background_img() = 0;
		virtual void set_grounds() = 0;
		virtual void set_characters() = 0;
		virtual void set_client_character() = 0;
		virtual bool predicate_OOB(const Math::Vector2& position);
		virtual bool movable(const Math::Vector2& position);

	protected:
		Math::Vector2 m_map_size;
		std::weak_ptr<ObjectBase::character> m_self;
		std::vector<std::weak_ptr<ObjectBase::character>> m_characters;
		std::vector<GroundPointer> m_grounds;
		std::weak_ptr<Resource::Sound> m_bgm;
		std::weak_ptr<ImageWrapper> m_hud;
		std::weak_ptr<ImageWrapper> m_background;
		std::shared_ptr<Round> m_round;
		Radar m_radar;
	};
}

#endif
