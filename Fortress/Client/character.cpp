#include "character.h"

#include "object.h"

Object::CollisionCode Object::character::is_collision(const character& object) const
{
    const auto diff = object.get_position() - m_position;
    const auto dist = std::powf(diff.get_x(), 2) + std::powf(diff.get_y(), 2);

    // @todo: bug, if two object is intersects, their collisions are done indefinitely.
    // identical
    if(m_position == object.get_position())
    {
	    return CollisionCode::Identical;
    }

    const auto hitbox_diff = m_hitbox - object.m_hitbox;
    const auto hitbox_sum = m_hitbox + object.m_hitbox;

    // Too far
    if(hitbox_diff.get_x() > dist || hitbox_sum.get_x() < dist ||
        hitbox_diff.get_y() > dist || hitbox_sum.get_y() < dist)
    {
	    return CollisionCode::None;
    }

    // Collision, met each other in radius.
    if(hitbox_diff.get_x() - dist < Math::epsilon || hitbox_sum.get_x() - dist < Math::epsilon)
    {
	    return CollisionCode::XHitBoundary;
    }
    if (hitbox_diff.get_x() < dist && dist < hitbox_sum.get_x())
    {
        return CollisionCode::XHitInside;
    }

    if (hitbox_diff.get_y() - dist < Math::epsilon || hitbox_sum.get_y() - dist < Math::epsilon)
    {
		return CollisionCode::YHitBoundary;   
    }
    if (hitbox_diff.get_y() < dist && dist < hitbox_sum.get_y())
    {
	    return CollisionCode::YHitInside;
    }

    return CollisionCode::None;
}
