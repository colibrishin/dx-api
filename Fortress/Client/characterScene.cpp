#include "characterScene.h"

#include "application.h"
#include "input.hpp"

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
		0,
		0,
		Object::CharacterType::CANNON};
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

	Fortress::Application::checkWindowFrame(m_object);
}

void Scene::CharacterScene::render()
{
	_scene::render();

	Ellipse(
			m_hdc,
			m_object.get_x(), 
			m_object.get_y(), 
			m_object.get_x() + m_object.m_hitbox.get_x(),
			m_object.get_y() + m_object.m_hitbox.get_y());

	wchar_t notice[100] = {};
	swprintf_s(notice, 100,  L"Use WASD to move...");
	const size_t strlen = wcsnlen_s(notice, 100);
	TextOut(m_hdc, 300, 300, notice, strlen);
}
