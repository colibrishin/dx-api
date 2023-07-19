#pragma once

// ## STL

#include <vector>
#include <list>
#include <string>
#include <map>
#include <set>
#include <functional>
#include <queue>

// ## WINAPI

#include "framework.h"

namespace Fortress
{
	enum class CollisionCode
	{
		None = 0,
		Identical = 1,
		Top = 2,
		Bottom = 4,
		Left = 8,
		Right = 16,
		TopLeft = 10,
		TopRight = 18,
		BottomLeft = 12,
		BottomRight = 20,
	};

	enum class eCharacterState
	{
		Idle = 0,
		Move,
		Firing,
		Fire,
		Item
	};
}
