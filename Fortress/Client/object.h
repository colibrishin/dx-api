#ifndef OBJECT_H
#define OBJECT_H
#pragma once

#include "character.h"
#include "object.hpp"
#include "rigidbody.hpp"

namespace ObjectInternal
{
	enum class CollisionCode;
	class _baseObject;
	class _rigidBody;
}

namespace Object
{
	using Character = character;
}

#endif