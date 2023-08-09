#ifndef VIRTUAL_THIS_HPP
#define VIRTUAL_THIS_HPP
#include <memory>

namespace Fortress::Dev
{
	class _MISP : public std::enable_shared_from_this<_MISP>
	{
	public:
		virtual ~_MISP() = default;
	};

	template <typename T>
	class multi_inherit_this : public virtual _MISP
	{
	public:
		std::shared_ptr<T> shared_from_this()
		{
			return std::dynamic_pointer_cast<T>(_MISP::shared_from_this());
		}
		
		template <typename U>
		std::shared_ptr<U> downcast_from_this()
		{
			return std::dynamic_pointer_cast<U>(_MISP::shared_from_this());
		}
	};
}
#endif // VIRTUAL_THIS_HPP
