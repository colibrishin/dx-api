#pragma once
#ifndef ROOMSCENE_H
#define ROOMSCENE_H

// Ah, I cannot forget the room scene music. it was tense.

#include "../Common/ImageWrapper.hpp"
#include "../Common/sceneManager.hpp"
#include "../Common/sound.hpp"
#include "../Common/scene.hpp"
#include "../Common/message.hpp"

namespace Fortress::Scene
{
	class RoomScene final : public Abstract::scene
	{
	public:
		RoomScene() : scene(L"Room Scene"), m_room_id(0)
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

		Network::RoomID m_room_id;
		Network::RoomInfo m_room_info;
	};
}

#endif
