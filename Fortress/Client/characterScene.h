#pragma once
#ifndef CHARACTERSCENE_H
#define CHARACTERSCENE_H

#include "object.h"
#include "sceneManager.hpp"

namespace Scene 
{
	class CharacterScene final : public SceneManager::_scene 
	{
	public:
		CharacterScene() : _scene(L"Character Scene")
		{
		}
		void initialize() override;
		void update() override;
		void render() override;

	private:
		Object::Character m_object;
	};
}

#endif