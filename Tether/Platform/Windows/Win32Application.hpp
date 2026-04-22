#pragma once

#include <Tether/Application.hpp>

#include "Win32Window.hpp"

namespace Tether
{
	struct TETHER_EXPORT Application::Impl
	{
		HINSTANCE hinstance = nullptr;
		Window::Impl* m_HiddenCursorWindow = nullptr;
	};
}
