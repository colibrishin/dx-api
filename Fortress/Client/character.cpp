#include "character.hpp"

#include "deltatime.hpp"
#include "ground.hpp"
#include "projectile.hpp"

namespace Fortress::ObjectBase
{
	void character::initialize()
	{
		m_hitbox = m_texture.get_image(L"idle", L"left")->get_hitbox();
		set_current_sprite(L"idle", m_offset == Math::left ? L"left" : L"right");
		rigidBody::initialize();
	}

	void character::hit(const projectile* p)
	{
		m_hp -= p->get_damage();
	}

	void character::shoot()
	{
		set_current_sprite(L"fire", m_offset == Math::left ? L"left" : L"right");

		// @todo: maybe queue?
		m_current_sprite->play([this]()
		{
			set_current_sprite(L"idle", m_offset == Math::left ? L"left" : L"right");
		});
		m_power = 1.0f;
	}

	float character::get_charged_power() const
	{
		return m_power;
	}

	float character::get_hp_percentage() const
	{
		return m_hp / static_cast<float>(character_full_hp);
	}

	float character::get_mp_percentage() const
	{
		return m_mp / static_cast<float>(character_full_mp);
	}

	character::~character()
	{
		rigidBody::~rigidBody();
	}

	void character::update()
	{
		rigidBody::update();
	}

	void character::render()
	{
		Math::Vector2 pos{};

		if (is_active())
		{
			if(Scene::SceneManager::get_active_scene()->get_camera()->get_locked_object() == std::dynamic_pointer_cast<object>(shared_from_this()))
			{
				pos = Scene::SceneManager::get_active_scene()->get_camera()->get_offset();
			}
			else
			{
				pos = Scene::SceneManager::get_active_scene()->get_camera()->get_relative_position(std::dynamic_pointer_cast<object>(shared_from_this()));
			}

			m_current_sprite->render(pos, m_hitbox);

			Debug::Log(L"Char pos" + std::to_wstring(pos.get_x()) + std::to_wstring(pos.get_y()));
			Debug::draw_rect(pos, m_hitbox);
			Debug::draw_dot(pos);

			Debug::Log(L"Angle : " +  std::to_wstring(Scene::SceneManager::get_active_scene()->get_camera()->get_offset().local_inner_angle(pos)));

			// c
			Debug::draw_line(pos, Scene::SceneManager::get_active_scene()->get_camera()->get_offset());

			// t
			Debug::draw_line(
				Scene::SceneManager::get_active_scene()->get_camera()->get_offset(), 
				{Scene::SceneManager::get_active_scene()->get_camera()->get_offset().get_x(), pos.get_y()});

			// s
			Debug::draw_line(
				{Scene::SceneManager::get_active_scene()->get_camera()->get_offset().get_x(), pos.get_y()}, pos);
		}

		rigidBody::render();
	}

	void character::firing()
	{
		if(get_current_sprite_name().find(L"charging") == std::wstring::npos)
		{
			set_current_sprite(L"charging", m_offset == Math::left ? L"left" : L"right");
		}

		// @todo: somewhat like quadratic function
		static float interval = 0.0f;

		if(interval >= 0.2f)
		{
			m_power = m_power * m_power + 0.0001f;
			interval = 0.0f;
		}

		interval += DeltaTime::get_deltaTime();

		Debug::Log(L"Power : " + std::to_wstring(m_power));
	}

	void character::move()
	{
		if (m_mp < Math::epsilon)
		{
			return;
		}

		if (std::fabs(m_velocity.get_x()) > Math::epsilon)
		{
			m_mp -= 20.0f * DeltaTime::get_deltaTime();
		}

		rigidBody::move();
	}

	void character::on_collision(const std::shared_ptr<Abstract::rigidBody>& other)
	{
		if(const auto ground = std::dynamic_pointer_cast<Object::Ground>(other))
		{
			if(ground)
			{
				Debug::Log(L"Character hits the ground");
				m_gravity_speed = 0.0f;
				m_bGrounded = true;
				return;
			}
		}

		rigidBody::on_collision(other);
	}

	void character::set_current_sprite(const std::wstring& name, const std::wstring& orientation)
	{
		m_current_sprite = m_texture.get_image(name, orientation);
		m_current_sprite->play();
	}

	void character::set_sprite_offset(const std::wstring& name, const std::wstring& orientation,
		const Math::Vector2& offset)
	{
		m_texture.get_image(name, orientation)->set_offset(offset);
	}

	const std::wstring& character::get_current_sprite_name() const
	{
		return m_current_sprite->get_name();
	}

	Math::Vector2 character::get_offset() const
	{
		return m_offset;
	}

	void character::move_left()
	{
		m_offset = Math::left;
		set_current_sprite(L"move", L"left");
		rigidBody::move_left();
	}

	void character::move_right()
	{
		m_offset = Math::right;
		set_current_sprite(L"move", L"right");
		rigidBody::move_right();
	}

	void character::stop()
	{
		set_current_sprite(L"idle", m_offset == Math::left ? L"left" : L"right");
		rigidBody::stop();
	}

	character::character(const character& other) :
		rigidBody(other), m_hp(other.m_hp), m_mp(other.m_mp),
		m_power(other.m_power), m_bGrounded(other.m_bGrounded),
		m_offset(other.m_offset), m_texture(other.m_texture.get_name()), m_current_sprite(other.m_current_sprite)
	{
		character::initialize();
	}
}
