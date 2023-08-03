#pragma once
#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "AnimationQueue.hpp"
#include "GifWrapper.hpp"
#include "rigidBody.hpp"
#include "common.h"
#include "sceneManager.hpp"
#include "sound.hpp"
#include "SoundPack.hpp"
#include "Texture.hpp"

namespace Fortress
{
	namespace Item
	{
		class DoubleShotItem;
	}
}

// forward declaration for avoiding circular reference
namespace Fortress
{
	namespace Object
	{
		class NutShellProjectile;
		class item;
		enum class GroundState;
		class Ground;
	}
}

namespace Fortress::ObjectBase
{
	class projectile;

	constexpr float character_full_hp = 100.0f;
	constexpr float character_full_mp = 1000.0f;
	constexpr float character_max_charge = 250.0f;

	enum class eCharacterAnimation
	{
		// order is priority of animation.
		Dead = 0,
		Item,
		Hit,
		Fire,
		Death,
		Charging,
		Move,
		Idle
	};

	static std::wstring get_animation_name(const eCharacterAnimation& anim)
	{
		switch(anim)
		{
		case eCharacterAnimation::Dead: return L"dead";
		case eCharacterAnimation::Item: return L"item";
		case eCharacterAnimation::Hit: return L"hit";
		case eCharacterAnimation::Death: return L"death";
		case eCharacterAnimation::Charging: return L"charging";
		case eCharacterAnimation::Move: return L"move";
		case eCharacterAnimation::Idle: return L"idle";
		case eCharacterAnimation::Fire: return L"fire";
		default: return L"";
		}
	}

	class character : public Abstract::rigidBody
	{
	public:
		character() = delete;
		character& operator=(const character& other) = default;
		character& operator=(character&& other) = default;
		character(const character& other) = default;
		character(character&& other) = default;
		~character() override = default;

		virtual void initialize() override;
		void hit(const std::weak_ptr<projectile>& p);
		void hit(const float damage);
		void post_hit();
		void update() override;
		void render() override;
		virtual void firing();
		virtual void shoot();
		float get_charged_power() const;
		virtual void move() override;
		void change_projectile();
		void equip_nutshell();
		void unequip_nutshell();
		std::weak_ptr<projectile> get_current_projectile();
		const std::wstring& get_short_name() const;

		virtual void on_collision(const CollisionCode& collision, const Math::Vector2& hit_vector, const std::weak_ptr<Abstract::rigidBody>& other) override;
		virtual void on_nocollison() override;

		eCharacterState get_state() const;
		void default_state();
		void idle_state();
		void move_state();
		void firing_state();
		void fire_state();
		virtual void item_state();
		virtual void dead_state();

		void set_hp(const float hp);
		void set_unmovable();
		void set_movable();
		void reset_mp();
		void set_item_active(int n);

		void push_sprite(const eCharacterAnimation& animation, bool override);
		void set_sprite_offset(const std::wstring& name, const std::wstring& orientation, const Math::Vector2& offset);
		const std::wstring& get_current_sprite_name() const;
		
		virtual void move_left() override;
		virtual void move_right() override;
		virtual void stop() override;
		virtual void prerender();

		float get_hp_percentage() const;
		float get_mp_percentage() const;
		float get_hp_raw() const;

	private:
		int m_player_id;
		float m_hp;
		float m_mp;
		float m_power;
		bool m_bGrounded;
		bool m_bMovable;
		const std::wstring m_shot_name;
		eCharacterState m_state;

		void set_state(const eCharacterState& state);
		void render_hp_bar(const Math::Vector2& position);

		void ground_walk(const CollisionCode& collision, const std::weak_ptr<Object::Ground>& ptr_ground);
		void ground_gravity(const std::weak_ptr<Object::Ground>& ptr_ground);
		void ground_pitching(const std::weak_ptr<Object::Ground>& ptr_ground);

		Math::Vector2 get_next_velocity(const Math::Vector2& local_position_bottom,
		                                const std::weak_ptr<Object::Ground>& ground_ptr) const;

		Texture<GifWrapper> m_texture;
		Resource::AnimationQueue m_animation_queue;

		SoundPack m_sound_pack;
		std::weak_ptr<GifWrapper> m_current_sprite;

		std::weak_ptr<projectile> m_current_projectile;
		std::shared_ptr<projectile> m_main_projectile;
		std::shared_ptr<projectile> m_secondary_projectile;

		// @todo: nutshell doesn't have to be exist for every character
		std::shared_ptr<Object::NutShellProjectile> m_nutshell_projectile;
		std::weak_ptr<projectile> m_tmp_projectile;

		std::map<int, std::shared_ptr<Object::item>> m_available_items;
		std::weak_ptr<Object::item> m_active_item;
	protected:
		character(
			const int player_id,
			const std::wstring& name,
			const std::wstring& short_name,
			const Math::Vector2 offset,
			const Math::Vector2 position,
			const Math::Vector2 velocity,
			const float mass,
			const Math::Vector2& speed,
			const Math::Vector2& acceleration,
			const int hp,
			const int mp,
			const std::weak_ptr<projectile>& main_projectile,
			const std::weak_ptr<projectile>& secondary_projectile):
			rigidBody(name, position, {50.0f, 50.0f}, velocity, mass, speed, acceleration, true),
			m_player_id(player_id),
			m_hp(hp),
			m_mp(mp),
			m_power(1.0f),
			m_bGrounded(false),
			m_bMovable(true),
			m_shot_name(short_name),
			m_state(eCharacterState::Idle),
			m_texture(short_name),
			m_sound_pack(short_name),
			m_main_projectile(main_projectile),
			m_secondary_projectile(secondary_projectile),
			m_available_items{},
			m_active_item{}
		{
			character::initialize();
		}
	};
}

#endif
