#pragma once

#include <Tether/Application.hpp>
#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Types.hpp>
#include <Tether/Devices/Monitor.hpp>
#include <Tether/Events/EventHandler.hpp>
#include <Tether/Events/EventType.hpp>
#include <Tether/Input/InputListener.hpp>

#include <atomic>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <string_view>
#include <vector>

namespace Tether
{
	class TETHER_EXPORT Window final
	{
	public:
		struct Impl;
		enum class Type
		{
			NORMAL
		};

		enum class CursorMode
		{
			NORMAL,
			HIDDEN,
			DISABLED,
		};

		Window(int width, int height, std::wstring_view title,
			bool visible = false);
		~Window();
		TETHER_NO_COPY(Window);

		void AddEventHandler(Events::EventHandler& handler, 
			Events::EventType eventType);
		void RemoveEventHandler(const Events::EventHandler& handler);
		void AddInputListener(Input::InputListener& listener, 
			Input::InputType inputType);
		void RemoveInputListener(const Input::InputListener& listener);

		void SetCloseRequested(bool requested);
		bool IsCloseRequested() const;

		void SpawnEvent(
			Events::EventType eventType,
			std::function<void(Events::EventHandler&)> callEventFun
		);

		void SpawnInput(
			Input::InputType inputType,
			std::function<void(Input::InputListener&)> callInputFun
		);

		void SpawnKeyInput(uint32_t scancode, uint32_t keycode, bool pressed);

		void SetVisible(bool visibility) const;
		void SetRawInputEnabled(bool enabled) const;
		void SetCursorMode(CursorMode mode);
		void SetCursorPos(int x, int y) const;
		void SetCursorRootPos(int x, int y);
		void SetX(int x);
		void SetY(int y);
		void SetPosition(int x, int y);
		void SetWidth(int width);
		void SetHeight(int height);
		void SetSize(int width, int height);
		void SetTitle(std::wstring_view title) const;
		void SetBoundsEnabled(bool enabled) const;
		void SetBounds(int minWidth, int minHeight, int maxWidth, int maxHeight);
		void SetDecorated(bool enabled) const;
		void SetResizable(bool resizable) const;
		void SetClosable(bool closable) const;
		void SetButtonStyleBitmask(uint8_t mask) const;
		void SetMaximized(bool maximize) const;
		void SetPreferredResizeInc(int x, int y);
		void EnableFullscreen(const Devices::Monitor& monitor) const;
		void DisableFullscreen() const;
		bool IsFocused() const;
		bool IsVisible() const;

		int GetY() const;
		int GetX() const;
		int GetWidth() const;
		int GetHeight() const;
		int GetMouseX() const;
		int GetMouseY() const;
		int GetRelativeMouseX() const;
		int GetRelativeMouseY() const;

		/**
		 *
		 * @return The HWND on Windows, and an Xorg Window on Linux.
		 */
		size_t GetHandle() const;
		Impl* GetImpl() const;
	private:
		Application& m_App;

		int m_X = 0;
		int m_Y = 0;
		int m_Width = 0;
		int m_Height = 0;

		int m_MouseX = -1;
		int m_MouseY = -1;
		int m_RelMouseX = -1;
		int m_RelMouseY = -1;

		std::unique_ptr<Impl> m_impl;

		std::atomic_bool m_CloseRequested = false;

		// Each event has a list of handlers to handle that specific event.
		std::shared_mutex m_HandlersMutex;
		std::unordered_map<Events::EventType, 
			std::vector<Events::EventHandler*>> m_Handlers;

		std::shared_mutex m_InputListenersMutex;
		std::unordered_map<Input::InputType, 
			std::vector<Input::InputListener*>> m_InputListeners;
	};
}
