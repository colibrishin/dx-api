#pragma once

#ifndef MATH_H
#define MATH_H

#include "vector2.hpp"

namespace MathInternal
{
	static constexpr float epsilon = 0.0001f;

	// Low-level data struct for vector2
	struct _vector2;
	// High-level data struct for vector2
	struct vector2;
}

// redirection for actual vector2


namespace Math 
{
	using Vector2 = MathInternal::vector2;
}

#endif