#pragma once
#ifndef TITLESCENE_H
#define TITLESCENE_H

#include "character.hpp"
#include "ImageWrapper.hpp"
#include "sceneManager.hpp"

namespace Fortress::Scene
{
	class TitleScene final : public Abstract::scene
	{
	public:
		TitleScene() : scene(L"Title Scene")
		{
		}

		void initialize() override;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;

		ImageWrapper* m_imBackground;
	};
}

#endif
