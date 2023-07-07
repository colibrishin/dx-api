#pragma once
#ifndef TITLESCENE_H
#define TITLESCENE_H

#include "object.h"
#include "sceneManager.hpp"

namespace Scene 
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
		Object::Character m_objects[100];
	};
}

#endif