#pragma once
#ifndef TITLESCENE_HPP
#define TITLESCENE_HPP

#include "scene.hpp"

namespace Fortress 
{
	class TitleScene final : public _scene 
	{
	public:
		void initialize() override;
		void update() override;
		void render() override;
	};
}

#endif