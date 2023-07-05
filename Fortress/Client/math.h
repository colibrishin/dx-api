#pragma once

#ifndef MATH_H
#define MATH_H

#define _USE_MATH_DEFINES

#include <math.h>
#include "vector2.hpp"

namespace Math 
{
	using Vector2 = struct vector2;
	constexpr float PI = M_PI;
	constexpr float G = 6.6742E-11;

}

#endif