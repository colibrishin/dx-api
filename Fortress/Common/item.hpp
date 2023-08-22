#ifndef ITEM_HPP
#define ITEM_HPP
#include "CharacterController.hpp"
#include "entity.hpp"

namespace Fortress::Object
{
	class item : public Abstract::entity
	{
	public:
		item(const std::wstring& name, const Network::eItemType item_type, const bool instant)
			: entity(name), m_instant(instant), m_is_used(false), m_is_ended(false), m_item_type_(item_type) {}

		~item() override = default;

		virtual void initialize();
		virtual void update(const std::weak_ptr<ObjectBase::character>& owner) = 0;

		virtual void set_icon() = 0;
		virtual void set_icon_thumbnail() = 0;

		std::weak_ptr<ImageWrapper> get_icon() const;
		std::weak_ptr<ImageWrapper> get_icon_thumbnail() const;
		Network::eItemType get_item_type() const;

		virtual bool is_effect_ended();
		bool is_instant() const;
		bool is_used() const;

		virtual void reset();
		void set_ended();
	protected:
		void set_hp(const std::weak_ptr<ObjectBase::character>& owner, float hp) const;
		void equip_nutshell(const std::weak_ptr<ObjectBase::character>& owner);
		void unequip_nutshell(const std::weak_ptr<ObjectBase::character>& owner);
		void fire(const std::weak_ptr<ObjectBase::character>& owner) const;
		void set_double_damage(const std::weak_ptr<ObjectBase::character>& owner);

		std::weak_ptr<ImageWrapper> m_icon;
		std::weak_ptr<ImageWrapper> m_icon_thumbnail;
	private:
		bool m_instant;
		bool m_is_used;
		bool m_is_ended;
		Network::eItemType m_item_type_;
	};

	inline void item::initialize()
	{
		set_icon();
		set_icon_thumbnail();
	}

	inline std::weak_ptr<ImageWrapper> item::get_icon() const
	{
		return m_icon;
	}

	inline std::weak_ptr<ImageWrapper> item::get_icon_thumbnail() const
	{
		return m_icon_thumbnail;
	}

	inline Network::eItemType item::get_item_type() const
	{
		return m_item_type_;
	}

	inline bool item::is_effect_ended()
	{
		return m_is_ended;
	}

	inline bool item::is_instant() const
	{
		return m_instant;
	}

	inline bool item::is_used() const
	{
		return m_is_used;
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
	inline void item::set_double_damage(const std::weak_ptr<ObjectBase::character>& owner)
	{
		const auto casted = owner.lock()->stateController::downcast_from_this<Controller::CharacterController>();
		casted->set_double_damage();
	}
}
#endif // ITEM_HPP
