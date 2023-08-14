#pragma once
#ifndef ROOMSCENE_H
#define ROOMSCENE_H

// Ah, I cannot forget the room scene music. it was tense.

#include "ImageWrapper.hpp"
#include "sceneManager.hpp"
#include "sound.hpp"
#include "scene.hpp"

namespace Fortress::Scene
{
	class RoomScene final : public Abstract::scene
	{
	public:
		RoomScene() : scene(L"Room Scene")
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
	};
}

#endif
