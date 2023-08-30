#pragma once
#include <algorithm>
#include "../Common/vector2.hpp"
#include "../Common/message.hpp"
#include "../Client/CharacterProperties.hpp"

#undef max
#undef min

namespace Fortress::Network::Server
{
	inline float calculate_damage(
		const eCharacterType shooter_type,
		const eCharacterType victim_type,
		const eProjectileType prj_type,
		const unsigned int previous_hit_count,
		const bool double_damage,
		const Math::Vector2& hitbox,
		const Math::Vector2& hit_point,
		const Math::Vector2& near_point)
	{
		std::wstring shooter_ch_name;
		std::wstring shooter_prj_name;

		std::wstring victim_ch_name;

		switch(shooter_type)
		{
		case eCharacterType::None: break;
		case eCharacterType::CannonCharacter: 
			shooter_ch_name = L"cannon";
			break;
		case eCharacterType::MissileCharacter: 
			shooter_ch_name = L"missile";
			break;
		case eCharacterType::SecwindCharacter: 
			shooter_ch_name = L"secwind";
			break;
		default: ;
		}

		switch(victim_type)
		{
		case eCharacterType::None: break;
		case eCharacterType::CannonCharacter: 
			victim_ch_name = L"cannon";
			break;
		case eCharacterType::MissileCharacter: 
			victim_ch_name = L"missile";
			break;
		case eCharacterType::SecwindCharacter: 
			victim_ch_name = L"secwind";
			break;
		default: ;
		}

		switch(prj_type)
		{
		case eProjectileType::Main: 
			shooter_prj_name = L"main";
			break;
		case eProjectileType::Sub: 
			shooter_prj_name = L"sub";
			break;
		case eProjectileType::Nutshell: break;
		default: ;
		}

		const float damage = Object::Property::projectile_damage_getter(shooter_ch_name, shooter_prj_name);
		const float armor = Object::Property::character_armor_getter(victim_ch_name);
		const float penetration_rate = Object::Property::projectile_pen_rate_getter(shooter_ch_name, shooter_prj_name);
		const float radius = Object::Property::projectile_radius_getter(shooter_ch_name, shooter_prj_name);

		// most hit takes from boundary, this value is for compensation the error by hits from boundary.
		// note that this hitbox should be from projectile. (near_point = character's nearest
		// point, which means m_position - (m_hitbox / 2) (with hitbox).
		// but projectile position comes from get_center(), which means m_position (without hitbox).
		const float proximate_close_hit_epsilon = hitbox.magnitude() / 2;
		const float distance = (hit_point - near_point).magnitude();

		// pen vs armor. armor gets bigger, value gets smaller.
		const float pen_armor_rate = penetration_rate / armor;

		// normalize the value to between from 0 to 1.
		const float normalized_rate = std::min(1.0f, std::fabs(pen_armor_rate));
		const float pen_damage = damage * normalized_rate;

		// see proximate_close_hit_epsilon.
		const float close_hit_compensation = std::fabs(distance - proximate_close_hit_epsilon);
		// if hit point is further than radius, damage gets smaller.
		const float dist_ratio = logf(close_hit_compensation) / logf(radius);
		// dist_ratio can be negative value, if it is too close (log scale).
		const float dist_ratio_flooring = std::max(dist_ratio, 0.0f);
		const float far_dist_ratio = std::min(1.0f, dist_ratio_flooring);

		const float dist_pen_damage = pen_damage * (1.0f - far_dist_ratio);
		
		const float consecutive_hit_bonus = dist_pen_damage * (static_cast<float>(previous_hit_count) * 0.25f);
		float total_damage = dist_pen_damage + consecutive_hit_bonus;

		if(double_damage)
		{
			total_damage += total_damage;
		}

		return total_damage;
	}
}
