#ifndef DOUBLEDAMAGEITEM_HPP
#define DOUBLEDAMAGEITEM_HPP
#include "item.hpp"
#include "projectile.hpp"

namespace Fortress::Item
{
	class DoubleDamageItem : public Object::item
	{
	public:
		DoubleDamageItem() : item(L"Double shot", false)
		{
			DoubleDamageItem::initialize();
		}

		void initialize() override;
		virtual void update(const std::weak_ptr<ObjectBase::character>& owner) override;
		~DoubleDamageItem() override = default;
		virtual void reset() override;
	};

	inline void DoubleDamageItem::initialize()
	{
	}

	inline void DoubleDamageItem::update(const std::weak_ptr<ObjectBase::character>& owner)
	{
		if (const auto ch = owner.lock())
		{
			const auto prjs = ch->get_projectiles();

			if (!ch->is_projectile_active())
			{
				set_double_damage(owner);
				fire(owner);
			}
			else if(!ch->is_projectile_active())
			{
				set_ended();
			}
		}
	}
}
#endif // DOUBLEDAMAGEITEM_HPP
