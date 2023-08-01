#ifndef CATWALK_HPP
#define CATWALK_HPP
#include "ground.hpp"

namespace Fortress::Object
{
	class Stairway : public Ground
	{
	public:
		Stairway(const std::wstring& name, const Math::Vector2& position, const Math::Vector2& size, const Math::Vector2& ramp_offset) :
			Ground(name, position, size),
			m_ramp_offset(ramp_offset)
		{
			Stairway::initialize();
		}
		Stairway& operator=(const Stairway& other) = default;
		Stairway& operator=(Stairway&& other) = default;

		void initialize() override;
	private:
		Math::Vector2 m_ramp_offset;
	};

	inline void Stairway::initialize()
	{
		Ground::initialize();

		if(m_ramp_offset == Math::right)
		{
			int start_pos = m_hitbox.get_x() / 4;

			for(int i = 1; i < m_hitbox.get_y(); ++i)
			{
				for(int j = 1; j < start_pos; ++j)
				{
					unsafe_set_destroyed(j, i);
					unsafe_set_destroyed_visual(j, i);
				}

				start_pos--;
			}
		}
		else if(m_ramp_offset == Math::left)
		{
			int start_pos = m_hitbox.get_x() / 4;

			for(int i = 0; i < m_hitbox.get_y(); ++i)
			{
				for(int j = 0; j < start_pos; ++j)
				{
					unsafe_set_destroyed(m_hitbox.get_x() - 1 - j, i);
					unsafe_set_destroyed_visual(m_hitbox.get_x() - 1 - j, i);
				}

				start_pos--;
			}
		}
		else
		{
			assert(nullptr);
		}
		
	}
}
#endif // CATWALK_HPP
