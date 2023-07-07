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
	m_render_queue.emplace(0, [this]() 
	{
		Rectangle(
			m_hdc,
			m_ground.get_x(), 
			m_ground.get_y(), 
			m_ground.get_x() + m_ground.m_hitbox.get_x(),
			m_ground.get_y() + m_ground.m_hitbox.get_y());
	});

	m_render_queue.emplace(0, [this]() 
	{
		Ellipse(
			m_hdc,
			m_object.get_x(), 
			m_object.get_y(), 
			m_object.get_x() + m_object.m_hitbox.get_x(),
			m_object.get_y() + m_object.m_hitbox.get_y());
	});

	m_render_queue.emplace(1, [this]()
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

