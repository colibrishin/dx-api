#pragma once
#ifndef SCENE_HPP
#define SCENE_HPP
#include <vector>

#include "entity.hpp"
#include "layer.hpp"

namespace Fortress::Abstract
{
	class scene : public entity
	{
	public:
		scene(const std::wstring& name);

		virtual void initialize();
		virtual void update();
		virtual void render();
		virtual void deactivate();
		virtual void activate();
		template <class T>
		std::vector<T*> is_in_range(const Math::Vector2& top_left, const Math::Vector2& hit_box, float radius);
		void add_game_object(LayerType layer_type, object* obj);
		virtual void check_ground();

		std::vector<object*> get_objects();

	protected:
		std::vector<Layer> m_layers;
		std::vector<object*> m_objects{};
	};

	inline void scene::check_ground()
	{
	}

	inline std::vector<object*> scene::get_objects()
	{
		return m_objects;
	}

	inline void scene::add_game_object(LayerType layer_type, object* obj)
	{
		m_layers[static_cast<unsigned int>(layer_type)].add_game_object(obj);
		m_objects.push_back(obj);
	}

	inline scene::scene(const std::wstring& name):
		entity(name),
		m_objects(0)
	{
		scene::initialize();
	}

	inline void scene::initialize()
	{
		for(int i = 0; i < static_cast<unsigned int>(LayerType::_END); ++i)
		{
			m_layers.emplace_back(static_cast<LayerType>(i));
		}
	}

	inline void scene::update()
	{
		for(const auto& l : m_layers)
		{
			l.update();
		}
	}

	inline void scene::render()
	{
		for(const auto& l : m_layers)
		{
			l.render();
		}
	}

	inline void scene::deactivate()
	{
		for(const auto& l : m_layers)
		{
			l.deactivate();
		}
	}

	inline void scene::activate()
	{
		for(const auto& l : m_layers)
		{
			l.activate();
		}
	}

	template <typename T>
	std::vector<T*> scene::is_in_range(
		const Math::Vector2& top_left,
		const Math::Vector2& hit_box,
		const float radius)
	{
		std::vector<T*> ret = {};

		static_assert(std::is_base_of_v<object, T>);

		const auto mid_point = Math::Vector2{
			top_left.get_x() + hit_box.get_x() / 2,
			top_left.get_y() + hit_box.get_y() / 2
		};

		for (const auto obj : m_objects)
		{
			if (typeid(obj) != typeid(T) || !obj->is_active())
			{
				continue;
			}

			if (obj->m_position.get_x() <= mid_point.get_x() + radius &&
				obj->m_position.get_x() >= mid_point.get_x() - radius &&
				obj->m_position.get_y() <= mid_point.get_y() + radius &&
				obj->m_position.get_y() >= mid_point.get_y() - radius)
			{
				ret.push_back(obj);
			}
		}

		return ret;
	}
}
#endif // SCENE_HPP