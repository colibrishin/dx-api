#ifndef EGYPTSTAIRWAY_HPP
#define EGYPTSTAIRWAY_HPP
#include "../Common/ground.hpp"
#include "Stairway.hpp"

namespace Fortress::Object
{
	class EgyptStairway : public Stairway
	{
	public:
		EgyptStairway(
			const std::wstring& name, 
			const Math::Vector2& position, 
			const Math::Vector2& size, 
			const Math::Vector2& ramp_offset) :
			Stairway(
				name, 
				position, 
				size, 
				ramp_offset, 
				Resource::ResourceManager::load<ImageWrapper>(L"Egypt tile", "./resources/misc/tiles/egypt.png"))
		{
			EgyptStairway::initialize();
		}
		EgyptStairway& operator=(const EgyptStairway& other) = default;
		EgyptStairway& operator=(EgyptStairway&& other) = default;

		void initialize() override;
	};

	inline void EgyptStairway::initialize()
	{
		Stairway::initialize();
	}
}
#endif // EGYPTSTAIRWAY_HPP
