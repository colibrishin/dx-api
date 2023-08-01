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
			int start_pos = m_hitbox.get_x() / 4 ;

			for(int i = 0; i < m_hitbox.get_y(); ++i)
			{
				unsafe_set_line_destroyed({0, i}, start_pos);
				start_pos--;
			}
		}
		else if(m_ramp_offset == Math::left)
		{
			int start_pos = m_hitbox.get_x() / 4;

			for(int i = 0; i < m_hitbox.get_y(); ++i)
			{
				unsafe_set_line_destroyed_reverse(
					Math::Vector2{m_hitbox.get_x() - 1, i}, start_pos);
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
