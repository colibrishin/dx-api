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
	};
}
