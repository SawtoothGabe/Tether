#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Events/WindowMoveEvent.hpp>
#include <Tether/Events/WindowResizeEvent.hpp>

namespace Tether::Events
{
	class TETHER_EXPORT EventHandler
	{
	public:
		virtual ~EventHandler() = default;

		virtual void OnWindowClosing() {}
		virtual void OnWindowRepaint() {}
		virtual void OnWindowResize(const WindowResizeEvent& event) {}
		virtual void OnWindowMove(const WindowMoveEvent& event) {}
	};
}
