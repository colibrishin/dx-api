#ifndef REPAIRITEM_HPP
#define REPAIRITEM_HPP
#include "item.hpp"
#include "projectile.hpp"

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
		~RepairItem() override = default;
	};

	inline void RepairItem::initialize()
	{
	}

	inline void RepairItem::update(const std::weak_ptr<ObjectBase::character>& owner)
	{
		if (const auto ch = owner.lock())
		{
			set_hp(owner, ch->get_hp_raw() + 50.0f);
			set_ended();
		}
	}
}
#endif // REPAIRITEM_HPP
