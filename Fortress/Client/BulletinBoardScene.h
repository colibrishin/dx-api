#pragma once
#ifndef BULLETINBOARDSCENE_H
#define BULLETINBOARDSCENE_H

#include "../Common/ImageWrapper.hpp"
#include "../Common/sceneManager.hpp"
#include "../Common/sound.hpp"
#include "../Common/scene.hpp"

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
