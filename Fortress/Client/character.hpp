#pragma once
#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "object.h"
#include "rigidbody.hpp"
#include "sceneManager.hpp"

namespace Object
{
	enum class CharacterType
	{
		CANNON = 0,
	};

	constexpr float character_full_hp = 100.0f;
	constexpr float character_full_mp = 100.0f;

	class projectile final : public ObjectInternal::_rigidBody
	{
		enum class ProjectileType
		{
			Precision = 0,
			Explosion,
		};

	public:
		projectile(
			const std::wstring& name, 
			const Math::Vector2& position, 
			const Math::Vector2& WH, 
			const Math::Vector2& velocity,
			const float speed,
			const float acceleration,
			const float damage) :
			_rigidBody(name, position, WH, velocity, speed, acceleration),
			m_projectile_type(ProjectileType::Precision),
			m_damage(damage)
		{
			initialize();
		}

		float get_damage() const
		{
			return m_damage;
		}

		~projectile() override;
		void initialize() override; 
		static void update();
		void render() override;
	private:
		ProjectileType m_projectile_type;
		float m_damage;
		inline static std::vector<projectile*> _known_projectiles = {};
	};

	// @todo: render can be moved into baseObject, virtual and override.
	inline void projectile::render()
	{
		if(m_bActive)
		{
			Ellipse(
				WinAPIHandles::get_buffer_dc(),
				m_position.get_x(), 
				m_position.get_y(), 
				m_position.get_x() + m_hitbox.get_x(),
				m_position.get_y() + m_hitbox.get_y());
		}
	}

	class character final : public ObjectInternal::_rigidBody
	{
	public:
		character() :
		_rigidBody(L"", {0, 0}, {0, 0}, {0, 0}, 0, 0),
		m_hp(0),
		m_mp(0),
		m_type(CharacterType::CANNON),
		m_base_projectile(
			L"Cannon Ball", 
			{m_position.get_x() + m_hitbox.get_x(), m_position.get_y() - 1.0f},
			{2.0f, 2.0f},
			{1.0f, 1.0f},
			50.0f,
			0.0f,
			10)
		{
			m_base_projectile.m_bActive = false;
		}

		// copying is intended for preventing nullptr (use-after-free).
		character(
			const std::wstring& name,
			const Math::Vector2 position, 
			const Math::Vector2 velocity,
			const Math::Vector2 WH,
			const float speed,
			const float acceleration,
			const int hp,
			const int mp,
			const CharacterType type)
		: _rigidBody(name, position, WH, velocity, speed, acceleration), m_hp(hp), m_mp(mp), m_type(type),
		m_base_projectile(
			L"Cannon Ball", 
			{m_position.get_x() + m_hitbox.get_x(), m_position.get_y() - 1.0f},
			{2.0f, 2.0f},
			{1.0f, -1.0f},
			50.0f,
			0.0f,
			10)
		{
			m_base_projectile.m_bActive = false;
		}

		void hit(const projectile* p);
		static void update();
		__forceinline float get_hp_percentage() const
		{
			return m_hp / static_cast<float>(character_full_hp);
		}
		__forceinline float get_mp_percentage() const
		{
			return m_mp / static_cast<float>(character_full_mp);
		}
		void shoot();
		bool is_movable() override;
		void render() override;

	private:
		float m_hp;
		float m_mp;
		CharacterType m_type;
		projectile m_base_projectile;
	};

	inline projectile::~projectile()
	{
		if(!_known_projectiles.empty())
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
	}

	inline void projectile::initialize()
	{
		_rigidBody::initialize();
		_known_projectiles.push_back(this);
	}

	inline void projectile::update()
	{
		for(const auto p : _known_projectiles)
		{
			if(!p->m_bActive)
			{
				continue;
			}

			Fortress::Debug::Log(L"Projectile is flying");

			if(!p->collision_lists.empty())
			{
				bool hit = false;

				for(const auto collider : p->collision_lists)
				{
					ground* gr = dynamic_cast<ground*>(collider);
					if(gr)
					{
						// @todo: dig a hole into the ground.
						Fortress::Debug::Log(L"Projectile hits the ground");
						hit = true;
					}

					character* ch = dynamic_cast<character*>(collider);
					if(ch)
					{
						Fortress::Debug::Log(L"Projectile hits the character");
						hit = true;
						ch->hit(p);
					}
				}

				if(hit)
				{
					p->m_bActive = false;
				}
			}
		}
	}

	inline void character::hit(const projectile* p)
	{
		m_hp -= p->get_damage();
	}

	inline void character::update()
	{
		_rigidBody::update();
		// @note: we will just update projectile with character because simultaneous shooting is not happening.
		projectile::update();
	}

	inline void character::shoot()
 	{
		// refreshing the projectile position
		m_base_projectile.m_position = {m_position.get_x() + m_hitbox.get_x() + 10.0f, m_position.get_y() - 10.0f};
		// set active for being calculated by rigidbody.
		m_base_projectile.m_bActive = true;
	}

	inline bool character::is_movable()
	{
		if(m_mp < Math::epsilon)
		{
			return false;
		}

		if(std::fabs(m_velocity.get_x()) > Math::epsilon || 
			std::fabs(m_velocity.get_y()) > Math::epsilon)
		{
			m_mp -= 20.0f * Fortress::DeltaTime::get_deltaTime();
		}

		return true;
	}

	inline void character::render()
	{
		if(m_bActive)
		{
			Ellipse(
				WinAPIHandles::get_buffer_dc(),
				get_x(), 
				get_y(), 
				get_x() + m_hitbox.get_x(),
				get_y() + m_hitbox.get_y());
		}

		m_base_projectile.render();		
	}
}

#endif
