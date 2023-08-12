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

#include <memory>

#include "framework.h"

namespace Fortress
{
	class ImageWrapper;

	namespace Abstract
	{
		class rigidBody;
	}

	namespace ObjectBase
	{
		class projectile;
	}

	namespace Object
	{
		class Ground;
	}

	namespace Math
	{
		struct Vector2;
	}

	using GlobalPosition = Math::Vector2;
	using LocalPosition = Math::Vector2;
	using UnitVector = Math::Vector2;

	using SpeedVector = Math::Vector2;
	using AccelVector = Math::Vector2;

	using GroundPointer = std::weak_ptr<Object::Ground>;
	using ProjectilePointer = std::weak_ptr<ObjectBase::projectile>;
	using RigidBodyPointer = std::weak_ptr<Abstract::rigidBody>;
	using ImagePointer = std::weak_ptr<ImageWrapper>;

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
		Fired,
		PreItem,
		Item,
		TurnEnd,
		Dead,
		Death,
		Hit,
	};

	enum class eProjectileState
	{
		Fire = 0,
		Flying,
		CharacterHit,
		GroundHit,
		Destroyed
	};

	enum class eProjectileType
	{
		Main = 0,
		Sub,
		Nutshell
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

	namespace Dev
	{
		template< typename ContainerT, typename PredicateT >
		void erase_if( ContainerT& items, const PredicateT& predicate ) {
	    for( auto it = items.begin(); it != items.end(); ) {
			if( predicate(*it) ) it = items.erase(it);
			else ++it;
			}
		}
	}
}
