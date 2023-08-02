#ifndef DOUBLESHOTITEM_HPP
#define DOUBLESHOTITEM_HPP
#include "item.hpp"
#include "projectile.hpp"

namespace Fortress::Item
{
	class DoubleShotItem : public Object::item
	{
	public:
		DoubleShotItem() : item(L"Double shot"), m_fire_count(0)
		{
			DoubleShotItem::initialize();
		}

		void initialize() override;
		virtual void update(const std::weak_ptr<ObjectBase::character> owner) override;
		~DoubleShotItem() override = default;
		virtual void reset() override;

	private:
		int m_fire_count;
	};

	inline void DoubleShotItem::initialize()
	{
		m_fire_count = 0;
	}

	inline void DoubleShotItem::update(const std::weak_ptr<ObjectBase::character> owner)
	{
		if (const auto ch = owner.lock())
		{
			if (!ch->get_current_projectile().lock()->is_active() && m_fire_count != 2)
			{
				ch->shoot();
				m_fire_count++;
			}
			else if(!ch->get_current_projectile().lock()->is_active() && m_fire_count == 2)
			{
				// does this really need to be separated with used count?
				set_ended();
				item::update(owner);
			}
		}
	}

	inline void DoubleShotItem::reset()
	{
		m_fire_count = 0;
		item::reset();
	}
}
#endif // DOUBLESHOTITEM_HPP
