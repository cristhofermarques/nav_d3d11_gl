#ifndef __NAV_EVENT_HPP__
#define __NAV_EVENT_HPP__

#include <Api/Nav_Api.hpp>
#include <Type/Primitive/Nav_Aka.hpp>

template <class T>
class NAV_API Event
{
    private :
    Size _capacity;
    Size _peek;

    public :
    T* GetCallbacks()
    {
        return (T*)this + 1;
    }
};

#endif