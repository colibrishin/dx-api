// Common.cpp : 정적 라이브러리를 위한 함수를 정의합니다.
//

#include "pch.h"

#include <chrono>

#include "framework.h"

namespace Fortress::Network
{
	[[nodiscard]] std::time_t get_time()
	{
		const std::time_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(
	            std::chrono::high_resolution_clock::now().time_since_epoch())
	            .count();
	    return ms;
	}	
}
