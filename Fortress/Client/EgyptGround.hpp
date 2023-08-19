#ifndef EGYPTGROUND_HPP
#define EGYPTGROUND_HPP
#include "../Common/ground.hpp"

namespace Fortress::Object
{
	class EgyptGround final : public Ground
	{
	public:
		EgyptGround(
			const std::wstring& name, 
			const Math::Vector2& position,
			const Math::Vector2& size) :
			Ground(
				name, 
				position, 
				size, 
				Resource::ResourceManager::load<ImageWrapper>(L"Egypt tile", "./resources/misc/tiles/egypt.png"))
		{
			EgyptGround::initialize();
		}
		EgyptGround& operator=(const EgyptGround& other) = default;
		EgyptGround& operator=(EgyptGround&& other) = default;

		void initialize() override;
	};

	inline void EgyptGround::initialize()
	{
		Ground::initialize();
	}
}
#endif // EGYPTGROUND_HPP
