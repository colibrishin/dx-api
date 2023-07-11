#include "character.hpp"

#include "deltatime.hpp"
#include "ground.hpp"
#include "projectile.hpp"

namespace Fortress::ObjectBase
{
	void character::hit(const projectile* p)
	{
		m_hp -= p->get_damage();
	}

	void character::shoot()
	{
		m_power = 0.0f;
	}

	float character::get_charged_power() const
	{
		return m_power;
	}

	float character::get_hp_percentage() const
	{
		return m_hp / static_cast<float>(character_full_hp);
	}

	float character::get_mp_percentage() const
	{
		return m_mp / static_cast<float>(character_full_mp);
	}

	character::~character()
	{
		rigidBody::~rigidBody();
	}

	void character::update()
	{
		rigidBody::update();
	}

	void character::firing()
	{
		m_power += 20.0f * DeltaTime::get_deltaTime();
	}

	void character::move()
	{
		if (m_mp < Math::epsilon)
		{
			return;
		}

		// @todo: going down also reduces the movement point.
		if (std::fabs(m_velocity.get_x()) > Math::epsilon ||
			std::fabs(m_velocity.get_y()) > Math::epsilon)
		{
			m_mp -= 20.0f * DeltaTime::get_deltaTime();
		}

		rigidBody::move();
	}

	void character::on_collision(rigidBody* other)
	{
		if(const auto* ground = dynamic_cast<Object::Ground*>(other))
		{
			if(ground)
			{
				Debug::Log(L"Character hits the ground");
				m_gravity_speed = 0.0f;
				m_bGrounded = true;
				return;
			}
		}

		rigidBody::on_collision(other);
	}

	character::character(const character& other) :
		rigidBody(other), m_hp(other.m_hp), m_mp(other.m_mp)
	{
		character::initialize();
	}
}
