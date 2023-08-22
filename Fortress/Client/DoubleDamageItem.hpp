#ifndef DOUBLEDAMAGEITEM_HPP
#define DOUBLEDAMAGEITEM_HPP
#include "../Common/item.hpp"
#include "../Common/projectile.hpp"

namespace Fortress::Item
{
	class DoubleDamageItem : public Object::item
	{
	public:
		DoubleDamageItem() : item(L"Double damage", Network::eItemType::DoubleDamage, false)
		{
			DoubleDamageItem::initialize();
		}

		void initialize() override;
		virtual void update(const std::weak_ptr<ObjectBase::character>& owner) override;
		void set_icon() override;
		void set_icon_thumbnail() override;
		~DoubleDamageItem() override = default;
	};

	inline void DoubleDamageItem::initialize()
	{
		item::initialize();
	}

	inline void DoubleDamageItem::update(const std::weak_ptr<ObjectBase::character>& owner)
	{
		if (const auto ch = owner.lock())
		{
			const auto prjs = ch->get_projectiles();

			if (prjs.empty())
			{
				set_double_damage(owner);
				fire(owner);
			}
			else if (ch->is_projectile_fire_counted() && !ch->is_projectile_active())
			{
				set_ended();
			}
		}
	}

	inline void DoubleDamageItem::set_icon()
	{
		m_icon = Resource::ResourceManager::load<ImageWrapper>(
			L"Double Damage item", "./resources/images/items/double-damage.png");
	}

	inline void DoubleDamageItem::set_icon_thumbnail()
	{
		m_icon_thumbnail = Resource::ResourceManager::load<ImageWrapper>(
			L"Double Damage item Thumbnail", "./resources/images/items/double-damage-thumbnail.png");
	}
}
#endif // DOUBLEDAMAGEITEM_HPP
