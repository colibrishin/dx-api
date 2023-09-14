#pragma once
#include <string>
#include "../Common/vector2.hpp"

namespace Fortress::Object::Property
{
	inline Math::Vector2 projectile_speed_getter(const std::wstring& short_name, const std::wstring& type)
	{
		const Math::Vector2 default_projectile_speed = Math::Vector2{1.0f, 2.0f};

		if(short_name == L"cannon" && type == L"main")
		{
			return default_projectile_speed;
		}
		if(short_name == L"cannon" && type == L"sub")
		{
			return default_projectile_speed * 2;
		}
		if(short_name == L"missile" && type == L"main")
		{
			return default_projectile_speed * 1.5;
		}
		if(short_name == L"missile" && type == L"sub")
		{
			return default_projectile_speed * 1.5;
		}
		if(short_name == L"secwind" && type == L"main")
		{
			return default_projectile_speed * 1.5;
		}
		if(short_name == L"secwind" && type == L"sub")
		{
			return default_projectile_speed * 2;
		}

		return {};
	}

	inline float projectile_radius_getter(const std::wstring& short_name, const std::wstring& type)
	{
		if(short_name == L"cannon" && type == L"main")
		{
			return 50.0f;
		}
		if(short_name == L"cannon" && type == L"sub")
		{
			return 15.0f;
		}
		if(short_name == L"missile" && type == L"main")
		{
			return 30.0f;
		}
		if(short_name == L"missile" && type == L"sub")
		{
			return 10.0f;
		}
		if(short_name == L"secwind" && type == L"main")
		{
			return 30.0f;
		}
		if(short_name == L"secwind" && type == L"sub")
		{
			return 10.0f;
		}

		return {};
	}

	inline float projectile_damage_getter(const std::wstring& short_name, const std::wstring& type)
	{
		if(short_name == L"cannon" && type == L"main")
		{
			return 10.0f;
		}
		if(short_name == L"cannon" && type == L"sub")
		{
			return 25.0f;
		}
		if(short_name == L"missile" && type == L"main")
		{
			return 10.0f;
		}
		if(short_name == L"missile" && type == L"sub")
		{
			return 10.0f;
		}
		if(short_name == L"secwind" && type == L"main")
		{
			return 30.0f;
		}
		if(short_name == L"secwind" && type == L"sub")
		{
			return 10.0f;
		}

		return {};
	}

	inline float projectile_pen_rate_getter(const std::wstring& short_name, const std::wstring& type)
	{
		if(short_name == L"cannon" && type == L"main")
		{
			return 0.9f;
		}
		if(short_name == L"cannon" && type == L"sub")
		{
			return 1.0f;
		}
		if(short_name == L"missile" && type == L"main")
		{
			return 0.9f;
		}
		if(short_name == L"missile" && type == L"sub")
		{
			return 0.7f;
		}
		if(short_name == L"secwind" && type == L"main")
		{
			return 0.7f;
		}
		if(short_name == L"secwind" && type == L"sub")
		{
			return 0.5f;
		}

		return {};
	}

	inline float character_armor_getter(const std::wstring& short_name)
	{
		if(short_name == L"cannon")
		{
			return 0.5f;
		}
		if(short_name == L"missile")
		{
			return 1.0f;
		}
		if(short_name == L"secwind")
		{
			return 0.7f;
		}

		return {};
	}

	inline SizeVector character_hitbox_getter()
	{
		return {50.0f, 50.0f};
	}

	inline SizeVector projectile_hitbox_getter()
	{
		return {30.0f, 30.0f};
	}
}
