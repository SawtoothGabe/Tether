#include "Win32Window.hpp"

#include "Win32Application.hpp"

#include <Tether/Common/StringTools.hpp>

#include <algorithm>
#include <random>

#include <WindowsX.h>

#undef ERROR

namespace Tether
{
    class HandleMessageCaller
    {
    public:
        explicit HandleMessageCaller(Window::Impl& window)
            :
            m_impl(window)
        {}

        LRESULT HandleMessage(const HWND hwnd, const DWORD msg, const WPARAM wparam, const LPARAM lparam) const
        {
            return m_impl.HandleMessage(hwnd, msg, wparam, lparam);
        }
    private:
        Window::Impl& m_impl;
    };

    static LRESULT CALLBACK WndProcRedir(const HWND hWnd, const UINT msg, const WPARAM wParam,
        const LPARAM lParam)
    {
        const HandleMessageCaller caller(*reinterpret_cast<Window*>(GetWindowLongPtr(hWnd,
            GWLP_USERDATA))->GetImpl());
        return caller.HandleMessage(hWnd, msg, wParam, lParam);
    }

    static LRESULT CALLBACK WindowProc(const HWND hWnd, const UINT msg, const WPARAM wParam,
        const LPARAM lParam)
    {
        // This hwnd procedure is only to initialize the hwnd user data before
        // using the actual redirection hwnd procedure.

        if (msg == WM_NCCREATE)
        {
            const auto pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            auto window = static_cast<Window*>(pCreate->lpCreateParams);

            // Sanity check
            if (window == nullptr)
                return 0;

            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
            // Switch the hwnd procedure
            SetWindowLongPtr(hWnd, GWLP_WNDPROC,
                reinterpret_cast<LONG_PTR>(WndProcRedir));
        }

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    Window::Impl::Impl(Window& window)
        :
        m_window(window)
    {}

    Window::Window(const int width, const int height, const std::wstring_view title,
        const bool visible)
        :
        m_App(Application::Get())
    {
        m_impl = std::make_unique<Impl>(*this);

        // HInstance doesn't need to be from the WinMain entrypoint.
        m_impl->m_Hinst = GetModuleHandle(nullptr);

        m_Width = width;
        m_Height = height;

        const RECT wr = m_impl->GetAdjustedRect(0, 0, width, height);

        m_impl->GenerateClassName();

        // Create class
        m_impl->m_WndClass.cbSize = sizeof(WNDCLASSEX);
        m_impl->m_WndClass.style = CS_HREDRAW | CS_VREDRAW;
        m_impl->m_WndClass.cbClsExtra = 0;
        m_impl->m_WndClass.cbWndExtra = 0;
        m_impl->m_WndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        m_impl->m_WndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
        m_impl->m_WndClass.hIcon = LoadIcon(m_impl->m_Hinst, IDI_APPLICATION);
        m_impl->m_WndClass.hIconSm = LoadIcon(m_impl->m_Hinst, IDI_APPLICATION);
        m_impl->m_WndClass.lpszMenuName = nullptr;
        m_impl->m_WndClass.lpszClassName = m_impl->m_ClassName.c_str();
        m_impl->m_WndClass.hInstance = m_impl->m_Hinst;
        m_impl->m_WndClass.lpfnWndProc = WindowProc;

        RegisterClassEx(&m_impl->m_WndClass);

        // Create hwnd
        m_impl->m_Hwnd = CreateWindowEx(
            0, // Extended style
            m_impl->m_ClassName.c_str(),
            title.data(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            wr.right - wr.left, // Width
            wr.bottom - wr.top, // Height
            nullptr, //Handle to the parent of this hwnd
            nullptr, //Handle to the menu of child hwnd identifier
            m_impl->m_Hinst,
            this
        );

        if (visible)
            SetVisible(true);

        RECT clientRect{};
        GetClientRect(m_impl->m_Hwnd, &clientRect);

        m_X = clientRect.left;
        m_Y = clientRect.top;
        m_Width = width;
        m_Height = height;
    }

    Window::~Window()
    {
        DestroyWindow(m_impl->m_Hwnd);
        UnregisterClass(m_impl->m_ClassName.c_str(), m_impl->m_Hinst);
    }

    void Window::SetVisible(const bool visibility) const
    {
        if (visibility)
        {
            ShowWindow(m_impl->m_Hwnd, SW_SHOW);
            m_impl->ReconstructStyle();
        } else
            ShowWindow(m_impl->m_Hwnd, SW_HIDE);

        m_impl->m_Visible = visibility;
    }

    bool Window::IsVisible() const
    {
        return m_impl->m_Visible;
    }

    void Window::SetRawInputEnabled(const bool enabled) const
    {
        if (enabled && !m_impl->m_RawInputInitialized)
        {
            RAWINPUTDEVICE rawInputDevice;
            rawInputDevice.usUsagePage = 0x01; // Mouse
            rawInputDevice.usUsage = 0x02;
            rawInputDevice.dwFlags = 0;
            rawInputDevice.hwndTarget = m_impl->m_Hwnd;

            if (!RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice)))
                return;

            m_impl->m_RawInputInitialized = true;
        }

        m_impl->m_RawInputEnabled = enabled;
    }

    void Window::SetCursorMode(const CursorMode mode)
    {
        auto& app = reinterpret_cast<Application::Impl&>(m_App);

        if (m_impl->m_CursorMode == CursorMode::DISABLED && mode != CursorMode::DISABLED)
            app.m_HiddenCursorWindow = nullptr;

        switch (mode)
        {
            case CursorMode::NORMAL:
            {
                ShowCursor(true);
            }
            break;

            case CursorMode::HIDDEN:
            {
                ShowCursor(false);
            }
            break;

            case CursorMode::DISABLED:
            {
                if (app.m_HiddenCursorWindow)
                    app.m_HiddenCursorWindow->m_window.SetCursorMode(CursorMode::NORMAL);
                app.m_HiddenCursorWindow = m_impl.get();

                ShowCursor(false);
            }
            break;
        }

        m_impl->m_CursorMode = mode;
    }

    static void SetCurPos(int x, int y)
    {
        SetCursorPos(x, y);
    }

    void Window::SetCursorPos(int x, int y) const
    {
        POINT pt;
        pt.x = x;
        pt.y = y;

        ClientToScreen(m_impl->m_Hwnd, &pt);
        SetCurPos(pt.x, pt.y);
    }

    void Window::SetCursorRootPos(int x, int y)
    {
        SetCurPos(x, y);
    }

    void Window::SetX(int x)
    {
        RECT wr;
        GetWindowRect(m_impl->m_Hwnd, &wr);

        MoveWindow(
            m_impl->m_Hwnd,
            static_cast<int>(x),
            wr.top,
            wr.right - wr.left,
            wr.bottom - wr.top,
            false
        );

        m_X = x;
    }

    void Window::SetY(int y)
    {
        RECT wr;
        GetWindowRect(m_impl->m_Hwnd, &wr);

        MoveWindow(
            m_impl->m_Hwnd,
            wr.left,
            static_cast<int>(y),
            wr.right - wr.left,
            wr.bottom - wr.top,
            false
        );

        m_Y = y;
    }

    void Window::SetPosition(int x, int y)
    {
        RECT wr;
        GetWindowRect(m_impl->m_Hwnd, &wr);

        MoveWindow(
            m_impl->m_Hwnd,
            static_cast<int>(x),
            static_cast<int>(y),
            wr.right - wr.left,
            wr.bottom - wr.top,
            false
        );

        m_X = x;
        m_Y = y;
    }

    void Window::SetWidth(int width)
    {
        RECT wr;
        GetWindowRect(m_impl->m_Hwnd, &wr);

        MoveWindow(
            m_impl->m_Hwnd,
            wr.left,
            wr.top,
            width,
            wr.bottom - wr.top,
            false
        );

        m_Width = width;
    }

    void Window::SetHeight(int height)
    {
        RECT wr;
        GetWindowRect(m_impl->m_Hwnd, &wr);

        MoveWindow(
            m_impl->m_Hwnd,
            wr.left,
            wr.top,
            wr.right - wr.left,
            height,
            false
        );

        m_Height = height;
    }

    void Window::SetSize(int width, int height)
    {
        RECT wr;
        GetWindowRect(m_impl->m_Hwnd, &wr);

        MoveWindow(
            m_impl->m_Hwnd,
            wr.left,
            wr.top,
            width,
            height,
            false
        );

        m_Width = width;
        m_Height = height;
    }

    void Window::SetTitle(const std::wstring_view title) const
    {
        SetWindowText(m_impl->m_Hwnd, title.data());
    }

    void Window::SetBoundsEnabled(const bool enabled) const
    {
        m_impl->m_BoundsEnabled = enabled;
    }

    void Window::SetBounds(int minWidth, int minHeight,
        int maxWidth, int maxHeight)
    {
        minWidth = minWidth;
        minHeight = minHeight;
        maxWidth = maxWidth;
        maxHeight = maxHeight;
    }

    void Window::SetDecorated(const bool decorated) const
    {
        m_impl->m_Decorated = decorated;
        m_impl->ReconstructStyle();
    }

    void Window::SetResizable(const bool resizable) const
    {
        m_impl->m_Resizable = resizable;
        m_impl->ReconstructStyle();
    }

    void Window::SetClosable(bool closable) const
    {
        m_impl->m_Closable = closable;
    }

    void Window::SetButtonStyleBitmask(uint8_t style) const
    {
        m_impl->m_StyleMask = style;
        m_impl->ReconstructStyle();
    }

    void Window::SetMaximized(const bool maximize) const
    {
        if (m_impl->m_Visible)
        {
            if (maximize)
                ShowWindow(m_impl->m_Hwnd, SW_MAXIMIZE);
            else
                ShowWindow(m_impl->m_Hwnd, SW_SHOW);
        }
    }

    void Window::SetPreferredResizeInc(int x, int y)
    {}

    void Window::EnableFullscreen(const Devices::Monitor& monitor) const
    {
        SetWindowLong(m_impl->m_Hwnd, GWL_STYLE, WS_POPUP);
        SetWindowPos(m_impl->m_Hwnd, HWND_TOP, monitor.GetX(), monitor.GetY(),
            monitor.GetWidth(), monitor.GetHeight(), SWP_SHOWWINDOW);
        ShowWindow(m_impl->m_Hwnd, SW_MAXIMIZE);
    }

    void Window::DisableFullscreen() const
    {
        DEVMODEW dmScreenSettings{};
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);

        ChangeDisplaySettings(&dmScreenSettings, CDS_RESET);

        m_impl->ReconstructStyle();
        SetWindowPos(m_impl->m_Hwnd, HWND_TOP,
            static_cast<int>(m_X),
            static_cast<int>(m_Y),
            static_cast<int>(m_Width),
            static_cast<int>(m_Height),
            SWP_SHOWWINDOW
        );
        ShowWindow(m_impl->m_Hwnd, SW_SHOW);
    }

    bool Window::IsFocused() const
    {
        return GetForegroundWindow() == m_impl->m_Hwnd;
    }

    LONG Window::Impl::CalculateStyle() const
    {
        LONG style = GetWindowLong(m_Hwnd, GWL_STYLE);

        if (!m_Decorated)
            style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
                       | WS_SYSMENU);
        else
        {
            style |= WS_CAPTION | WS_SYSMENU;

            if (m_Resizable)
                style |= WS_THICKFRAME;
            else
                style &= ~WS_THICKFRAME;

            if (m_StyleMask & ButtonStyleMask::MINIMIZE_BUTTON)
                style |= WS_MINIMIZEBOX;
            else
                style &= ~WS_MINIMIZEBOX;

            if (m_StyleMask & ButtonStyleMask::MAXIMIZE_BUTTON && m_Resizable)
                style |= WS_MAXIMIZEBOX;
            else
                style &= ~WS_MAXIMIZEBOX;
        }

        return style;
    }

    LONG Window::Impl::CalculateExtendedStyle() const
    {
        LONG exStyle = GetWindowLong(m_Hwnd, GWL_EXSTYLE);

        // Keep this. It might prove useful later.
        /*if (decorated)
            exStyle |= WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE;*/

        return exStyle;
    }

    void Window::Impl::ReconstructStyle() const
    {
        SetWindowLong(m_Hwnd, GWL_STYLE, CalculateStyle());
        SetWindowLong(m_Hwnd, GWL_EXSTYLE, CalculateExtendedStyle());
        SetWindowPos(m_Hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE
                                                  | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    }

    RECT Window::Impl::GetAdjustedRect(int x, int y, int width, int height) const
    {
        RECT wr;
        wr.left = static_cast<LONG>(x);
        wr.top = static_cast<LONG>(y);
        wr.right = wr.left + static_cast<LONG>(width);
        wr.bottom = wr.top + static_cast<LONG>(height);

        AdjustWindowRect(&wr, CalculateStyle(), false);

        return wr;
    }

    LRESULT Window::Impl::HandleMessage(const HWND hWnd, const DWORD msg, const WPARAM wParam,
        const LPARAM lParam)
    {
        using namespace Events;
        using namespace Input;

        // The hwnd variable should never be used in this function because
        // the CreateWindow function sends events before it has finished execution;
        // therefore, it is possible for the hwnd variable to be null.
        // Use hWnd instead. Why does WinAPI do this? I have no idea.

        using ClickType = MouseClickInfo::ClickType;

        const uint32_t wParam32 = static_cast<uint32_t>(wParam);

        switch (msg)
        {
            case WM_CLOSE:
            {
                if (!m_Closable)
                    break;

                m_window.SetCloseRequested(true);
                m_window.SpawnEvent(EventType::WINDOW_CLOSING,
                    [](EventHandler& eventHandler)
                    {
                        eventHandler.OnWindowClosing();
                    });
            }
            break;

            case WM_KEYDOWN:
            {
                if ((HIWORD(lParam) & KF_REPEAT) == KF_REPEAT)
                    return 0;

                UINT scancode = HIWORD(lParam) & (KF_EXTENDED | 0xFF);
                if (!scancode)
                    scancode = MapVirtualKey(wParam32, MAPVK_VK_TO_VSC);

                // Windows does this thing where you can't distinguish between either
                // shift keys, so instead, just press both and hope for the best.
                if (wParam == VK_SHIFT)
                {
                    m_window.SpawnKeyInput(scancode, Keycodes::KEY_LEFT_SHIFT, true);
                    m_window.SpawnKeyInput(scancode, Keycodes::KEY_RIGHT_SHIFT, true);

                    return 0;
                }

                Application& application = Application::Get();
                const int16_t* keycodes = application.GetKeycodes();

                m_window.SpawnKeyInput(scancode, keycodes[scancode], true);
            }
            break;

            case WM_KEYUP:
            {
                UINT scancode = MapVirtualKey(wParam32, MAPVK_VK_TO_VSC);

                if (wParam == VK_SHIFT)
                {
                    m_window.SpawnKeyInput(scancode, Keycodes::KEY_LEFT_SHIFT, false);
                    m_window.SpawnKeyInput(scancode, Keycodes::KEY_RIGHT_SHIFT, false);

                    return 0;
                }

                Application& application = Application::Get();
                const int16_t* keycodes = application.GetKeycodes();

                m_window.SpawnKeyInput(scancode, keycodes[scancode], false);
                return 0;
            }

            case WM_CHAR:
            {
                m_window.SpawnEvent(EventType::WINDOW_REPAINT,
                    [&](EventHandler& eventHandler)
                    {
                        eventHandler.OnWindowRepaint();
                    });

                KeyCharInfo event(
                    static_cast<char>(wParam),
                    (HIWORD(lParam) & KF_REPEAT) == KF_REPEAT
                );

                m_window.SpawnInput(InputType::KEY_CHAR,
                    [&](Input::InputListener& inputListener)
                    {
                        inputListener.OnKeyChar(event);
                    });
            }
            break;

            case WM_MOUSEMOVE:
            {
                int x = (int) (short) LOWORD(lParam);
                int y = (int) (short) HIWORD(lParam);

                POINT mouse;
                mouse.x = x;
                mouse.y = y;

                ClientToScreen(m_Hwnd, &mouse);

                if (x == m_window.m_RelMouseX && y == m_window.m_RelMouseY)
                    break;

                if (!m_PrevReceivedMouseMove)
                {
                    m_window.m_MouseX = mouse.x;
                    m_window.m_MouseY = mouse.y;
                    m_window.m_RelMouseX = x;
                    m_window.m_RelMouseY = y;
                }

                MouseMoveInfo event(
                    mouse.x,
                    mouse.y,
                    x,
                    y,
                    m_window.m_RelMouseX,
                    m_window.m_RelMouseY,
                    m_window.m_MouseX,
                    m_window.m_MouseY
                );

                m_window.SpawnInput(InputType::MOUSE_MOVE,
                    [&](InputListener& inputListener)
                    {
                        inputListener.OnMouseMove(event);
                    });

                if (m_CursorMode == CursorMode::DISABLED
                    && GetForegroundWindow() == m_Hwnd)
                {
                    m_window.SetCursorPos(
                        m_window.GetWidth() / 2,
                        m_window.GetHeight() / 2
                    );
                }

                m_window.m_MouseX = mouse.x;
                m_window.m_MouseY = mouse.y;
                m_window.m_RelMouseX = x;
                m_window.m_RelMouseY = y;

                m_PrevReceivedMouseMove = true;
            }
            break;

            case WM_MOVE:
            {
                m_window.m_X = LOWORD(lParam);
                m_window.m_Y = HIWORD(lParam);

                const WindowMoveEvent event(m_window.m_X, m_window.m_Y);
                m_window.SpawnEvent(EventType::WINDOW_MOVE,
                    [&](EventHandler& eventHandler)
                    {
                        eventHandler.OnWindowMove(event);
                    });
            }
            break;

            case WM_INPUT:
            {
                UINT dataSize = 0;
                GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL,
                    &dataSize, sizeof(RAWINPUTHEADER));

                if (dataSize > 0)
                {
                    std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
                    RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());

                    if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT,
                            rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize
                        && raw->header.dwType == RIM_TYPEMOUSE)
                    {
                        RawMouseMoveInfo event(
                            raw->data.mouse.lLastX,
                            raw->data.mouse.lLastY
                        );

                        m_window.SpawnInput(InputType::RAW_MOUSE_MOVE,
                        [&](InputListener& inputListener)
                        {
                            inputListener.OnRawMouseMove(event);
                        });
                    }
                }

                return DefWindowProc(hWnd, msg, wParam, lParam);
            }

            case WM_SIZE:
            {
                if (wParam != SIZE_RESTORED && wParam != SIZE_MAXIMIZED)
                    break;

                m_window.m_Width = LOWORD(lParam);
                m_window.m_Height = HIWORD(lParam);

                if (m_window.m_Width == 0 && m_window.m_Height == 0)
                    break;

                const WindowResizeEvent event(m_window.m_Width, m_window.m_Height);
                m_window.SpawnEvent(EventType::WINDOW_RESIZE,
                    [&](EventHandler& eventHandler)
                    {
                        eventHandler.OnWindowResize(event);
                    });
            }
            break;

            case WM_GETMINMAXINFO:
            {
                if (!m_BoundsEnabled)
                    break;

                const auto pInfo = reinterpret_cast<MINMAXINFO*>(lParam);
                pInfo->ptMinTrackSize = {
                    static_cast<long>(m_MinWidth),
                    static_cast<long>(m_MinHeight)
                };
                pInfo->ptMaxTrackSize = {
                    static_cast<long>(m_MaxWidth),
                    static_cast<long>(m_MaxHeight)
                };
            }
            break;

            case WM_PAINT:
            {
                m_window.SpawnEvent(EventType::WINDOW_REPAINT,
                    [&](EventHandler& eventHandler)
                    {
                        eventHandler.OnWindowRepaint();
                    });

                return DefWindowProc(hWnd, msg, wParam, lParam);
            }

            case WM_LBUTTONDOWN:
            {
                SpawnMouseClick(lParam, ClickType::LEFT_BUTTON, true);
            }
            break;

            case WM_LBUTTONUP:
            {
                SpawnMouseClick(lParam, ClickType::LEFT_BUTTON, false);
            }
            break;

            case WM_MBUTTONDOWN:
            {
                SpawnMouseClick(lParam, ClickType::MIDDLE_BUTTON, true);
            }
            break;

            case WM_MBUTTONUP:
            {
                SpawnMouseClick(lParam, ClickType::MIDDLE_BUTTON, false);
            }
            break;

            case WM_RBUTTONDOWN:
            {
                SpawnMouseClick(lParam, ClickType::RIGHT_BUTTON, true);
            }
            break;

            case WM_RBUTTONUP:
            {
                SpawnMouseClick(lParam, ClickType::RIGHT_BUTTON, false);
            }
            break;

            case WM_XBUTTONDOWN:
            {
                SpawnXbuttonClick(lParam, wParam, true);
            }
            break;

            case WM_XBUTTONUP:
            {
                SpawnXbuttonClick(lParam, wParam, false);
            }
            break;

            default: return DefWindowProc(hWnd, msg, wParam, lParam);
        }

        return 0;
    }

    void Window::Impl::SpawnMouseClick(
        const LPARAM lParam,
        const Input::MouseClickInfo::ClickType type,
        const bool pressed
    ) const
    {
        using namespace Input;

        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        POINT mouse;
        mouse.x = x;
        mouse.y = y;

        ClientToScreen(m_Hwnd, &mouse);

        MouseClickInfo event(
            (int) mouse.x,
            (int) mouse.y,
            x,
            y,
            type,
            pressed
        );

        m_window.SpawnInput(InputType::MOUSE_CLICK,
            [&](InputListener& inputListener)
            {
                inputListener.OnMouseClick(event);
            });
    }

    bool Window::Impl::SpawnXbuttonClick(
        LPARAM lParam,
        const WPARAM wParam,
        bool pressed
    )
    {
        using ClickType = Input::MouseClickInfo::ClickType;

        auto type = ClickType::SIDE_BUTTON1;
        switch (GET_XBUTTON_WPARAM(wParam))
        {
            case XBUTTON1: type = ClickType::SIDE_BUTTON1;
            case XBUTTON2: type = ClickType::SIDE_BUTTON2;

            default: return false;
        }

        SpawnMouseClick(lParam, type, pressed);

        return true;
    }

    void Window::Impl::GenerateClassName()
    {
        static constexpr const size_t CLASS_NAME_LENGTH = 32;

        const std::wstring nums = L"0123456789";
        const std::wstring caps = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const std::wstring lowers = L"abcdefghijklmnopqrstuvwxyz";
        const std::wstring allChars = nums + caps + lowers;

        std::random_device device;
        std::mt19937 generator(device());
        std::uniform_int_distribution<size_t> dist(0, allChars.size() - 1);

        for (size_t i = 0; i < CLASS_NAME_LENGTH; i++)
            m_ClassName += allChars[dist(generator)];
    }

    void* Window::GetHandle() const
    {
        return m_impl->m_Hwnd;
    }

    Window::Impl* Window::GetImpl() const
    {
        return m_impl.get();
    }
}
