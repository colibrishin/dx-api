#include "character.hpp"
#include "ground.hpp"

namespace Fortress::ObjectBase
{
	void character::ground_walk(const CollisionCode& collision, const std::weak_ptr<Object::Ground>& ptr_ground)
	{
		if(const auto ground = ptr_ground.lock())
		{
			const Math::Vector2 bottom_local_position = ground->to_top_left_local_position(get_bottom());

			const Object::GroundState bottom_check = ground->safe_is_destroyed(bottom_local_position);

			if(collision == CollisionCode::Inside)
			{
				if (get_state() == eCharacterState::Move) 
				{
					if (bottom_check == Object::GroundState::NotDestroyed)
					{
						if (ground->safe_is_object_stuck_global(get_bottom()))
						{
							const auto delta = ground->safe_nearest_surface(get_bottom());
							m_position -= delta;
						}
					}

					const auto candidate = get_next_velocity(bottom_local_position, ground);

					if (candidate == Math::Vector2{})
					{
						return;
					}

					if (m_bGrounded && candidate != Math::vector_inf)
					{
						m_velocity = candidate;
					}
				}
			}
		}
	}

	void character::ground_gravity(const std::weak_ptr<Object::Ground>& ptr_ground)
	{
		if(const auto ground = ptr_ground.lock())
		{
			const auto bottom_check = ground->safe_is_destroyed(ground->to_top_left_local_position(get_bottom()));

			if(bottom_check == Object::GroundState::NotDestroyed)
			{
				reset_current_gravity_speed();
				disable_gravity();
				m_bGrounded = true;
			}
			else if (bottom_check == Object::GroundState::Destroyed)
			{
				enable_gravity();
				m_bGrounded = false;
				set_movement_pitch_radian(0.0f);
				Debug::Log(L"Character hits the destroyed ground");
			}
			else if (bottom_check == Object::GroundState::OutOfBound)
			{
				enable_gravity();
				m_bGrounded = false;
				set_movement_pitch_radian(0.0f);
				Debug::Log(L"Character is outside of the ground");
			}
		}
	}

	void character::ground_pitching(const std::weak_ptr<Object::Ground>& ptr_ground)
	{
		if(const auto ground = ptr_ground.lock())
		{
			const bool uphilling = ground->safe_is_object_stuck_global(get_offset_bottom_forward_position());
			Math::Vector2 delta{};

			// calculate only downhilling from current position. if character is uphilling then calculate downhilling angle and reverse it.
			auto next_surface = get_offset_bottom_forward_position();
			Math::Vector2 search_vector = get_offset() == Math::left ? Math::right : Math::left;

			if(uphilling)
			{
				next_surface = get_offset() == Math::left ? get_bottom_right() : get_bottom_left();
				search_vector = get_offset();
			}

			for(int x = 0; x < m_hitbox.get_x(); ++x)
			{
				delta = ground->safe_orthogonal_surface_global(
					next_surface + 
					search_vector * x,
					m_hitbox.get_y() / 2);

				if(delta != Math::vector_inf)
				{
					delta += (get_offset() == Math::left ? Math::right : Math::left) * x;
					break;
				}
			}

			if(delta == Math::vector_inf)
			{
				set_movement_pitch_radian(0);
				return;
			}
			next_surface += delta;

			Debug::draw_dot(next_surface);
			auto rotate_radian = next_surface.local_inner_angle(get_bottom());

			set_movement_pitch_radian(rotate_radian);
		}
	}

	Math::Vector2 character::get_next_velocity(
		const Math::Vector2& local_position_bottom, const std::weak_ptr<Object::Ground>& ground_ptr) const
	{
		bool angle_check = false;
		bool climable = false;
		Math::Vector2 candidate{};

		if (const auto ground = ground_ptr.lock())
		{
			// check up-hilling condition
			for(int x = 0; x < 10; x++)
			{
				if (angle_check && climable) 
				{
					break;
				}

				angle_check = false;

				// searching in reverse. we need to check whether how stiff the curve is.
				for(int y = 99; y >= 0; --y)
				{
					Math::Vector2 local_new_pos = {
						local_position_bottom.get_x() + 
							(get_offset() == Math::left ?  -x : x),
						local_position_bottom.get_y() - y
					};

					const auto diff = local_new_pos - local_position_bottom;
					const auto unit = diff.normalized();
					const auto ground_check = ground->safe_is_destroyed(local_new_pos);

					if (isnan(unit.get_x()) || isnan(unit.get_y()) || 
						ground_check == Object::GroundState::OutOfBound)
					{
						continue;
					}

					if (ground_check == Object::GroundState::NotDestroyed)
					{
						if (!angle_check)
						{
							if (std::fabs(Math::to_degree(local_position_bottom.local_inner_angle(local_new_pos)))
								>= 60.0f || ground->safe_is_object_stuck_local(local_new_pos)) 
							{
								// @todo: fixed animation
								angle_check = false;
								climable = false;
								candidate = Math::vector_inf;
							}
							
							climable = true;
							angle_check = true;
						}

						candidate = unit;
					}
				}
			}

			if(climable)
			{
				return candidate;
			}

			candidate = {};

			for(int i = 0; i < 10; i++)
			{
				for(int j = 0; j < 100; ++j)
				{
					Math::Vector2 local_new_pos = {
						local_position_bottom.get_x() + 
							(get_offset() == Math::left ?  -i : i),
						local_position_bottom.get_y() + j
					};

					const auto diff = local_new_pos - local_position_bottom;
					const auto unit = diff.normalized();
					const auto ground_check = ground->safe_is_destroyed(local_new_pos);

					if (isnan(unit.get_x()) || isnan(unit.get_y()))
					{
						continue;
					}

					if (ground_check == Object::GroundState::NotDestroyed)
					{
						candidate = unit;
						return candidate;
					}
				}
			}
		}

		if (!angle_check || !climable) 
		{
			candidate = Math::vector_inf;
		}

		return candidate;
	}

	void character::on_collision(const CollisionCode& collision, const Math::Vector2& hit_vector, const std::weak_ptr<Abstract::rigidBody>& other)
	{
		if(const auto ground = std::dynamic_pointer_cast<Object::Ground>(other.lock()))
		{
			const auto left_local_position = ground->to_top_left_local_position(get_left());
			const auto right_local_position = ground->to_top_left_local_position(get_right());

			// @todo: delta checks valid but this collision starts from right side it has the negative values, and considered as oob.
			// check should be done in reverse. use the positive side only is probably the best way for avoiding any problem.
			const Object::GroundState left_check = ground->safe_is_destroyed(left_local_position + ground->m_hitbox);
			const Object::GroundState right_check = ground->safe_is_destroyed(right_local_position);
			const auto orthogonal_surface = ground->safe_orthogonal_surface_global(get_bottom(), -1, -1);

			// This ground is orthogonal surface. it will be treated as "main" ground.
			if(orthogonal_surface != Math::vector_inf)
			{
				ground_walk(collision, ground);
				ground_gravity(ground);
				ground_pitching(ground);
			}
			else
			{
				// If this ground is not orthogonal surface, probably this will exists in left or right, top side.
				// not-main ground is low enough to pass, then move it to the not-main ground.
				// remaining gravity or pitching control will be handled when the not-main becomes main.
				const Math::Vector2 bottom_local_position = ground->to_top_left_local_position(get_bottom());
				const auto possible_velocity = get_next_velocity(bottom_local_position, ground);

				if(possible_velocity != Math::vector_inf)
				{
					m_velocity = possible_velocity;
				}
				else if((left_check == Object::GroundState::NotDestroyed && get_velocity_offset() == Math::left) || 
					(right_check == Object::GroundState::NotDestroyed && get_velocity_offset() == Math::right))
				{
					m_velocity = {};
				}
			}
		}

		rigidBody::on_collision(collision, hit_vector, other);
	}

	void character::on_nocollison()
	{
		enable_gravity();
		m_bGrounded = false;
	}
}
