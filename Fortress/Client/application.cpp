#include <random>

#include "application.h"

#include "BattleScene.h"
#include "BulletinBoardScene.h"
#include "input.hpp"
#include "deltatime.hpp"
#include "LobbyScene.h"
#include "RoomScene.h"
#include "sceneManager.hpp"
#include "scene.hpp"
#include "titleScene.h"
#include "SoundManager.hpp"
#include "winapihandles.hpp"

namespace Fortress
{
	void Application::initialize(const HWND hwnd, const HDC hdc)
	{
		m_hwnd = hwnd;
		m_hdc = hdc;
		Input::initialize();
		DeltaTime::initialize();

		WinAPIHandles::initialize(hwnd, hdc);
		SoundManager::initialize();
		m_buffer_hdc = WinAPIHandles::get_buffer_dc();
		Debug::initialize(m_buffer_hdc);

		// lazy-initialization font collection, private font constructor initialized before gdi initialization.
		m_font_collection = std::make_unique<PrivateFontCollection>();

		Scene::SceneManager::initialize();
		Scene::SceneManager::CreateScene<Scene::TitleScene>();
		Scene::SceneManager::CreateScene<Scene::BulletinBoardScene>();
		Scene::SceneManager::CreateScene<Scene::LobbyScene>();
		Scene::SceneManager::CreateScene<Scene::RoomScene>();
		Scene::SceneManager::SetActive(L"Title Scene");

		const std::filesystem::path font_path = "./resources/font/ark-pixel-10px-monospaced-ko.ttf";

		if(m_font_collection->AddFontFile(font_path.native().c_str()) != Ok)
		{
			throw std::exception("Unable to load font file.");
		}

		m_font = std::make_unique<Font>(
					L"Ark Pixel 10px Monospaced ko",
					50,
					FontStyleRegular,
					UnitPixel,
					m_font_collection.get());
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
		Scene::SceneManager::update();
	}

	void Application::render()
	{
		Scene::SceneManager::render();
		Debug::render();
		DeltaTime::render();

		BitBlt(m_hdc, 0, 0, WinAPIHandles::get_window_width(), WinAPIHandles::get_window_height(), m_buffer_hdc, 0, 0,
		       SRCCOPY);
	}

	void Application::cleanup()
	{
		Scene::SceneManager::cleanup();
		CameraManager::cleanup();
		ObjectBase::ObjectManager::cleanup();
		Resource::ResourceManager::cleanup();
		SoundManager::cleanup();
		WinAPIHandles::cleanup();
	}

	std::weak_ptr<Font> Application::get_font()
	{
		return m_font;
	}
}
