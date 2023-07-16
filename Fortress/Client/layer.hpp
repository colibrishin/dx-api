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
		UI,
		_END
	};

	inline const wchar_t* str_layer_type [] = {
	    L"Debug", L"Environment", L"Ground", L"Character", L"UI", L"error",
	};

	class Layer final : public entity
	{
	public:
		Layer();
		Layer(LayerType);
		virtual ~Layer() final;

		void initialize();
		void update() const;
		void render() const;
		void deactivate() const;
		void activate() const;
		void add_game_object(const std::shared_ptr<object>& object);
		void remove_game_object(const std::shared_ptr<object>& obj);

	private:
		std::vector<std::shared_ptr<object>> m_objects;
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

	inline Layer::~Layer()
	{
		entity::~entity();
	}

	inline void Layer::initialize()
	{
		m_objects = {};
	}

	inline void Layer::update() const
	{
		for (const auto& obj : m_objects)
		{
			obj->update();
		}
	}

	inline void Layer::render() const
	{
		for(const auto& obj : m_objects)
		{
			obj->render();
		}
	}

	inline void Layer::deactivate() const
	{
		for(const auto& obj : m_objects)
		{
			obj->set_disabled();
		}
	}

	inline void Layer::activate() const
	{
		for(const auto& obj : m_objects)
		{
			obj->set_enabled();
		}
	}

	inline void Layer::add_game_object(const std::shared_ptr<object>& object)
	{
		m_objects.push_back(object);
	}

	inline void Layer::remove_game_object(const std::shared_ptr<object>& obj)
	{
		m_objects.erase(
			std::remove_if(m_objects.begin(), m_objects.end(),
				[this, obj](const std::shared_ptr<object>& p)
			{
				return p == obj;
			}),
			m_objects.end());
	}
}
#endif // LAYER_HPP
