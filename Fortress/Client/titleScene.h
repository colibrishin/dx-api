#pragma once
#ifndef TITLESCENE_H
#define TITLESCENE_H

#include "character.hpp"
#include "sceneManager.hpp"

namespace Fortress::Scene
{
	class TitleScene final : public SceneManager::_scene
	{
	public:
		TitleScene() : _scene(L"Title Scene")
		{
		}

		void initialize() override;
		void update() override;
		void render() override;
		void deactivate() override;
		void activate() override;

	private:
		std::shared_ptr<ObjectBase::character> m_objects[100];
	};
}

#endif
