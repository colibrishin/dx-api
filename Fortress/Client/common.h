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
		None,
		Identical,
		Boundary,
		Inside,
	};

	enum class eCharacterState
	{
		Idle = 0,
		Move,
		Firing,
		Fire,
		Item,
		Dead,
		Hit,
	};

	enum class eHitVector
	{
		Unknown = -1,
		Identical = 0,
		Top = 1,
		Bottom = 2,
		Left = 4,
		Right = 8,
		TopLeft = 5,
		TopRight = 9,
		BottomLeft = 6,
		BottomRight = 10
	};
}
