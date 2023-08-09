#ifndef STATECONTROLLER_HPP
#define STATECONTROLLER_HPP
#include "deltatime.hpp"
#include "GifWrapper.hpp"
#include "SoundPack.hpp"
#include "Texture.hpp"
#include "virtual_this.hpp"

namespace Fortress::Controller
{
	using SpritePointer = std::weak_ptr<GifWrapper>;
	using AnimFlag = bool;
	using AnimElapsedFloat = float;

	template <typename StateEnum>
	class stateController : public Dev::multi_inherit_this<stateController<StateEnum>>
	{
	public:
		stateController(
			const std::wstring& short_name,
			const StateEnum& m_state) :
			m_texture(short_name),
			m_sound_pack(short_name),
			m_state(m_state),
			m_anim_elapsed(0.0f)
		{}

		StateEnum get_state();

		virtual void initialize();
		virtual void update() = 0;
		virtual void prerender();

	protected:
		void set_state(const StateEnum&);

		virtual void set_current_sprite(const StateEnum&) = 0;
		AnimFlag is_anim_finished() const;
		void reset_anim_counter();

		Texture<GifWrapper> m_texture;
		SpritePointer m_current_sprite;
		SoundPack m_sound_pack;
	private:
		StateEnum m_state;
		AnimElapsedFloat m_anim_elapsed;
	};

	template <typename StateEnum>
	AnimFlag stateController<StateEnum>::is_anim_finished() const
	{
		if(const auto sprite = m_current_sprite.lock())
		{
			return m_anim_elapsed >= static_cast<float>(sprite->get_total_play_time()) / 1000.f;
		}

		// this is counter-intuitive, but freezing sprite animation seems more easy to catch a bug.
		return true;
	}

	template <typename StateEnum>
	void stateController<StateEnum>::reset_anim_counter()
	{
		m_anim_elapsed = 0.0f;
	}

	template <typename StateEnum>
	StateEnum stateController<StateEnum>::get_state()
	{
		return m_state;
	}

	template <typename StateEnum>
	void stateController<StateEnum>::initialize()
	{
		m_anim_elapsed = 0.0f;
	}

	template<typename StateEnum>
	inline void stateController<StateEnum>::prerender()
	{
		m_anim_elapsed += DeltaTime::get_deltaTime();
	}

	template <typename StateEnum>
	void stateController<StateEnum>::set_state(const StateEnum& state)
	{
		m_state = state;
	}
}
#endif // STATECONTROLLER_HPP
