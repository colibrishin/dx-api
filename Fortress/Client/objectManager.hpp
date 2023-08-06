#ifndef OBJECTMANAGER_HPP
#define OBJECTMANAGER_HPP
#include "object.hpp"

namespace Fortress::ObjectBase
{
	class ObjectManager
	{
	public:
		template <typename T, typename... Args>
		static std::weak_ptr<T> create_object(Args ... args);
		static void remove_object(const std::weak_ptr<Abstract::object>& obj);
		static void cleanup();

	private:
		inline static std::vector<std::shared_ptr<Abstract::object>> m_objects = {};
	};

	template<typename T, typename... Args>
	inline std::weak_ptr<T> ObjectManager::create_object(Args... args)
	{
		m_objects.push_back(std::make_shared<T>(args...));
		return std::dynamic_pointer_cast<T>(m_objects.back());
	}

	inline void ObjectManager::remove_object(const std::weak_ptr<Abstract::object>& obj)
	{
		m_objects.erase(
			std::remove_if(m_objects.begin(), m_objects.end(), [obj]
			(const std::shared_ptr<Abstract::object>& v_obj)
			{
				return v_obj == obj.lock();
			}),
			m_objects.end());
	}

	inline void ObjectManager::cleanup()
	{
		for(auto& object : m_objects)
		{
			object.reset();
		}
	}
}
#endif // OBJECTMANAGER_HPP
