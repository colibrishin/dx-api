#pragma once
#ifndef BULLETINBOARDSCENE_H
#define BULLETINBOARDSCENE_H

#include "ImageWrapper.hpp"
#include "sceneManager.hpp"

namespace Fortress::Scene
{
	class BulletinBoardScene final : public Abstract::scene
	{
	public:
		BulletinBoardScene() : scene(L"Bulletin Board Scene")
		{
		}

		void initialize() override;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;

		std::weak_ptr<ImageWrapper> m_imBackground;
	};
}

#endif
