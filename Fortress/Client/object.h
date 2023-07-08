#ifndef OBJECT_H
#define OBJECT_H
#pragma once

#include "character.hpp"
#include "ground.hpp"
#include "object.hpp"
#include "rigidbody.hpp"

namespace ObjectInternal
{
	typedef _baseObject BaseObject;
	typedef _rigidBody RigidBody;
}

namespace Object
{
	using Character = character;
	using Ground = ground;
	using Projectile = projectile;
}

#endif