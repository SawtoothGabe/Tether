#include <iostream>

#include <Tether/Tether.hpp>

#include <thread>
#include <chrono>
#include <math.h>

using namespace std::literals::chrono_literals;
using namespace Tether;

class FullscreenWindow
{
public:
	class Handler : public Events::EventHandler
	{
	public:
		void OnWindowClosing() override
		{
			Application::Get().Stop();
		}
	};

	FullscreenWindow()
		:
		m_Window(Window::Create(1280, 720, L"Fullscreen"))
	{
		m_Window->SetRawInputEnabled(true);

		m_Window->SetX(120);
		m_Window->SetY(120);

		m_Window->SetVisible(true);

		const Devices::Monitor monitor = Application::Get().GetMonitors()[0];
		m_Window->EnableFullscreen(monitor);

		m_Window->AddEventHandler(handler, Events::EventType::WINDOW_CLOSING);
	}
private:
	Handler handler;

	Scope<Window> m_Window;
};

int main()
{
	FullscreenWindow window;
	Application::Get().Run();

	return 0;
}