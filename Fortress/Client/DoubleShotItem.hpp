#ifndef DOUBLESHOTITEM_HPP
#define DOUBLESHOTITEM_HPP
#include "item.hpp"
#include "projectile.hpp"

namespace Fortress::Item
{
	class DoubleShotItem : public Object::item
	{
	public:
		DoubleShotItem() : item(L"Double shot", false), m_fire_count(0)
		{
			DoubleShotItem::initialize();
		}

		void initialize() override;
		virtual void update(const std::weak_ptr<ObjectBase::character>& owner) override;
		~DoubleShotItem() override = default;
		virtual void reset() override;
		void set_icon() override;
		void set_icon_thumbnail() override;

	private:
		int m_fire_count;
	};

	inline void DoubleShotItem::initialize()
	{
		item::initialize();
		m_fire_count = 0;
	}

	inline void DoubleShotItem::update(const std::weak_ptr<ObjectBase::character>& owner)
	{
		if (const auto ch = owner.lock())
		{
			const auto prjs = ch->get_projectiles();

			if (!ch->is_projectile_active() && m_fire_count != 2)
			{
				fire(owner);
				m_fire_count++;
			}
			else if(!ch->is_projectile_active() && m_fire_count == 2)
			{
				// does this really need to be separated with used count?
				set_ended();
			}
		}
	}

	inline void DoubleShotItem::reset()
	{
		m_fire_count = 0;
		item::reset();
	}

	inline void DoubleShotItem::set_icon()
	{
		m_icon = Resource::ResourceManager::load<ImageWrapper>(
			L"Double shot item", "./resources/images/items/double-shot.png");
	}

	inline void DoubleShotItem::set_icon_thumbnail()
	{
		m_icon_thumbnail = Resource::ResourceManager::load<ImageWrapper>(
			L"Double Shot Item Thumbnail", "./resources/images/items/double-shot-thumbnail.png");
	}
}
#endif // DOUBLESHOTITEM_HPP
