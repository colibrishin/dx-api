#include "characterScene.h"

#include "application.h"
#include "input.hpp"
#include "winapihandles.hpp"

void Scene::CharacterScene::initialize()
{
	_scene::initialize();
	m_object = {
		L"Ball",
		{1.0f, 1.0f},
		{0.0f, 0.0f},
		{20.0f, 20.0f},
		500.0f,
		0.0f,
		100,
		100,
		Object::CharacterType::CANNON};

	m_object.m_bActive = false;
	m_ground.m_bActive = false;
}

void Scene::CharacterScene::update()
{
	_scene::update();

	if(Fortress::Input::getKey(Fortress::eKeyCode::W))
	{
		m_object.move_up();
	}
	if(Fortress::Input::getKey(Fortress::eKeyCode::A))
	{
		m_object.move_left();
	}
	if(Fortress::Input::getKey(Fortress::eKeyCode::S))
	{
		m_object.move_down();
	}
	if(Fortress::Input::getKey(Fortress::eKeyCode::D))
	{
		m_object.move_right();
	}

	if(Fortress::Input::getKeyDown(Fortress::eKeyCode::SPACE))
	{
		m_object.shoot();
	}

	if(Fortress::Input::getKeyUp(Fortress::eKeyCode::W) ||
		Fortress::Input::getKeyUp(Fortress::eKeyCode::A) ||
		Fortress::Input::getKeyUp(Fortress::eKeyCode::S) ||
		Fortress::Input::getKeyUp(Fortress::eKeyCode::D))
	{
		m_object.stop();
	}

	Object::Character::block_window_frame(m_object);
}

void Scene::CharacterScene::render()
{
	// @todo: Text and bar can be made in class
	// HP bar
	m_render_queue.push(0, [this]() 
	{
		const int x = 30;
		const int y = 500;

		// hp text
		wchar_t hp_notice[100] = {};
		swprintf_s(hp_notice, 100,  L"HP : ");
		const size_t strlen = wcsnlen_s(hp_notice, 100);
		TextOut(m_hdc, x, y + 5, hp_notice, strlen);

		// bar outline
		Rectangle(
			m_hdc,
			x + 20,
			y,
			x + 20 + 250,
			y + 25);

		// bar inside
		const HBRUSH brush = CreateSolidBrush(BLACK_BRUSH);
		const RECT rect = {x + 20, y, static_cast<int>(x + 20.0f + m_object.get_hp_percentage() * 250.0f), y + 25};
		FillRect(WinAPIHandles::get_buffer_dc(), &rect, brush);
		DeleteObject(brush);
	});

	// MP bar
	m_render_queue.push(0, [this]() 
	{
		const int x = 30;
		const int y = 530;

		// hp text
		wchar_t hp_notice[100] = {};
		swprintf_s(hp_notice, 100,  L"MP : ");
		const size_t strlen = wcsnlen_s(hp_notice, 100);
		TextOut(m_hdc, x, y + 5, hp_notice, strlen);

		// bar outline
		Rectangle(
			m_hdc,
			x + 20,
			y,
			x + 20 + 250,
			y + 25);

		// bar inside
		const HBRUSH brush = CreateSolidBrush(BLACK_BRUSH);
		const RECT rect = {x + 20, y, static_cast<int>(x + 20 + m_object.get_mp_percentage() * 250), y + 25};
		FillRect(WinAPIHandles::get_buffer_dc(), &rect, brush);
		DeleteObject(brush);
	});

	// Ground
	m_render_queue.push(0, [this]() 
	{
		m_ground.render();
	});

	// Player
	m_render_queue.push(0, [this]()
	{
		// @todo: workaround version of passing this pointer
		m_object.render();
	});

	m_render_queue.push(1, [this]()
	{
		wchar_t notice[100] = {};
		swprintf_s(notice, 100,  L"Use WASD to move...");
		const size_t strlen = wcsnlen_s(notice, 100);
		TextOut(m_hdc, 300, 300, notice, strlen);
	});

	_scene::render();
}

void Scene::CharacterScene::deactivate()
{
	_scene::deactivate();

	m_object.m_bActive = false;
	m_ground.m_bActive = false;
}

void Scene::CharacterScene::activate()
{
	m_object.m_bActive = true;
	m_ground.m_bActive = true;
}

