#ifndef CHARACTERCONTROLLER_HPP
#define CHARACTERCONTROLLER_HPP
#include "common.h"
#include "rigidBody.hpp"
#include "stateController.hpp"

namespace Fortress
{
	namespace Network
	{
		namespace Client
		{
			namespace Object
			{
				class ClientCharacter;
			}
		}
	}
}

namespace Fortress
{
	namespace Object
	{
		class item;
	}
}

namespace Fortress
{
	namespace ObjectBase
	{
		class projectile;
		class character;
	}
}

namespace Fortress::Controller
{
	using ProjectilePointer = std::weak_ptr<ObjectBase::projectile>;
	using RefOnlyRigidBodyPointer = const Abstract::rigidBody* const;

	class CharacterController : public stateController<eCharacterState>
	{
	public:
		CharacterController() = delete;
		~CharacterController() override = default;

		void initialize() override;
		void update() override;
		void prerender() override;

		void set_unmovable();
		void set_movable();
		bool is_movable_localplayer() const;
		bool is_localplayer() const;

		bool is_projectile_fire_counted() const;
		bool is_projectile_active() const;

		float get_charged_power() const;
		float get_hp_percentage() const;
		float get_mp_percentage() const;
		float get_hp_raw() const;
		int get_previous_hit_count() const;

		const std::vector<std::weak_ptr<ObjectBase::projectile>>& get_projectiles() const;
		const eProjectileType& get_projectile_type() const;
		std::weak_ptr<ObjectBase::projectile> get_one_active_projectile();

		void add_item(const unsigned int index, const std::shared_ptr<Object::item>& item);
		std::vector<std::pair<const int, std::weak_ptr<Object::item>>> get_available_items() const;

	protected:
		CharacterController(
			const Network::PlayerID& player_id,
			const std::wstring& short_name, 
			float hp,
			float mp,
			RefOnlyRigidBodyPointer rb);

		friend class Object::item;

		void set_sprite_offset(const std::wstring& name, const std::wstring& orientation, const Math::Vector2& offset);
		void set_sprite_rotation_offset(const std::wstring& name, const std::wstring& orientation,
		                                const Math::Vector2& offset);
		const std::wstring& get_current_sprite_name() const;

		virtual void fire();
		virtual void move_left();
		virtual void move_right();
		virtual void stop();
		virtual void move();

		void apply_damage(float damage);
		void set_hp(const float hp);

		void set_double_damage();
		bool is_double_damage() const;

		void add_active_projectile(const std::weak_ptr<ObjectBase::projectile>& prj);

		virtual void change_projectile();
		virtual void equip_nutshell();
		virtual void unequip_nutshell();

		unsigned int m_active_item_index;
		std::weak_ptr<Object::item> m_active_item;

		virtual std::weak_ptr<ObjectBase::projectile> get_main_projectile() = 0;
		virtual std::weak_ptr<ObjectBase::projectile> get_sub_projectile() = 0;
		virtual std::weak_ptr<ObjectBase::projectile> get_nutshell_projectile() = 0;

	private:
		friend Network::Client::Object::ClientCharacter;
		static std::wstring anim_name_getter(const eCharacterState& state);
		void set_current_sprite(const eCharacterState&) override;

		void default_state();
		void idle_state();
		void move_state();
		void firing_state();
		void fire_state();
		void fired_state();
		void hit_state();
		void turn_end_state();
		void preitem_state();
		void item_state();
		void death_state();
		void dead_state();
		void set_item_active(const unsigned int n);

		void reset_mp();

		const Network::PlayerID& m_player_id_;

		float m_hp;
		float m_mp;
		float m_power;
		bool m_bMovable;
		int m_previousHitCount;
		bool m_bDoubleDamage;

		const Abstract::rigidBody* const m_rb;

		eProjectileType m_projectile_type;
		eProjectileType m_tmp_projectile_type;

		std::map<unsigned int, std::shared_ptr<Object::item>> m_available_items;

		std::vector<std::weak_ptr<ObjectBase::projectile>> m_active_projectiles;
	};
}
#endif // CHARACTERCONTROLLER_HPP
