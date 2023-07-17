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
	private:
		inline static std::vector<std::shared_ptr<Abstract::object>> m_objects = {};
	};

	template<typename T, typename... Args>
	inline std::weak_ptr<T> ObjectManager::create_object(Args... args)
	{
		m_objects.push_back(std::make_shared<T>(args...));
		return std::dynamic_pointer_cast<T>(m_objects.back());
	}
}
#endif // OBJECTMANAGER_HPP
