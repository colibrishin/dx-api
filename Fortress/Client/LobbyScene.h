#pragma once
#ifndef LOBBYSCENE_H
#define LOBBYSCENE_H

#include "../Common/ImageWrapper.hpp"
#include "../Common/sceneManager.hpp"
#include "../Common/sound.hpp"
#include "../Common/scene.hpp"
#include "../Common/message.hpp"

namespace Fortress::Scene
{
	class LobbyScene final : public Abstract::scene
	{
	public:
		LobbyScene() : scene(L"Lobby Scene")
		{
			initialize();
		}

		void initialize() override;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;

	private:
		Network::LobbyInfo m_lobby_info_;
		std::weak_ptr<ImageWrapper> m_imBackground;
		std::weak_ptr<Resource::Sound> m_bgm;
	};
}

#endif
