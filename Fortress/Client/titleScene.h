#pragma once
#ifndef TITLESCENE_H
#define TITLESCENE_H

#include "../Common/character.hpp"
#include "../Common/ImageWrapper.hpp"
#include "../Common/scene.hpp"

namespace Fortress::Scene
{
	class TitleScene final : public Abstract::scene
	{
	public:
		TitleScene() : scene(L"Title Scene")
		{
			initialize();
		}

		void initialize() override;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;

		std::weak_ptr<ImageWrapper> m_imBackground;
		std::shared_ptr<ObjectBase::character> m_object;
	};
}

#endif
