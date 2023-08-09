#ifndef ITEM_HPP
#define ITEM_HPP
#include "CharacterController.hpp"
#include "entity.hpp"

namespace Fortress::Object
{
	class item : public Abstract::entity
	{
	public:
		item(const std::wstring& name, const bool instant)
			: entity(name), m_instant(instant), m_is_used(false), m_is_ended(false) {}

		~item() override = default;

		virtual void initialize() = 0;
		virtual void update(const std::weak_ptr<ObjectBase::character>& owner) = 0;
		virtual bool is_effect_ended();
		bool is_instant() const;
		virtual void reset();
		void set_ended();
	protected:
		void set_hp(const std::weak_ptr<ObjectBase::character>& owner, float hp) const;
		void equip_nutshell(const std::weak_ptr<ObjectBase::character>& owner);
		void unequip_nutshell(const std::weak_ptr<ObjectBase::character>& owner);
		void fire(const std::weak_ptr<ObjectBase::character>& owner) const;
	private:
		bool m_instant;
		bool m_is_used;
		bool m_is_ended;
	};

	inline bool item::is_effect_ended()
	{
		return m_is_ended;
	}

	inline bool item::is_instant() const
	{
		return m_instant;
	}

	inline void item::reset()
	{
		m_is_used = false;
		m_is_ended = false;
	}

	inline void item::set_ended()
	{
		m_is_ended = true;
	}

	inline void item::set_hp(const std::weak_ptr<ObjectBase::character>& owner, const float hp) const
	{
		const auto casted = owner.lock()->stateController::downcast_from_this<Controller::CharacterController>();
		casted->set_hp(hp);
	}

	inline void item::equip_nutshell(const std::weak_ptr<ObjectBase::character>& owner)
	{
		const auto casted = owner.lock()->stateController::downcast_from_this<Controller::CharacterController>();
		casted->equip_nutshell();
	}

	inline void item::unequip_nutshell(const std::weak_ptr<ObjectBase::character>& owner)
	{
		const auto casted = owner.lock()->stateController::downcast_from_this<Controller::CharacterController>();
		casted->unequip_nutshell();
	}

	inline void item::fire(const std::weak_ptr<ObjectBase::character>& owner) const
	{
		const auto casted = owner.lock()->stateController::downcast_from_this<Controller::CharacterController>();
		casted->fire();
	}
}
#endif // ITEM_HPP
