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
	class GifWrapper;
	class ImageWrapper;
	class NextPlayerTimer;
	class Round;

	namespace Abstract
	{
		class rigidBody;
	}

	namespace ObjectBase
	{
		class character;
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
	using DirVector = Math::Vector2;
	using SizeVector = Math::Vector2;

	using SpeedVector = Math::Vector2;
	using AccelVector = Math::Vector2;

	using CharacterPointer = std::weak_ptr<ObjectBase::character>;
	using GroundPointer = std::weak_ptr<Object::Ground>;
	using ProjectilePointer = std::weak_ptr<ObjectBase::projectile>;
	using RigidBodyPointer = std::weak_ptr<Abstract::rigidBody>;
	using ImagePointer = std::weak_ptr<ImageWrapper>;

	using SpritePointer = std::weak_ptr<GifWrapper>;
	using AnimFlag = bool;
	using AnimElapsedFloat = float;

	using GifOnTimerFunction = std::function<void(GifWrapper*)>;
	using NextPlayerTimerFunction = std::function<void(Round*)>;
	using ProjectileInitFunction = std::function<ProjectilePointer(ObjectBase::character*, const UnitVector&, const float)>;

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
		IdleLow,
		Move,
		MoveLow,
		Firing,
		Fire,
		FireSub,
		Fired,
		PreItem,
		Item,
		TurnEnd,
		Dead,
		Death,
		Hit,
	};

	enum class eRoundState
	{
		Start = 0,
		InProgress,
		Waiting,
		NextTurn,
		End,
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

	enum class eDirVector
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
