#include "character.hpp"
#include "ground.hpp"

namespace Fortress::ObjectBase
{
	bool character::is_moving_toward(const GroundPointer& ground_ptr) const
	{
		const auto position = 
			search_ground(ground_ptr, get_offset_bottom_backward_position(), get_offset(), true);
		const auto unit = (position - get_bottom()).normalized();

		const auto velocity_offset = get_velocity_offset();
		const auto positional_offset = unit.get_x() < 0 ? Math::left : Math::right;

		return velocity_offset == positional_offset;
	}

	bool character::check_angle(const GlobalPosition& position, const GroundPointer& ground_ptr) const
	{
		if(const auto ground = ground_ptr.lock())
		{
			const auto offset = get_velocity_offset();

			const auto unit = (position - get_bottom()).normalized();
			const auto radian = unit.unit_angle();
			// if target position is behind of character, add +180.0d
			const auto positional_radian = unit.get_x() < 0 ? Math::flip_radian_polarity(radian) : radian;
			const auto reset_polarity = std::fabs(positional_radian);
			const auto velocity_radian = offset == Math::left ? -reset_polarity : reset_polarity;
			const auto degree = Math::to_degree(velocity_radian);

			return degree <= 80.0f;
		}

		return false;
	}

	void character::ground_walk(const CollisionCode& collision, const GroundPointer& ptr_ground)
	{
		if(const auto ground = ptr_ground.lock())
		{
			const Math::Vector2 bottom_local_position = ground->to_top_left_local_position(get_bottom());

			const Object::GroundState bottom_check = ground->safe_is_destroyed(bottom_local_position);

			if(collision == CollisionCode::Inside)
			{
				if (get_state() == eCharacterState::Move) 
				{
					const auto candidate = get_next_velocity(bottom_local_position, ground);

					if (candidate == Math::Vector2{})
					{
						return;
					}

					if (candidate != Math::vector_inf)
					{
						m_velocity = candidate;
					}

					if (candidate != Math::vector_inf && bottom_check == Object::GroundState::NotDestroyed)
					{
						if (ground->safe_is_object_stuck_global(get_bottom()))
						{
							const auto delta = ground->safe_nearest_surface(get_bottom());
							m_position -= delta;
						}
					}

					// check ground stiffness;
					const auto mid_ground = get_forward_ground(ground, false);

					if (is_moving_toward(ground) && !check_angle(mid_ground, ground))
					{
						m_velocity = {};
					}
				}
			}
		}
	}

	void character::ground_gravity(const GroundPointer& ptr_ground)
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

	void character::ground_pitching(const GroundPointer& ptr_ground)
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
					next_surface + search_vector * x,
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

	/**
	 * \brief gets nearest ground in parallel, in depth of the half of hitbox of y-axis.
	 * \param ground_ptr ground pointer
	 * \param reverse if true, search starts from bottom
	 * \return nearest ground vector, inf if it has found nothing.
	 */
	Math::Vector2 character::get_forward_ground(
		const GroundPointer& ground_ptr,
		const bool& reverse = false) const
	{
		if (const auto ground = ground_ptr.lock())
		{
			const bool uphilling = is_moving_toward(ground_ptr);

			// flipping is needed because hitbox is inside of the ground if character is moving forward to ground.
			const auto ray_start_pos = uphilling ? get_offset_backward_position() : get_offset_forward_position();
			// uphilling = shoots ray toward to moving position
			// downhilling = shoots ray toward to backward position (wall is at the behind of character)
			const auto offset = uphilling ? get_offset() : get_backward_offset();

			return search_ground(ground_ptr, ray_start_pos, offset, reverse);
		}

		return Math::vector_inf;
	}

	GlobalPosition character::search_ground(
		const GroundPointer& ground,
		const GlobalPosition& start_position, 
		const UnitVector& offset,
		bool reverse = false) const
	{
		int start_y = reverse ? m_hitbox.get_y() / 2 : 0;
		int end_y = reverse ? 0 : m_hitbox.get_y();

		for(int y = start_y;
			reverse ? (y >= end_y) : (y < end_y); 
			reverse ? y-- : y++)
		{
			const auto current_position = start_position + Math::Vector2{0.0f, y};
			const auto next_position = ground.lock()->safe_parallel_surface_global(
				current_position, 
				offset) + current_position;

			const auto unit = (next_position - get_bottom()).normalized();
			const auto radian = unit.unit_angle();

			if(unit == Math::zero || std::isnan(radian))
			{
				continue;
			}

			return next_position;
		}

		return Math::vector_inf;
	}

	Math::Vector2 character::get_next_velocity(
		const Math::Vector2& local_position_bottom, const GroundPointer& ground_ptr) const
	{
		Math::Vector2 candidate = Math::vector_inf;

		if (const auto ground = ground_ptr.lock())
		{
			// check up-hilling condition
			for(int x = 1; x < 100; x++)
			{
				// searching in reverse. we need to check whether how stiff the curve is.
				for(int y = 99; y >= 1; --y)
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

					if(unit == Math::Vector2{})
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

			if(candidate != Math::vector_inf)
			{
				return candidate;
			}

			candidate = Math::vector_inf;

			for(int i = 1; i < 10; i++)
			{
				for(int j = 1; j < 100; ++j)
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

					if(unit == Math::Vector2{} || 
						ground_check == Object::GroundState::OutOfBound)
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

				if(get_state() == eCharacterState::Move &&
					((left_check == Object::GroundState::NotDestroyed && get_velocity_offset() == Math::left) || 
					(right_check == Object::GroundState::NotDestroyed && get_velocity_offset() == Math::right)))
				{
					m_velocity = {};
				}
				else if (get_state() == eCharacterState::Move)
				{
					const auto bottom_local_position = ground->to_top_left_local_position(get_bottom());
					const auto next_velocity = get_next_velocity(bottom_local_position, ground);
					const auto is_toward = is_moving_toward(ground);

					Debug::Log(L"Ground : " + ground->get_name());
					Debug::Log(L"Is Toward:" + std::to_wstring(is_toward));
					Debug::Log(L"Velocity : " + std::to_wstring(next_velocity.get_x()) + L" , " + 
						std::to_wstring(next_velocity.get_y()));
					
					// @todo : inconsistency, need to find out why this is not working
					if(!is_toward && next_velocity != Math::vector_inf)
					{
						m_velocity = next_velocity;
					}
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
