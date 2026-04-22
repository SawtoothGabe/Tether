#include <iostream>

#include <Tether/Tether.hpp>

#include <thread>
#include <chrono>
#include <deque>
#include <math.h>

using namespace std::literals::chrono_literals;
using namespace Tether;

static constexpr const size_t WINDOW_COUNT = 3;
static bool s_WindowsClosed[WINDOW_COUNT]{};

class TestWindow
{
public:
	class EventHandler : public Events::EventHandler
	{
	public:
		EventHandler(Window& window, size_t windowIndex)
			:
			m_Window(window),
			m_WindowIndex(windowIndex)
		{
			if (windowIndex >= WINDOW_COUNT)
				throw std::invalid_argument("Window index larger than window count");
		}

		void OnWindowClosing() override;
		
		Window& m_Window;
		size_t m_WindowIndex = 0;
	};

	TestWindow(const Devices::Monitor& monitor, size_t windowIndex)
		:
		m_Window(700, 700, std::to_wstring(windowIndex + 1)),
		handler(m_Window, windowIndex)
	{
		int usableWidth = monitor.GetWidth() - m_Window.GetWidth();
		int usableHeight = monitor.GetHeight() - m_Window.GetHeight();

		m_Window.AddEventHandler(handler, Events::EventType::WINDOW_CLOSING);

		m_Window.SetResizable(false);
		m_Window.SetRawInputEnabled(true);

		m_Window.SetX(120);
		m_Window.SetY(120);

		m_Window.SetVisible(true);

		m_Window.SetX(rand() % std::max(0, usableWidth));
		m_Window.SetY(rand() % std::max(0, usableHeight));
	}

	bool IsCloseRequested()
	{
		return m_Window.IsCloseRequested();
	}
private:
	Window m_Window;

	EventHandler handler;
};

void TestWindow::EventHandler::OnWindowClosing()
{
	m_Window.SetVisible(false);
	s_WindowsClosed[m_WindowIndex] = true;

	for (size_t i = 0; i < WINDOW_COUNT; i++)
		if (!s_WindowsClosed[i])
			return;

	Application::Get().Stop();
}

#include <iostream>
#include <vector>

#if defined(_WIN32) && !defined(_DEBUG)
#include <Windows.h>
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
#else
int main()
#endif
{
	std::vector<Devices::Monitor> monitors = Application::Get().GetMonitors();
	Devices::Monitor monitor = monitors[0];

	std::deque<TestWindow> windows;

	for (size_t i = 0; i < WINDOW_COUNT; i++)
	{
		windows.emplace_back(monitor, i);
		std::this_thread::sleep_for(100ms);
	}

	Application::Get().Run();

	return 0;
}