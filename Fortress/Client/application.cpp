#include "application.h"

void Fortress::Application::initialize(const HWND hwnd, const HDC hdc)
{
	m_hwnd = hwnd;
	m_hdc = hdc;
}

void Fortress::Application::run()
{
}

void Fortress::Application::update(const UINT message, const WPARAM wParam)
{
    switch (message) 
	{
	case WM_KEYDOWN:
		m_update_tick.set_ticked();

        switch (wParam)
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

    // @todo: test with static push
    m_render_queue.push([this]()
    {
        TextOut(m_hdc, m_playerPos.get_x(), m_playerPos.get_y(), L"@", 1);
        });

	if(m_update_tick.is_ticked())
	{
        InvalidateRect(m_hwnd, nullptr, true);
		m_update_tick.unset_ticked();
	}
}

void Fortress::Application::render()
{
    if(!m_render_tick.is_ticked())
    {
        m_render_tick.set_ticked();

	    while(!m_render_queue.empty())
	    {
		    m_render_queue.front()();
			m_render_queue.pop();
	    }

        m_render_tick.unset_ticked();
    }
}
