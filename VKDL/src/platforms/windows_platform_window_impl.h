#pragma once

#define VKDL_USE_PLATFORM

#include "../../include/vkdl/core/include_vulkan.h"
#include "../../include/vkdl/core/context.h"
#include "../../include/vkdl/system/platform_window.h"
#include "../../include/vkdl/system/cursor.h"
#include "../../include/vkdl/math/rect.h"
#include "../../include/vkdl/core/exception.h"
#include "platform_window_base.h"

#define WNDCLASSNAME "vkdl_window_class"

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#define RECT_WIDTH(rect) (rect.right - rect.left)
#define RECT_HEIGHT(rect) (rect.bottom - rect.top)
#define RECT_POS(rect) rect.left, rect.top
#define RECT_SIZE(rect) RECT_WIDTH(rect), RECT_HEIGHT(rect)

VKDL_BEGIN

static void set_process_dpi_aware()
{
    HINSTANCE shCoreDll = LoadLibrary("Shcore.dll");

    if (shCoreDll) {
        enum ProcessDpiAwareness {
            ProcessDpiUnaware         = 0,
            ProcessSystemDpiAware     = 1,
            ProcessPerMonitorDpiAware = 2
        };

        typedef HRESULT(WINAPI* SetProcessDpiAwarenessFuncType)(ProcessDpiAwareness);
        SetProcessDpiAwarenessFuncType SetProcessDpiAwarenessFunc = reinterpret_cast<SetProcessDpiAwarenessFuncType>(reinterpret_cast<void*>(GetProcAddress(shCoreDll, "SetProcessDpiAwareness")));

        if (SetProcessDpiAwarenessFunc) {
            if (SetProcessDpiAwarenessFunc(ProcessPerMonitorDpiAware) == E_INVALIDARG) {
                VKDL_ERROR("Failed to set process DPI awareness");
            } else {
                FreeLibrary(shCoreDll);
                return;
            }
        }

        FreeLibrary(shCoreDll);
    }

    HINSTANCE user32Dll = LoadLibrary("user32.dll");

    if (user32Dll) {
        typedef BOOL(WINAPI* SetProcessDPIAwareFuncType)(void);
        SetProcessDPIAwareFuncType SetProcessDPIAwareFunc = reinterpret_cast<SetProcessDPIAwareFuncType>(reinterpret_cast<void*>(GetProcAddress(user32Dll, "SetProcessDPIAware")));

        if (SetProcessDPIAwareFunc) {
            if (!SetProcessDPIAwareFunc())
                VKDL_ERROR("Failed to set process DPI awareness");
        }

        FreeLibrary(user32Dll);
    }
}

static DWORD create_style(VKDL_NAMESPACE_NAME::PlatformWindowStyleFlags style)
{
    DWORD dw_style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
    
    if (style & PlatformWindowStyle::Resizable)
        dw_style |= WS_THICKFRAME;

    if (style & PlatformWindowStyle::Minimize)
        dw_style |= WS_CAPTION | WS_MINIMIZEBOX;

    if (style & PlatformWindowStyle::Maximize)
        dw_style |= WS_CAPTION | WS_MAXIMIZEBOX;
    
    if (style & PlatformWindowStyle::Close)
        dw_style |= WS_CAPTION | WS_SYSMENU;

    return dw_style;
}

static DWORD create_ex_style(PlatformWindowStyleFlags style)
{
    DWORD dw_ex_style = WS_EX_APPWINDOW;

    if (style & PlatformWindowStyle::TopMost)
        dw_ex_style |= WS_EX_TOPMOST;

    return dw_ex_style;
}

static irect get_window_rect(HWND hwnd)
{
    RECT rect;
    GetWindowRect(hwnd, &rect);
    return { RECT_POS(rect), RECT_SIZE(rect)};
}

static irect get_client_rect(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    return { RECT_POS(rect), RECT_SIZE(rect) };
}

static void setTracking(HWND hwnd, bool track)
{
    TRACKMOUSEEVENT mouseEventMsg;
    mouseEventMsg.cbSize      = sizeof(TRACKMOUSEEVENT);
    mouseEventMsg.dwFlags     = track ? TME_LEAVE : TME_CANCEL;
    mouseEventMsg.hwndTrack   = hwnd;
    mouseEventMsg.dwHoverTime = HOVER_DEFAULT;
    TrackMouseEvent(&mouseEventMsg);
}

static Key virtual_key_code_to_key(WPARAM key, LPARAM flags)
{
    switch (key) {
    case VK_SHIFT: {
        static UINT lShift = MapVirtualKeyW(VK_LSHIFT, MAPVK_VK_TO_VSC);
        UINT scancode = static_cast<UINT>((flags & (0xFF << 16)) >> 16);
        return scancode == lShift ? Key::LShift : Key::RShift;
    }
    case VK_MENU: return (GET_Y_LPARAM(flags) & KF_EXTENDED) ? Key::RAlt : Key::LAlt;
    case VK_CONTROL: return (GET_Y_LPARAM(flags) & KF_EXTENDED) ? Key::RControl : Key::LControl;
    default: return Keyboard::toKey(key);
    }
}

static uint16_t* decode(uint16_t* begin, uint16_t* end, uint32_t& output, uint32_t replacement = 0)
{
    uint16_t first = *begin++;

    if ((first >= 0xd800) && (first <= 0xdbff)) {
        if (begin < end) {
            uint32_t second = *begin++;
            if ((second >= 0xdc00) && (second <= 0xdfff)) {
                output = ((first - 0xd800u) << 10) + (second - 0xdc00) + 0x0010000;
            } else {
                output = replacement;
            }
        } else {
            begin = end;
            output = replacement;
        }
    } else {
        output = first;
    }

    return begin;
}

static uint32_t* to_utf32(uint16_t* begin, uint16_t* end, uint32_t* output)
{
    while (begin < end)
    {
        uint32_t codepoint;
        begin = decode(begin, end, codepoint);
        *output++ = codepoint;
    }

    return output;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void register_window_class() {
    WNDCLASSEX wc;
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.hIcon         = nullptr;
    wc.hCursor       = nullptr;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = WNDCLASSNAME;
    wc.hIconSm       = nullptr;

    if (!RegisterClassEx(&wc))
        VKDL_ERROR("error while creating window class");
}

VKDL_PRIV_BEGIN

struct PlatformWindowImpl : public PlatformWindowBase
{
    PlatformWindowImpl() :
        PlatformWindowBase(),
        hwnd(nullptr),
        surrogate(0),
        is_mouse_inside(false),
        is_resizing(false),
        key_repeat_enabled(true)
    {
    }

    HWND hwnd;
    int  surrogate;
    bool is_mouse_inside;
    bool is_resizing;
    bool key_repeat_enabled;

    void processEventMsg(bool block) {
        MSG msg;
        if (block) {
            while (GetMessage(&msg, NULL, 0, 0)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else {
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    bool pollEventMsg(EventMsg& event) {
        processEventMsg(false);

        if (events.empty()) return false;

        event = events.front();
        events.pop();
        return true;
    }

    bool waitEventMsg(EventMsg& event) {
        if (!events.empty()) {
            event = events.front();
            events.pop();
            return true;
        }

        do {
            processEventMsg(true);
        } while (events.empty());

        event = events.front();
        events.pop();
        return true;
    }
};

VKDL_PRIV_END

static uint32_t window_counter = 0;

PlatformWindow::PlatformWindow() :
    impl(nullptr)
{
}

PlatformWindow::PlatformWindow(PlatformWindow&& rhs) :
    impl(std::move(rhs.impl))
{
}

PlatformWindow::PlatformWindow(uint32_t width, uint32_t height, const char* title, PlatformWindowStyleFlags style)
{
    create(width, height, title, style);
}

PlatformWindow::PlatformWindow(uint32_t width, uint32_t height, const char* title, PlatformWindow* parent, PlatformWindowStyleFlags style)
{
    create(width, height, title, parent, style);
}

PlatformWindow::~PlatformWindow()
{
    impl.reset();
}

PlatformWindow& PlatformWindow::operator=(PlatformWindow&& rhs)
{
    impl = std::move(rhs.impl);
    return *this;
}

void PlatformWindow::create(uint32_t width, uint32_t height, const char* title, PlatformWindowStyleFlags style)
{
    create(width, height, title, nullptr, style);
}

void PlatformWindow::create(uint32_t width, uint32_t height, const char* title, PlatformWindow* parent, PlatformWindowStyleFlags style)
{
    impl = std::make_unique<VKDL_PRIV_NAMESPACE_NAME::PlatformWindowImpl>();

    impl->platform_window   = this;
    impl->parent            = parent;

    if (window_counter++ == 0) {
        register_window_class();
        set_process_dpi_aware();
    }

    DWORD dw_style    = create_style(style);
    DWORD dw_ex_style = create_ex_style(style);

    HDC screenDC = GetDC(NULL);
    int32_t left = (GetDeviceCaps(screenDC, HORZRES) - (int32_t)(width)) / 2;
    int32_t top  = (GetDeviceCaps(screenDC, VERTRES) - (int32_t)(height)) / 2;
    ReleaseDC(NULL, screenDC);

    RECT rect = { 0, 0, (LONG)width, (LONG)height };
    AdjustWindowRectEx(&rect, dw_style, false, dw_ex_style);

    impl->hwnd = CreateWindowEx(
        dw_ex_style,
        WNDCLASSNAME,
        title,
        dw_style,
        left,
        top,
        RECT_WIDTH(rect),
        RECT_HEIGHT(rect),
        parent ? parent->impl->hwnd : NULL,
        NULL,
        GetModuleHandle(NULL),
        impl.get());

    if (style & PlatformWindowStyle::Visible) {
        ShowWindow(impl->hwnd, SW_SHOW);
        impl->visible = true;
    }

    auto window_rect = get_window_rect(impl->hwnd);

    impl->position          = window_rect.getPosition();
    impl->size              = window_rect.getSize();
    impl->frame_buffer_size = { width, height };
    impl->title             = title;

    {
        auto& ctx = Context::get();

        vk::Win32SurfaceCreateInfoKHR info = {
            {},
            GetModuleHandle(NULL),
            impl->hwnd
        };

        impl->surface = ctx.instance.createWin32SurfaceKHR(info);
        impl->init();
    }
}

void PlatformWindow::destroy()
{
    impl->destroy();
    impl.reset();
}

void PlatformWindow::processEvent(bool block)
{
    impl->processEventMsg(block);
}

bool PlatformWindow::pollEvent(EventMsg& event)
{
    if (!impl) return false;
    return impl->pollEventMsg(event);
}

bool PlatformWindow::waitEvent(EventMsg& event)
{
    return impl->waitEventMsg(event);
}

bool PlatformWindow::hasEvent() const
{
    return !impl->events.empty();
}

PROPERTY_IMPL_GET(PlatformWindow, Position)
{
    return impl->position;
}

PROPERTY_IMPL_SET(PlatformWindow, Position)
{
    if (impl->position != value) {
        SetWindowPos(impl->hwnd, nullptr, value.x, value.y, 0, 0, SWP_NOSIZE);
    }
}

PROPERTY_IMPL_GET(PlatformWindow, Size)
{
    return impl->size;
}

PROPERTY_IMPL_SET(PlatformWindow, Size)
{
    if (impl->size != value) {
        SetWindowPos(impl->hwnd, nullptr, 0, 0, value.x, value.y, SWP_NOMOVE);
        impl->update_swapchain = true;
    }
}

PROPERTY_IMPL_GET(PlatformWindow, FrameBufferSize)
{
    return impl->frame_buffer_size;
}

PROPERTY_IMPL_SET(PlatformWindow, FrameBufferSize)
{
    if (impl->frame_buffer_size != value) {
        RECT rect{ 0, 0, (LONG)value.x, (LONG)value.y };

        auto style = GetWindowLong(impl->hwnd, GWL_STYLE);
        auto ex_style = GetWindowLong(impl->hwnd, GWL_EXSTYLE);

        AdjustWindowRectEx(&rect, style, false, ex_style);
        SetWindowPos(impl->hwnd, nullptr, 0, 0, RECT_SIZE(rect), SWP_NOMOVE);
        impl->update_swapchain = true;
    }
}

PROPERTY_IMPL_GET(PlatformWindow, MinSizeLimit)
{
    return impl->size_limit_min;
}

PROPERTY_IMPL_SET(PlatformWindow, MinSizeLimit)
{
    VKDL_CHECK(impl->size_limit_min.x <= impl->size_limit_max.x);
    VKDL_CHECK(impl->size_limit_min.y <= impl->size_limit_max.y);
    impl->size_limit_min = value;
}

PROPERTY_IMPL_GET(PlatformWindow, MaxSizeLimit)
{
    return impl->size_limit_max;
}

PROPERTY_IMPL_SET(PlatformWindow, MaxSizeLimit)
{
    if (value.x == 0) {
        impl->size_limit_min.x = 0;
        impl->size_limit_max.x = 0;
    }

    if (value.y == 0) {
        impl->size_limit_min.y = 0;
        impl->size_limit_max.y = 0;
    }

    VKDL_CHECK(impl->size_limit_min.x <= impl->size_limit_max.x);
    VKDL_CHECK(impl->size_limit_min.y <= impl->size_limit_max.y);
    impl->size_limit_max = value;
}

PROPERTY_IMPL_GET(PlatformWindow, Title)
{
    return impl->title;
}

PROPERTY_IMPL_SET(PlatformWindow, Title)
{
    impl->title = value;
    SetWindowText(impl->hwnd, value.c_str());
}

PROPERTY_IMPL_GET(PlatformWindow, WindowState)
{
    if (impl->minimized) return PlatformWindowState::Minimized;
    if (impl->maximized) return PlatformWindowState::Maximized;
    return PlatformWindowState::Normal;
}

PROPERTY_IMPL_SET(PlatformWindow, WindowState)
{
    switch (value) {
    case PlatformWindowState::Normal:
        ShowWindow(impl->hwnd, SW_RESTORE);
        break;
    case PlatformWindowState::Minimized:
        ShowWindow(impl->hwnd, SW_MAXIMIZE);
        break;
    case PlatformWindowState::Maximized:
        ShowWindow(impl->hwnd, SW_MINIMIZE);
        break;
    default:
        VKDL_ERROR("Unknown PlatformWindowState value");
    }
}

PROPERTY_IMPL_GET(PlatformWindow, Transparency)
{
    return impl->transparency;
}

PROPERTY_IMPL_SET(PlatformWindow, Transparency)
{
    auto clamped = std::clamp(value, 0.f, 1.f);

    if (value == 0.f && impl->transparency != 0.f) {
        SetWindowLong(impl->hwnd, GWL_EXSTYLE, GetWindowLong(impl->hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
    } else if (value != 0.f && impl->transparency == 0.f) {
        SetWindowLong(impl->hwnd, GWL_EXSTYLE, GetWindowLong(impl->hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    }

    SetLayeredWindowAttributes(impl->hwnd, 0, (BYTE)(255.99f * value), LWA_ALPHA);
}

PROPERTY_IMPL_GET(PlatformWindow, Visible)
{
    return impl->visible;
}

PROPERTY_IMPL_SET(PlatformWindow, Visible)
{
    if (impl->visible == value) return;
    ShowWindow(impl->hwnd, impl->visible = value ? SW_SHOW : SW_HIDE);
}

PROPERTY_IMPL_GET(PlatformWindow, Focussed)
{
    return impl->focussed;
}

PROPERTY_IMPL_SET(PlatformWindow, Focussed)
{
    SetFocus(Focussed ? impl->hwnd : NULL);
}

PROPERTY_IMPL_GET(PlatformWindow, Resizable)
{
    return GetWindowLong(impl->hwnd, GWL_STYLE) & WS_THICKFRAME;
}

PROPERTY_IMPL_SET(PlatformWindow, Resizable)
{
    auto style = GetWindowLong(impl->hwnd, GWL_STYLE);

    if ((bool)(style & WS_THICKFRAME) != value) {
        if (value) {
            style |= WS_THICKFRAME;
        }
        else {
            style &= ~WS_THICKFRAME;
        }
        SetWindowLong(impl->hwnd, GWL_STYLE, style);
    }
}

PROPERTY_IMPL_GET(PlatformWindow, VSync)
{
    return false;
}

PROPERTY_IMPL_SET(PlatformWindow, VSync)
{

}

PROPERTY_IMPL_GET(PlatformWindow, Parent)
{
    auto parent_hwnd = (HWND)GetWindowLongPtr(impl->hwnd, GWLP_HWNDPARENT);
    return ((VKDL_PRIV_NAMESPACE_NAME::PlatformWindowImpl*)GetWindowLongPtr(parent_hwnd, GWLP_USERDATA))->parent;
}

PROPERTY_IMPL_SET(PlatformWindow, Parent)
{
    if (value != nullptr)
        SetWindowLongPtr(impl->hwnd, GWLP_HWNDPARENT, (LONG_PTR)value->impl->hwnd);
    else
        SetWindowLongPtr(impl->hwnd, GWLP_HWNDPARENT, NULL);
}

PROPERTY_IMPL_GET(PlatformWindow, IsClosed)
{
    return false;
}

PROPERTY_IMPL_GET(PlatformWindow, IsDestroyed)
{
    return !impl;
}

PROPERTY_IMPL_GET(PlatformWindow, UserPtr)
{
    return impl->user_ptr;
}

PROPERTY_IMPL_SET(PlatformWindow, UserPtr)
{
    impl->user_ptr = value;
}

void* PlatformWindow::getNativeHandle() const
{
    return (void*)impl->hwnd;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* impl = reinterpret_cast<VKDL_PRIV_NAMESPACE_NAME::PlatformWindowImpl*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_CREATE: {
        LONG_PTR window_ptr = (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, window_ptr);
        EventMsg e{ EventMsg::Create };
        ((VKDL_PRIV_NAMESPACE_NAME::PlatformWindowImpl*)window_ptr)->sendEventMsg(e);
    } break;
    case WM_DESTROY: {
        EventMsg e{ EventMsg::Destroy };
        impl->sendEventMsg(e);
    }
    case WM_CLOSE: {
        EventMsg e{ EventMsg::Close };
        impl->sendEventMsg(e);
        return 0;
    }
    case WM_SIZE: {
        uvec2 size    = get_window_rect(hwnd).getSize();
        uvec2 fb_size = get_client_rect(hwnd).getSize();

        if (wParam == SIZE_MINIMIZED) {
            impl->minimized = true;
            impl->maximized = false;
        } else if (wParam == SIZE_MAXIMIZED) {
            impl->minimized = false;
            impl->maximized = true;
        } else if (wParam == SIZE_RESTORED) {
            impl->minimized = false;
            impl->maximized = false;
        }

        if (impl->size != size || impl->frame_buffer_size != fb_size) {
            impl->size              = size;
            impl->frame_buffer_size = fb_size;
            impl->update_swapchain  = true;

            if (wParam == SIZE_MINIMIZED || impl->is_resizing) break;

            EventMsg e{ EventMsg::Resize };
            e.resize.size    = impl->size;
            e.resize.fb_size = impl->frame_buffer_size;
            impl->sendEventMsg(e);

        }
    } break;
    case WM_MOVE: {
        impl->position = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

        EventMsg e{ EventMsg::Move };
        e.move.pos = impl->position;
        impl->sendEventMsg(e);
    } break;
    case WM_ENTERSIZEMOVE: {
        impl->is_resizing = true;
    } break;
    case WM_EXITSIZEMOVE: {
        impl->is_resizing = false;

        uvec2 size    = get_window_rect(hwnd).getSize();
        uvec2 fb_size = get_client_rect(hwnd).getSize();

        if (impl->size != size || impl->frame_buffer_size != fb_size) {
            impl->size              = size;
            impl->frame_buffer_size = fb_size;

            EventMsg e{ EventMsg::Resize };
            e.resize.size    = impl->size;
            e.resize.fb_size = impl->frame_buffer_size;
            impl->sendEventMsg(e);

            impl->update_swapchain = true;
        }
    } break;
    case WM_GETMINMAXINFO: {
        if (!impl) break;

        LPMINMAXINFO info = (LPMINMAXINFO)lParam;
        
        if (impl->size_limit_min.x)
            info->ptMinTrackSize.x = (LONG)impl->size_limit_min.x;
        if (impl->size_limit_min.y)
            info->ptMinTrackSize.y = (LONG)impl->size_limit_min.y;
        if (impl->size_limit_max.x)
            info->ptMaxTrackSize.x = (LONG)impl->size_limit_max.x;
        if (impl->size_limit_max.y)
            info->ptMaxTrackSize.y = (LONG)impl->size_limit_max.y;
        return 0;
    } break;
    case WM_SYSCOMMAND: {
        EventMsg e;
        switch (wParam) {
        case SC_RESTORE:
            e.type = EventMsg::Restore;
            impl->sendEventMsg(e);
            break;
        case SC_MINIMIZE:
            e.type = EventMsg::Minimize;
            impl->sendEventMsg(e);
            break;
        case SC_MAXIMIZE:
            e.type = EventMsg::Maximize;
            impl->sendEventMsg(e);
            break;
        }
    } break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: {
        EventMsg e{ EventMsg::KeyPressed };
        e.keyboard.key  = virtual_key_code_to_key(wParam, lParam);
        e.keyboard.alt  = GET_Y_LPARAM(GetKeyState(VK_MENU)) != 0;
        e.keyboard.ctrl = GET_Y_LPARAM(GetKeyState(VK_CONTROL)) != 0;
        e.keyboard.shft = GET_Y_LPARAM(GetKeyState(VK_SHIFT)) != 0;
        e.keyboard.sys  = GET_Y_LPARAM(GetKeyState(VK_LWIN)) || GET_Y_LPARAM(GetKeyState(VK_RWIN));
        impl->sendEventMsg(e);
        break;
    }
    case WM_KEYUP: 
    case WM_SYSKEYUP: {
        EventMsg e{ EventMsg::KeyReleased };
        e.keyboard.key  = virtual_key_code_to_key(wParam, lParam);
        e.keyboard.alt  = GET_Y_LPARAM(GetKeyState(VK_MENU)) != 0;
        e.keyboard.ctrl = GET_Y_LPARAM(GetKeyState(VK_CONTROL)) != 0;
        e.keyboard.shft = GET_Y_LPARAM(GetKeyState(VK_SHIFT)) != 0;
        e.keyboard.sys  = GET_Y_LPARAM(GetKeyState(VK_LWIN)) || GET_Y_LPARAM(GetKeyState(VK_RWIN));
        impl->sendEventMsg(e);
        break;
    }
    case WM_CHAR: {
        if (impl->key_repeat_enabled || ((lParam & (1 << 30)) == 0)) {
            uint32_t character = static_cast<uint32_t>(wParam);

            if ((character >= 0xd800) && (character <= 0xdbff)) {
                impl->surrogate = static_cast<uint16_t>(character);
            } else {
                if ((character >= 0xdc00) && (character <= 0xdfff)) {
                    uint16_t utf16[] = { impl->surrogate, static_cast<uint16_t>(character) };
                    to_utf32(utf16, utf16 + 2, &character);
                    impl->surrogate = 0;
                }

                EventMsg e{ EventMsg::TextEntered };
                e.text.unicode = character;
                impl->sendEventMsg(e);
            }
        }
    } break;
    case WM_MOUSEWHEEL: {
        POINT position;
        position.x = (int32_t)GET_X_LPARAM(lParam);
        position.y = (int32_t)GET_Y_LPARAM(lParam);
        ScreenToClient(hwnd, &position);

        EventMsg e{ EventMsg::WheelScrolled };
        e.wheel.delta = { 0.f, (float)GET_Y_LPARAM(wParam) / 120.f };
        e.wheel.pos   = { position.x, position.y };
        impl->sendEventMsg(e);
        SetFocus(hwnd);
    } break;
    case WM_MOUSEHWHEEL: {
        POINT position;
        position.x = (int32_t)GET_X_LPARAM(lParam);
        position.y = (int32_t)GET_Y_LPARAM(lParam);
        ScreenToClient(hwnd, &position);

        EventMsg e{ EventMsg::WheelScrolled };
        e.wheel.delta = { (float)GET_Y_LPARAM(wParam) / 120.f, 0.f };
        e.wheel.pos = { position.x, position.y };
        impl->sendEventMsg(e);
        SetFocus(hwnd);
    } break;
    case WM_LBUTTONDOWN: {
        EventMsg e{ EventMsg::MousePressed };
        e.mouseButton.button = Mouse::Left;
        e.mouseButton.pos    = { (int32_t)GET_X_LPARAM(lParam), (int32_t)GET_Y_LPARAM(lParam) };
        impl->sendEventMsg(e);
        SetFocus(hwnd);
    } break;
    case WM_LBUTTONUP: {
        EventMsg e{ EventMsg::MouseReleased };
        e.mouseButton.button = Mouse::Left;
        e.mouseButton.pos    = { (int32_t)GET_X_LPARAM(lParam), (int32_t)GET_Y_LPARAM(lParam) };
        impl->sendEventMsg(e);
    } break;
    case WM_RBUTTONDOWN: {
        EventMsg e{ EventMsg::MousePressed };
        e.mouseButton.button = Mouse::Right;
        e.mouseButton.pos    = { (int32_t)GET_X_LPARAM(lParam), (int32_t)GET_Y_LPARAM(lParam) };
        impl->sendEventMsg(e);
        SetFocus(hwnd);
    } break;
    case WM_RBUTTONUP: {
        EventMsg e{ EventMsg::MouseReleased };
        e.mouseButton.button = Mouse::Right;
        e.mouseButton.pos    = { (int32_t)GET_X_LPARAM(lParam), (int32_t)GET_Y_LPARAM(lParam) };
        impl->sendEventMsg(e);
    } break;
    case WM_MBUTTONDOWN: {
        EventMsg e{ EventMsg::MousePressed };
        e.mouseButton.button = Mouse::Middle;
        e.mouseButton.pos    = { (int32_t)GET_X_LPARAM(lParam), (int32_t)GET_Y_LPARAM(lParam) };
        impl->sendEventMsg(e);
        SetFocus(hwnd);
    } break;
    case WM_MBUTTONUP: {
        EventMsg e{ EventMsg::MouseReleased };
        e.mouseButton.button = Mouse::Middle;
        e.mouseButton.pos    = { (int32_t)GET_X_LPARAM(lParam), (int32_t)GET_Y_LPARAM(lParam) };
        impl->sendEventMsg(e);
    } break;
    case WM_XBUTTONDOWN: {
        EventMsg e{ EventMsg::MousePressed };
        e.mouseButton.button = GET_Y_LPARAM(wParam) == XBUTTON1 ? Mouse::XButton1 : Mouse::XButton2;
        e.mouseButton.pos    = { (int32_t)GET_X_LPARAM(lParam), (int32_t)GET_Y_LPARAM(lParam) };
        impl->sendEventMsg(e);
        SetFocus(hwnd);
    } break;
    case WM_XBUTTONUP: {
        EventMsg e{ EventMsg::MouseReleased };
        e.mouseButton.button = GET_Y_LPARAM(wParam) == XBUTTON1 ? Mouse::XButton1 : Mouse::XButton2;
        e.mouseButton.pos    = { (int32_t)GET_X_LPARAM(lParam), (int32_t)GET_Y_LPARAM(lParam) };
        impl->sendEventMsg(e);
    } break;
    case WM_MOUSELEAVE: {
        if (impl->is_mouse_inside) {
            impl->is_mouse_inside = false;

            EventMsg e{ EventMsg::MouseLeft };
            impl->sendEventMsg(e);
        }
    } break;
    case WM_MOUSEMOVE: {
        int32_t x = static_cast<int32_t>(GET_X_LPARAM(lParam));
        int32_t y = static_cast<int32_t>(GET_Y_LPARAM(lParam));
        ivec2 pos = { x, y };

        RECT area;
        GetClientRect(hwnd, &area);

        if ((wParam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_XBUTTON1 | MK_XBUTTON2)) == 0) {
            if (GetCapture() == hwnd)
                ReleaseCapture();
        } else if (GetCapture() != hwnd) {
            SetCapture(hwnd);
        }

        if ((x < area.left) || (x > area.right) || (y < area.top) || (y > area.bottom)) {
            if (impl->is_mouse_inside) {
                impl->is_mouse_inside = false;

                setTracking(hwnd, false);

                EventMsg e{ EventMsg::MouseLeft };
                impl->sendEventMsg(e);
            }
        } else {
            if (!impl->is_mouse_inside) {
                impl->is_mouse_inside = true;

                setTracking(hwnd, true);

                impl->mouse_prev = pos;

                EventMsg e{ EventMsg::MouseEntered };
                impl->sendEventMsg(e);
            }
        }

        EventMsg e{ EventMsg::MouseMoved };
        e.mouseMove.pos   = pos;
        e.mouseMove.delta = pos - impl->mouse_prev;
        impl->sendEventMsg(e);

        impl->mouse_prev = pos;
    } break;
    case WM_SETFOCUS: {
        EventMsg e{ EventMsg::Focussed };
        impl->sendEventMsg(e);
        impl->focussed = true;
    } return 0;
    case WM_KILLFOCUS: {
        EventMsg e{ EventMsg::LostFocus };
        impl->sendEventMsg(e);
        impl->focussed = false;
    } return 0;
    case WM_SETCURSOR: {
        if (LOWORD(lParam) == HTCLIENT)
            Cursor::updateCursor();
    } break;
    case WM_ERASEBKGND: {
        return 1;
    }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

VKDL_END