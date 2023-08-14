#pragma once
#ifndef BULLETINBOARDSCENE_H
#define BULLETINBOARDSCENE_H

#include "ImageWrapper.hpp"
#include "sceneManager.hpp"
#include "sound.hpp"
#include "scene.hpp"

namespace Fortress::Scene
{
	class BulletinBoardScene final : public Abstract::scene
	{
	public:
		BulletinBoardScene() : scene(L"Bulletin Board Scene")
		{
			initialize();
		}

		void initialize() override;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;

		std::weak_ptr<ImageWrapper> m_imBackground;
		std::weak_ptr<Resource::Sound> m_sound_bgm;
	};
}

#endif
