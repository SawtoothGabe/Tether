#pragma once

#include <Tether/Window.hpp>

#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h> // oh, no

namespace Tether
{
	struct TETHER_EXPORT Window::Impl
	{
		explicit Impl(Window& window);

		LRESULT HandleMessage(HWND hWnd, DWORD msg, WPARAM wParam, LPARAM lParam);

		void SpawnMouseClick(
			LPARAM lParam,
			Input::MouseClickInfo::ClickType type,
			bool pressed
		) const;

		bool SpawnXbuttonClick(
			LPARAM lParam,
			WPARAM wParam,
			bool pressed
		);

		void GenerateClassName();

		void ReconstructStyle() const;
		[[nodiscard]] RECT GetAdjustedRect(int x, int y, int width, int height) const;
		[[nodiscard]] LONG CalculateStyle() const;
		[[nodiscard]] LONG CalculateExtendedStyle() const;

		Window& m_window;

		HWND m_Hwnd = nullptr;
		HINSTANCE m_Hinst = nullptr;
	
		std::wstring m_ClassName;

		bool m_Decorated = true;
		bool m_Visible = false;
		bool m_Resizable = true;
		bool m_Closable = true;
		bool m_Fullscreen = false;

		bool m_RawInputEnabled = false;
		bool m_RawInputInitialized = false;
		CursorMode m_CursorMode = CursorMode::NORMAL;

		bool m_BoundsEnabled = false;
		int m_MinWidth = INT_MIN; 
		int m_MinHeight = INT_MIN;
		int m_MaxWidth = INT_MAX;
		int m_MaxHeight = INT_MAX;

		bool m_PrevReceivedMouseMove = false;
		
		uint8_t m_StyleMask = ButtonStyleMask::MAXIMIZE_BUTTON
			              | ButtonStyleMask::MINIMIZE_BUTTON;

		WNDCLASSEXW m_WndClass{};
	};
}
