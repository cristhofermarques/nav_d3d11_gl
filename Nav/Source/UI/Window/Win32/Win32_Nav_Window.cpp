#include <UI/Window/Nav_Window.hpp>
#include <UI/Window/Win32/Win32_Nav_Window_Platform_Part.hpp>
#include <windows.h>

LPCSTR NavWindowClassName = (LPCSTR)"nav_wnd_class";
const UINT NAV_CLOSE_WINDOW = WM_USER + 1;

IMPLEMENT_GET_PLATFORM_PART(Window);
IMPLEMENT_PLATFORM_PART_GET_SIZE(Window);

LRESULT CALLBACK NavWindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE :
            PostMessage(hWnd, NAV_CLOSE_WINDOW, 0, 0);
            return 0;
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);

}

Bool Window::UpdateEvents()
{
    PlatformPart* platPart = this->GetPlatformPart();
    if(platPart == NullPtr){return False;}

    MSG msg;
    HWND hWnd = platPart->hWnd;
    while(PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
    {
        if(msg.message == NAV_CLOSE_WINDOW && this->_canClose)
        {
            return False;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return True;
}

Bool Window::GetCanClose()
{
    return this->_canClose;
}

void Window::SetCanClose(Bool state)
{
    this->_canClose = state;
}

Window::WindowMode Window::GetMode()
{
    return this->_mode;
}

Int2 Window::GetClientSize()
{
    PlatformPart* platPart = this->GetPlatformPart();
    if(platPart == NullPtr){return {-1, -1};}

    RECT rect;
    if(!GetClientRect(platPart->hWnd, &rect)){return {-1, -1};}

    Int2 size = NullStruct;
    size.x = rect.right - rect.left;
    size.y = rect.bottom - rect.top;
    return size;
}

Bool Window::Create(char* title, int width, int height, Window::WindowMode mode, Window* wnd)
{
    if(wnd == NullPtr){return False;}

    HINSTANCE hInstance = GetModuleHandleA(NULL);

    WNDCLASSEXA wndClassExA = {0};
    wndClassExA.cbSize = sizeof(WNDCLASSEXA);
    wndClassExA.style = CS_OWNDC;
    wndClassExA.lpfnWndProc = NavWindowProcedure;
    wndClassExA.cbClsExtra = 0;
    wndClassExA.cbWndExtra = 0;
    wndClassExA.hInstance = hInstance;
    wndClassExA.hIcon = 0;
    wndClassExA.hCursor = 0;
    wndClassExA.lpszMenuName = 0;
    wndClassExA.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClassExA.lpszClassName = NavWindowClassName;

    if(! GetClassInfoExA(hInstance, NavWindowClassName, &wndClassExA))
    {
        if(! RegisterClassExA(&wndClassExA))
        {
            //DEBUG_ERROR("Falied Register Window Class");
            return False;
        }
    }

    DWORD wndStyle = WS_OVERLAPPED | WS_SYSMENU;
    switch(mode)
    {
        case Windowed :
            wndStyle = WS_OVERLAPPED | WS_SYSMENU;
        break;

        case Borderless :
            wndStyle = WS_POPUP;
        break;

        default :
            wndStyle = WS_OVERLAPPEDWINDOW;
        break;
    }

    HWND hWnd = CreateWindowExA(0, NavWindowClassName, title, wndStyle | WS_VISIBLE, 0, 0, width, height, NULL, NULL, hInstance, NULL);

    if(!hWnd)
    {
        //DEBUG_ERROR("falied create window");
        return False;
    }

    HDC hDc = GetWindowDC(hWnd);
    wnd->_canClose = True;
    wnd->_mode = mode;
    wnd->GetPlatformPart()->hWnd = hWnd;
    wnd->GetPlatformPart()->hDc = hDc;

    return True;
}

Bool Window::Destroy(Window* wnd)
{
    if(wnd != NullPtr)
    {
        Bool destroyed = DestroyWindow(wnd->GetPlatformPart()->hWnd);
        return destroyed;
    }

    return False;
}