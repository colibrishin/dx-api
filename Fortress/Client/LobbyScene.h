#pragma once
#ifndef LOBBYSCENE_H
#define LOBBYSCENE_H

#include "ImageWrapper.hpp"
#include "sceneManager.hpp"
#include "sound.hpp"
#include "scene.hpp"

namespace Fortress::Scene
{
	class LobbyScene final : public Abstract::scene
	{
	public:
		LobbyScene() : scene(L"Lobby Scene")
		{
		}

		void initialize() override;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;

	private:
		std::weak_ptr<ImageWrapper> m_imBackground;
		std::weak_ptr<Resource::Sound> m_bgm;
	};
}

#endif
