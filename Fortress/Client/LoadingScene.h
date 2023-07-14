#pragma once
#ifndef LOADINGSCENE_H
#define LOADINGSCENE_H

#include "ImageWrapper.hpp"
#include "sceneManager.hpp"

namespace Fortress::Scene
{
	class LoadingScene final : public Abstract::scene
	{
	public:
		LoadingScene() : scene(L"Loading Scene")
		{
		}

		void initialize() override;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;

	private:
		ImageWrapper* m_imBackground;
	};
}

#endif
