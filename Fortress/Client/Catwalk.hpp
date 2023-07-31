#ifndef CATWALK_HPP
#define CATWALK_HPP
#include "ground.hpp"

namespace Fortress::Object
{
	class Catwalk : public Ground
	{
		public:
		Catwalk(const std::wstring& name, const Math::Vector2& position, const Math::Vector2& size) : Ground(name, position, size)
		{
			Catwalk::initialize();
		}
		Catwalk& operator=(const Catwalk& other) = default;
		Catwalk& operator=(Catwalk&& other) = default;

		void initialize() override;
	};

	inline void Catwalk::initialize()
	{
		Ground::initialize();

		int x_tmp = m_hitbox.get_x();

		for(int i = 0; i < m_hitbox.get_y(); ++i)
		{
			for(int j = 0; j < x_tmp; ++j)
			{
				unsafe_set_destroyed(j, i);
				unsafe_set_destroyed_visual(j, i);
			}

			x_tmp--;
		}
	}
}
#endif // CATWALK_HPP
