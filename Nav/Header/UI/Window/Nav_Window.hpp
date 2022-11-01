#ifndef __NAV_WINDOW_H__
#define __NAV_WINDOW_H__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>
#include <Type/Nav_Value_Type.hpp>
#include <Math/Nav_Int2.hpp>

class NAV_API Window
{
    public :
    DECLARE_PLATFORM_PART();
    DECLARE_GET_PLATFORM_PART();
    DECLARE_PLATFORM_PART_GET_SIZE();

    enum WindowMode
    {
        Windowed,
        Borderless,
        FullScreen
    };

    private :
    Bool _canClose;
    WindowMode _mode;

    public :
    Bool UpdateEvents();
    Bool GetCanClose();
    void SetCanClose(Bool state);
    WindowMode GetMode();
    Int2 GetClientSize();


    public :
    static Bool Create(char* title, int width, int height, WindowMode type, Window* wnd);
    static Bool Destroy(Window* wnd);
};

#endif