#ifndef PROJECTILECONTROLLER_HPP
#define PROJECTILECONTROLLER_HPP
#include "stateController.hpp"

namespace Fortress
{
	namespace Object
	{
		class Ground;
	}
}

namespace Fortress
{
	namespace ObjectBase
	{
		class character;
	}
}

namespace Fortress::Controller
{
	class ProjectileController : public stateController<eProjectileState>
	{
	public:
		~ProjectileController() override = default;
		ProjectileController(const std::wstring& short_name, const Math::Vector2& velocity,
		                     const Math::Vector2& fired_position,
		                     const Math::Vector2& position, int max_hit_count, int max_fire_count);

		void initialize() override;
		void update() override;
		void prerender() override;

		bool is_cooldown() const;
		bool is_exploded() const;

		int get_hit_count() const;
		int get_fire_count() const;
		int get_max_hit_count() const;

	private:
		void set_current_sprite(const eProjectileState&) override;

		void fire_state();
		void flying_state();
		void character_hit_state();
		void ground_hit_state();
		void destroyed_state();

		void reset_cooldown();
		void increase_hit_count();

		const Math::Vector2& m_pc_velocity;

		const int m_max_hit_count;
		int m_curr_hit_count;
		float m_hit_cooldown;

		int m_fire_count;

		bool m_bExploded;

		float m_pitch;

		const Math::Vector2& m_fired_position;
		const Math::Vector2& m_pc_position;
		Math::Vector2 m_previous_position;

	protected:
		friend class Object::Ground;

		virtual void fire() = 0;
		virtual void flying() = 0;
		virtual void hit() = 0;
		virtual void destroyed() = 0;

		virtual void play_fire_sound() = 0;
		virtual void play_hit_sound() = 0;

		float get_pitch() const;
		void notify_character_hit();
		void notify_ground_hit();
	};
}
#endif // PROJECTILECONTROLLER_HPP
