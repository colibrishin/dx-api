#pragma once
#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "character.hpp"
#include "rigidBody.hpp"

namespace Fortress::ObjectBase
{
	constexpr float default_explosion_radius = 10.0f;

	class projectile : public Abstract::rigidBody
	{
	public:
		projectile() = delete;
		projectile& operator=(const projectile& other) = delete;
		projectile& operator=(projectile&& other) = delete;

		__forceinline void initialize() override;
		__forceinline static void update();

		float get_damage() const;

	protected:
		projectile(
			const std::wstring& name,
			const Math::Vector2& position,
			const Math::Vector2& WH,
			const Math::Vector2& velocity,
			const float speed,
			const float acceleration,
			const float damage) :
			rigidBody(name, position, WH, velocity, speed, acceleration),
			m_damage(damage)
		{
			projectile::initialize();
		}

		__forceinline ~projectile() override
		{
			if (!_known_projectiles.empty())
			{
				_known_projectiles.erase(
					std::remove_if(
						_known_projectiles.begin(),
						_known_projectiles.end(),
						[this](const projectile* r)
						{
							return r == this;
						}),
					_known_projectiles.end());
			}

			rigidBody::~rigidBody();
		}

	private:
		inline static std::vector<projectile*> _known_projectiles = {};
		float m_damage;
	};

	__forceinline void projectile::update()
	{
		for (const auto p : _known_projectiles)
		{
			if (!p->m_bActive)
			{
				continue;
			}

			Debug::Log(L"Projectile is flying");

			if (!p->collision_lists.empty())
			{
				bool hit = false;

				for (const auto collider : p->collision_lists)
				{
					auto gr = dynamic_cast<Object::Ground*>(collider);
					if (gr)
					{
						// @todo: dig a hole into the Ground.
						Debug::Log(L"Projectile hits the Ground");
						hit = true;
					}

					auto ch = dynamic_cast<character*>(collider);
					if (ch)
					{
						Debug::Log(L"Projectile hits the character");
						hit = true;
						ch->hit(p);
					}
				}

				if (hit)
				{
					p->m_bActive = false;
					// @todo: do a "thing" if a projectile is a explosive.
				}
			}
		}
	}
}
#endif // PROJECTILE_HPP
