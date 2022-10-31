#ifndef __NAV_WINDOW_H__
#define __NAV_WINDOW_H__

#include <Api/Nav_Api.h>
#include <Type/Nav_Value_Type.h>

class NAV_API Window
{
    public :
    struct PlatformPart;
    enum WindowType
    {
        Windowed,
        Borderless
    };

    private :
    Bool _canClose;
    WindowType _type;

    public :
    PlatformPart* GetPlatformPart();
    Bool UpdateEvents();
    Bool GetCanClose();
    void SetCanClose(Bool state);

    public :
    static Size GetSize();
    static Bool Create(char* title, int width, int height, WindowType type, Window* wnd);
    static Bool Destroy(Window* wnd);
};

#endif