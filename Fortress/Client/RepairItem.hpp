#ifndef REPAIRITEM_HPP
#define REPAIRITEM_HPP
#include "../Common/item.hpp"
#include "../Common/projectile.hpp"

namespace Fortress::Item
{
	class RepairItem : public Object::item
	{
	public:
		RepairItem() : item(L"Repair", true)
		{
			RepairItem::initialize();
		}

		void initialize() override;
		virtual void update(const std::weak_ptr<ObjectBase::character>& owner) override;
		void set_icon() override;
		void set_icon_thumbnail() override;
		~RepairItem() override = default;
	};

	inline void RepairItem::initialize()
	{
		item::initialize();
	}

	inline void RepairItem::update(const std::weak_ptr<ObjectBase::character>& owner)
	{
		if (const auto ch = owner.lock())
		{
			set_hp(owner, ch->get_hp_raw() + 50.0f);
			set_ended();
		}
	}

	inline void RepairItem::set_icon()
	{
		m_icon = Resource::ResourceManager::load<ImageWrapper>(
			L"Repair item", "./resources/images/items/repair.png");
	}
	
	inline void RepairItem::set_icon_thumbnail()
	{
		m_icon_thumbnail = Resource::ResourceManager::load<ImageWrapper>(
			L"Repair item Thumbnail", "./resources/images/items/repair-thumbnail.png");
	}
}
#endif // REPAIRITEM_HPP
