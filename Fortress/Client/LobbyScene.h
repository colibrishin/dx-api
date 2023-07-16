#pragma once
#ifndef LOBBYSCENE_H
#define LOBBYSCENE_H

#include "ImageWrapper.hpp"
#include "sceneManager.hpp"

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
		std::shared_ptr<ImageWrapper> m_imBackground;
	};
}

#endif