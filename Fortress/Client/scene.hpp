#pragma once
#ifndef SCENE_HPP
#define SCENE_HPP
#include <vector>

#include "camera.hpp"
#include "cameraManager.hpp"
#include "entity.hpp"
#include "layer.hpp"
#include "objectManager.hpp"

namespace Fortress
{
	namespace Dev
	{
		struct weakPointerComparer;
	}
}

namespace Fortress::Abstract
{
	class scene : public entity
	{
	public:
		scene(const std::wstring& name);
		~scene() override;

		virtual void initialize();
		virtual void update();
		virtual void render();
		virtual void deactivate();
		virtual void activate();

		template <class T>
		std::vector<std::weak_ptr<T>> is_in_range(const Math::Vector2& mid_point, float radius);
		template <class T>
		std::vector<std::pair<std::weak_ptr<T>, std::pair<float, Math::Vector2>>> is_in_range_nearest(
			const std::weak_ptr<object>& pivot, float radius);

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

		struct weakPointerComparer {
		    bool operator() (
				const std::weak_ptr<Abstract::object> &lhs, 
				const std::weak_ptr<Abstract::object> &rhs) const
		{
		        const auto lptr = lhs.lock();
		    	const auto rptr = rhs.lock();
		        if (!rptr) return false; // nothing after expired pointer 
		        if (!lptr) return true;  // every not expired after expired pointer
		        return lptr.get() < rptr.get();
		    }
		};
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

	inline scene::~scene()
	{
		for(const auto& obj : m_objects)
		{
			ObjectBase::ObjectManager::remove_object(obj);
		}
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
		const Math::Vector2& mid_point,	const float radius)
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

	template <typename T>
	std::vector<std::pair<std::weak_ptr<T>, std::pair<float, Math::Vector2>>> scene::is_in_range_nearest(
		const std::weak_ptr<object>& pivot, const float radius)
	{
		static_assert(std::is_base_of_v<object, T>);

		using DistanceVectorPair = std::pair<float, Math::Vector2>;
		using MapPair = std::pair<std::weak_ptr<T>, DistanceVectorPair>;

		std::map<std::weak_ptr<T>, DistanceVectorPair, weakPointerComparer> map = {};
		const auto pivot_ptr = pivot.lock();
		const std::vector<Math::Vector2> compare_list = pivot_ptr->get_all_hit_points();

		for (const auto& obj : m_objects)
		{
			if (const auto ptr = std::dynamic_pointer_cast<T>(obj.lock()))
			{
				if(!ptr->is_active())
				{
					continue;
				}

				for(const auto& point : compare_list)
				{
					const Math::Vector2 local_position = ptr->to_nearest_local_position(point);
					const float distance = local_position.magnitude();

					if (distance <= radius)
					{
						if(map.find(ptr) != map.end() && map[ptr].first > distance)
						{
							map[ptr] = {distance, ptr->get_nearest_point(point)};
							continue;
						}

						map[ptr] = {distance, ptr->get_nearest_point(point)};
					}
				}
			}
		}

		std::vector<MapPair> sorted(map.begin(), map.end());
		std::sort(sorted.begin(), sorted.end(), []
			(const MapPair& left, const MapPair& right)
			{
				return left.second.first < right.second.first;
			});

		return sorted;
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
