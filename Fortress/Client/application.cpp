#include "application.h"

Fortress::Application::~Application()
{
}

void Fortress::Application::initialize(const HWND hwnd, const HDC hdc)
{
	m_hwnd = hwnd;
	m_hdc = hdc;
}

void Fortress::Application::run(const UINT message, const WPARAM wparam)
{
	switch (message) 
	{
	case WM_KEYDOWN:
        switch (wparam)
        {
        case VK_LEFT:
            m_playerPos = m_playerPos.left();
            break;
        case VK_RIGHT:
            m_playerPos = m_playerPos.right();
            break;
        case VK_UP:
            m_playerPos = m_playerPos.bottom();
            break;
        case VK_DOWN:
            m_playerPos = m_playerPos.top();
            break;
        }
	}
}

void Fortress::Application::update(bool erase) const
{
	InvalidateRect(m_hwnd, nullptr, erase);
}

void Fortress::Application::render()
{
    TextOut(m_hdc, static_cast<int>(m_playerPos.get_x()), static_cast<int>(m_playerPos.get_y()), L"A", 1);
}
