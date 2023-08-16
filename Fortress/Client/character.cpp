#include "character.hpp"

#include "ground.hpp"
#include "input.hpp"
#include "projectile.hpp"
#include "ProjectileController.hpp"
#include "NutshellProjectile.hpp"
#include "CharacterController.hpp"

#undef max
#undef min

namespace Fortress::ObjectBase
{
	character::~character()
	{
		TimerManager::remove(m_multi_projectile_timer);
	}

	void character::initialize()
	{
		rigidBody::initialize();
		CharacterController::initialize();
		m_multi_projectile_timer = TimerManager::create<ProjectileTimer>(
			&character::initialize_projectile, this);
	}

	void character::update()
	{
		rigidBody::update();
		CharacterController::update();
	}

	void character::prerender()
	{
		rigidBody::prerender();
		CharacterController::prerender();
	}

	float character::get_damage_pen_dist(const std::weak_ptr<projectile>& p, const Math::Vector2& hit_point) const
	{
		// most hit takes from boundary, this value is for compensation the error by hits from boundary.
		const float proximate_close_hit_epsilon = p.lock()->m_hitbox.magnitude() / 3;

		// base damage
		const float damage = p.lock()->get_damage();
		// armor penetration rate
		const float penetration_rate = p.lock()->get_penetration_rate();
		// armor
		const float armor = get_armor();

		// gets the nearest point from projectile and distance.
		const Math::Vector2 near_point = get_nearest_point(hit_point);
		const float distance = (hit_point - near_point).magnitude();
		// explosion radius
		const float radius = p.lock()->get_radius();

		// pen vs armor. armor gets bigger, value gets smaller.
		const float pen_armor_rate = penetration_rate / armor;

		if(pen_armor_rate < 0.0f)
		{
			Debug::Log(L"Warning: pen_armor_rate is negative value.");
		}

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

		const int previous_hit_count = get_previous_hit_count();
		const float consecutive_hit_bonus = dist_pen_damage * (static_cast<float>(previous_hit_count) * 0.25f);
		const float total_damage = dist_pen_damage + consecutive_hit_bonus;

		return total_damage;
	}

	void character::hit(const std::weak_ptr<projectile>& p, const Math::Vector2& hit_point)
	{
		apply_damage(get_damage_pen_dist(p, hit_point));
	}

	void character::fire()
	{
		m_multi_projectile_timer.lock()->stop();

		float charged = get_charged_power();

		if(charged <= 0.0f)
		{
			charged = 10.0f;
		}

		Math::Vector2 angle{};

		if(get_offset() == Math::left)
		{
			angle = {-cosf(get_movement_pitch_radian()), -sinf(get_movement_pitch_radian())};
		}
		else
		{
			angle = {cosf(get_movement_pitch_radian()), sinf(get_movement_pitch_radian())};
		}

		const std::weak_ptr<projectile> instantiated = initialize_projectile(angle, charged);

		const int remaining = instantiated.lock()->get_fire_count() - 1;

		if(remaining > 0)
		{
			m_multi_projectile_timer.lock()->set_count(remaining);
			m_multi_projectile_timer.lock()->set_angle(angle);
			m_multi_projectile_timer.lock()->set_charged(charged);
			m_multi_projectile_timer.lock()->toggle();
		}

		CharacterController::fire();
	}

	float character::get_armor() const
	{
		return m_armor;
	}

	void character::render_hp_bar(const Math::Vector2& position)
	{
		// white box
		Rectangle(
			WinAPIHandles::get_buffer_dc(),
			position.get_x(),
			position.get_y() - 20,
			position.get_x() + 52,
			position.get_y() - 10);

		// inside hp bar
		const float hp_percentage = get_hp_percentage();
		HBRUSH brush;

		if (hp_percentage > 0.5f) 
		{
			brush = CreateSolidBrush(RGB(0, 255, 0));
		}
		else if (hp_percentage >= 0.3f)
		{
			brush = CreateSolidBrush(RGB(255, 255, 0));
		}
		else
		{
			brush = CreateSolidBrush(RGB(255, 0, 0));
		}

		const RECT rect = {
			static_cast<int>(position.get_x()),
			static_cast<int>(position.get_y() - 19),
			static_cast<int>(position.get_x() + (51 * get_hp_percentage())),
			static_cast<int>(position.get_y() - 12) };

		FillRect(WinAPIHandles::get_buffer_dc(), &rect, brush);
		DeleteObject(brush);
	}

	void character::render()
	{
		Math::Vector2 pos{};

		if (is_active())
		{
			const auto camera_ptr = Scene::SceneManager::get_active_scene().lock()->get_camera().lock();

			if(camera_ptr->get_locked_object().lock() == 
				std::dynamic_pointer_cast<object>(rigidBody::shared_from_this()))
			{
				pos = camera_ptr->get_offset(m_hitbox);
			}
			else
			{
				pos = camera_ptr->get_relative_position(
					std::dynamic_pointer_cast<object>(rigidBody::shared_from_this()));
			}

			prerender();
			render_hp_bar(pos);
			
			m_current_sprite.lock()->render(pos, m_hitbox, {1, 1}, Math::to_degree(get_movement_pitch_radian()));

			// c
			Debug::draw_line(pos, camera_ptr->get_offset());

			// t
			Debug::draw_line(
				camera_ptr->get_offset(), 
				{camera_ptr->get_offset().get_x(), pos.get_y()});

			// s
			Debug::draw_line(
				{camera_ptr->get_offset().get_x(), pos.get_y()}, pos);
		}

		rigidBody::render();
	}

	const std::wstring& character::get_short_name() const
	{
		return m_short_name;
	}

	ProjectilePointer character::initialize_projectile(const Math::Vector2& angle, const float charged)
	{
		std::weak_ptr<projectile> instantiated;

		if(get_projectile_type() == eProjectileType::Main)
		{
			instantiated = get_main_projectile();
		}
		else if(get_projectile_type() == eProjectileType::Sub)
		{
			instantiated = get_sub_projectile();
		}
		else
		{
			instantiated = ObjectManager::create_object<Object::NutShellProjectile>(this);
		}

		const auto projectile = instantiated.lock();

		// thinking as virtual circle which has an radius of projectile size + half of character hitbox,
		// with an top left/right toward to.
		const auto forward = Math::Vector2{get_offset().get_x(), -1} * (projectile->m_hitbox + (m_hitbox / 2));
		const auto forward_rotation = forward.rotate(get_movement_pitch_radian());

		projectile->fire(
			get_center() + forward_rotation, 
			angle, 
			charged);

		if(const auto scene = Scene::SceneManager::get_active_scene().lock())
		{
			scene->add_game_object(Abstract::LayerType::Projectile, projectile);
		}

		add_active_projectile(projectile);

		return instantiated;
	}

	character::character(
			const std::wstring& name,
			const std::wstring& short_name,
			const Math::Vector2& offset,
			const Math::Vector2& position,
			const Math::Vector2& velocity,
			const float mass,
			const Math::Vector2& speed,
			const Math::Vector2& acceleration,
			const int hp,
			const int mp,
			const float armor):
			rigidBody(
				name, 
				position, 
				{50.0f, 50.0f}, 
				velocity,
				mass, 
				speed, 
				acceleration, 
				true),
			CharacterController(
				short_name, 
				hp, 
				mp,
				this),
			m_bGrounded(false),
			m_short_name(short_name),
			m_armor(armor)
		{
		}


	void character::move()
	{
		rigidBody::move();
		CharacterController::move();
	}

	void character::move_left()
	{
		rigidBody::move_left();
		CharacterController::move_left();
	}

	void character::move_right()
	{
		rigidBody::move_right();
		CharacterController::move_right();
	}

	void character::stop()
	{
		rigidBody::stop();
		CharacterController::stop();
	}
}
