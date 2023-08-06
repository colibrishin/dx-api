#pragma once
#ifndef SCENE_HPP
#define SCENE_HPP
#include <vector>

#include "camera.hpp"
#include "cameraManager.hpp"
#include "entity.hpp"
#include "layer.hpp"
#include "objectManager.hpp"

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
		std::vector<std::weak_ptr<T>> is_in_range(const Math::Vector2& mid_point, float radius);

		void add_game_object(LayerType layer_type, const std::weak_ptr<object>& obj);
		void remove_game_object(LayerType layer_type, const std::weak_ptr<object>& obj);

		template <class T>
		std::vector<std::weak_ptr<T>> get_objects();
		std::weak_ptr<Camera> get_camera();

		std::vector<std::weak_ptr<object>> get_objects();

	private:
		std::weak_ptr<Camera> m_camera;
		std::vector<std::weak_ptr<object>> m_objects{};
		std::vector<Layer> m_layers;
	};

	inline std::vector<std::weak_ptr<object>> scene::get_objects()
	{
		return m_objects;
	}

	inline void scene::add_game_object(LayerType layer_type, const std::weak_ptr<object>& obj)
	{
		m_layers[static_cast<unsigned int>(layer_type)].add_game_object(obj);
		m_objects.push_back(obj);
	}

	inline void scene::remove_game_object(LayerType layer_type, const std::weak_ptr<object>& obj)
	{
		ObjectBase::ObjectManager::remove_object(obj);
		m_layers[static_cast<unsigned int>(layer_type)].remove_game_object(obj);
		m_objects.erase(
			std::remove_if(m_objects.begin(), m_objects.end(),
				[this, obj](const std::weak_ptr<object>& p)
			{
				return p.lock() == obj.lock();
			}),
			m_objects.end());
	}

	inline std::weak_ptr<Camera> scene::get_camera()
	{
		return m_camera;
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

		m_camera = CameraManager::create_camera<Camera>();
		m_camera.lock()->initialize();
	}

	inline void scene::update()
	{
		if(const auto ptr = m_camera.lock())
		{
			ptr->update();
		}

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
	std::vector<std::weak_ptr<T>> scene::is_in_range(
		const Math::Vector2& mid_point,
		const float radius)
	{
		std::vector<std::weak_ptr<T>> ret = {};

		static_assert(std::is_base_of_v<object, T>);

		for (const auto& obj : m_objects)
		{
			if (const auto ptr = std::dynamic_pointer_cast<T>(obj.lock()))
			{
				if(ptr->is_active())
				{
					const Math::Vector2 local_position = ptr->to_nearest_local_position(mid_point);
					const float distance = local_position.magnitude();

					if (distance <= radius)
					{
						ret.push_back(ptr);
					}
				}
			}
		}

		std::sort(
			ret.begin(), 
			ret.end(), 
			[mid_point](
				const std::weak_ptr<T>& left, const std::weak_ptr<T>& right)
		{
			return Math::Vector2(left.lock()->m_position - mid_point).magnitude() <
				Math::Vector2(right.lock()->m_position - mid_point).magnitude();
		});

		return ret;
	}

	template <class T>
	std::vector<std::weak_ptr<T>> scene::get_objects()
	{
		std::vector<std::weak_ptr<T>> ret = {};

		for(const auto& obj : m_objects)
		{
			if(const auto cast = std::dynamic_pointer_cast<T>(obj.lock()))
			{
				ret.push_back(cast);
			}
		}

		return ret;
	}
}
#endif // SCENE_HPP
