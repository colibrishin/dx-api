#include <random>

#include "application.h"
#include <filesystem>

#include "BulletinBoardScene.h"
#include "LoadingScene.hpp"
#include "LobbyScene.h"
#include "RoomScene.h"
#include "titleScene.h"
#include "../Common/input.hpp"
#include "../Common/deltatime.hpp"
#include "../Common/BattleScene.h"
#include "../Common/debug.hpp"
#include "../Common/sceneManager.hpp"
#include "../Common/scene.hpp"
#include "../Common/SoundManager.hpp"
#include "winapihandles.hpp"

namespace Fortress
{
	void Application::initialize(const HWND hwnd, const HDC hdc)
	{
		m_hwnd = hwnd;
		m_hdc = hdc;
		Input::initialize();
		DeltaTime::initialize();

		EngineHandle::set_handle(std::make_shared<WinAPIHandles>());
		EngineHandle::get_handle().lock()->initialize(hwnd, hdc);
		m_buffer_hdc = EngineHandle::get_handle().lock()->get_buffer_dc();

		SoundManager::initialize();
		Debug::initialize(m_buffer_hdc);
		Scene::SceneManager::initialize();
		Scene::SceneManager::CreateScene<Scene::TitleScene>();
		Scene::SceneManager::CreateScene<Scene::RoomScene>();
		Scene::SceneManager::CreateScene<Scene::LobbyScene>();
		Scene::SceneManager::CreateScene<Scene::BulletinBoardScene>();
		Scene::SceneManager::SetActive(L"Title Scene");

		m_messenger.send_alive();
 	}

	void Application::update()
	{
		if (!m_hdc || !m_hwnd || !m_buffer_hdc)
		{
			return;
		}

		/*Debug::draw_line(
			{static_cast<float>(WinAPIHandles::get_window_width() / 2), 0}, 
			{static_cast<float>(WinAPIHandles::get_window_width() / 2), static_cast<float>(WinAPIHandles::get_actual_max_y())});

		Debug::draw_line(
			{static_cast<float>(0), static_cast<float>(WinAPIHandles::get_actual_max_y() / 2)}, 
			{static_cast<float>(WinAPIHandles::get_window_width()), static_cast<float>(WinAPIHandles::get_actual_max_y() / 2)})*/;

		DeltaTime::update();
		Input::update();
		TimerManager::update();
		Scene::SceneManager::update();
	}

	void Application::render()
	{
		Scene::SceneManager::render();
		Debug::render();
		DeltaTime::render();

		const auto handle = EngineHandle::get_handle().lock();

		BitBlt(
			m_hdc,
			0, 
			0, 
			handle->get_window_width(), 
			handle->get_window_height(), 
			m_buffer_hdc, 
			0, 
			0,
			SRCCOPY);
	}

	void Application::cleanup()
	{
		Scene::SceneManager::cleanup();
		CameraManager::cleanup();
		ObjectBase::ObjectManager::cleanup();
		Resource::ResourceManager::cleanup();
		SoundManager::cleanup();
		EngineHandle::get_handle().lock().reset();
	}
}
