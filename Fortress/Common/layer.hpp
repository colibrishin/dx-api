#pragma once
#ifndef LAYER_HPP
#define LAYER_HPP
#include <vector>

#include "entity.hpp"
#include "object.hpp"

namespace Fortress::Abstract
{
	enum class LayerType
	{
		Debug = 0,
		Environment,
		Ground,
		Character,
		Projectile,
		UI,
		_END
	};

	inline const wchar_t* str_layer_type [] = {
	    L"Debug", L"Environment", L"Ground", L"Character", L"Projectile", L"UI", L"error",
	};

	class Layer final : public entity
	{
	public:
		Layer();
		Layer(LayerType);
		Layer& operator=(const Layer& other) = default;
		Layer& operator=(Layer&& other) = default;
		Layer(const Layer& other) = default;
		Layer(Layer&& other) = default;
		virtual ~Layer() final = default;

		void initialize();
		void update() const;
		void render() const;
		void deactivate() const;
		void activate() const;
		void add_game_object(const std::weak_ptr<object>& object);
		void remove_game_object(const std::weak_ptr<object>& obj);

	private:
		std::vector<std::weak_ptr<object>> m_objects;
	};
}

namespace Fortress::Abstract
{
	inline Layer::Layer() : entity(L"")
	{
	}

	inline Layer::Layer(LayerType layer_type):
		entity(str_layer_type[static_cast<unsigned int>(layer_type)])
	{
		initialize();
		m_objects = {};
	}

	inline void Layer::initialize()
	{
		m_objects = {};
	}

	inline void Layer::update() const
	{
		for (const auto& obj : m_objects)
		{
			if(const auto ptr = obj.lock())
			{
				ptr->update();	
			}
		}
	}

	inline void Layer::render() const
	{
		for(const auto& obj : m_objects)
		{
			if(const auto ptr = obj.lock())
			{
				ptr->render();	
			}
		}
	}

	inline void Layer::deactivate() const
	{
		for(const auto& obj : m_objects)
		{
			if(const auto ptr = obj.lock())
			{
				ptr->set_disabled();	
			}
		}
	}

	inline void Layer::activate() const
	{
		for(const auto& obj : m_objects)
		{
			if(const auto ptr = obj.lock())
			{
				ptr->set_enabled();	
			}
		}
	}

	inline void Layer::add_game_object(const std::weak_ptr<object>& object)
	{
		m_objects.push_back(object);
	}

	inline void Layer::remove_game_object(const std::weak_ptr<object>& obj)
	{
		m_objects.erase(
			std::remove_if(m_objects.begin(), m_objects.end(),
				[this, obj](const std::weak_ptr<object>& p)
			{
				return p.lock() == obj.lock();
			}),
			m_objects.end());
	}
}
#endif // LAYER_HPP
