#include <Tether/Application.hpp>

#include <stdexcept>
#include <cstring>

#include <Tether/Platform/PlatformDefs.hpp>

#ifndef TETHER_HEADLESS
#if defined(TETHER_PLATFORM_WINDOWS)
#include <Tether/Platform/Win32Application.hpp>
#elif defined(TETHER_PLATFORM_LINUX)
#include <Tether/Platform/X11Application.hpp>
#endif
#endif

namespace Tether
{
	Application::Application()
	{
		memset(m_Keycodes, -1, sizeof(m_Keycodes));
		memset(m_Scancodes, -1, sizeof(m_Scancodes));
	}

	Application::~Application() 
	{}

	const int16_t* const Application::GetKeycodes() const
	{
		return m_Keycodes;
	}

	const int16_t* const Application::GetScancodes() const
	{
		return m_Scancodes;
	}

	bool Application::IsRunning()
	{
		return m_IsRunning;
	}

	void Application::Stop()
	{
		m_IsRunning = false;
	}

	Application& Application::Get()
	{
		if (!internal.get())
		{
#ifndef TETHER_HEADLESS
#if defined(TETHER_PLATFORM_WINDOWS)
			internal = std::make_unique<Platform::Win32Application>();
#elif defined(TETHER_PLATFORM_LINUX)
			internal = std::make_unique<Platform::X11Application>();
#endif
#else
		    throw std::logic_error("Tether was compiled in headless mode");
#endif

			internal->CreateKeyLUTs(internal->m_Keycodes, internal->m_Scancodes);
		}

		return *internal;
	}
}
