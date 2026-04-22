#include <Tether/Application.hpp>

namespace Tether
{
	const int16_t* Application::GetKeycodes() const
	{
		return m_Keycodes;
	}

	const int16_t* Application::GetScancodes() const
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

	Application::Impl* Application::GetImpl() const
	{
		return m_impl.get();
	}

	Application& Application::Get()
	{
		static Application instance;
		return instance;
	}
}
