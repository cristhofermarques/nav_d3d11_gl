#ifndef __NAV_STACK_HPP__
#define __NAV_STACK_HPP__

#include <Api/Nav_Api.hpp>
#include <Type/Primitive/Nav_Aka.hpp>
#include <Math/Nav_Comparation.hpp>

template<class T>
class Stack
{
    private :
    Size _capacity = 0ULL;
    Size _peek = 0ULL;

    public :
    inline Size GetCapacity(){return _capacity;}
    inline Size GetCount(){return _peek;}
    inline Size GetFreeSlotCount(){return _capacity - _peek;}
    inline T* GetData(){return (T*)(this + 1);}
    inline T* GetIndex(Size idx)
    {
        if(IS_NOT_IN_RANGE(idx, 0ULL, _peek - 1ULL)){return NullPtr;}
        return & GetData()[idx];
    }
    Bool Add(T* t)
    {
        if(_peek >= _capacity){return False;}

        *(GetData() + _peek) = *t;
        _peek++;
        return True;
    }
    Bool Add(T t)
    {
        if(_peek >= _capacity){return False;}

        *(GetData() + _peek) = t;
        _peek++;
        return True;
    }
    Bool Remove()
    {
        if(_peek <= 0ULL){return False;}

        _peek--;
        return True;
    }
};

#define DECLARE_STACK_STRUCTURE_DATA(dataCapacity) \
    template<class T> \
    struct Stack##dataCapacity \
    { \
        public : \
        Size capacity = dataCapacity; \
        Size peek = 0ULL; \
        T data[dataCapacity]; \
        \
        public : \
        inline Stack<T>* AsStack(){return (Stack<T>*)this;} \
    };


DECLARE_STACK_STRUCTURE_DATA(8);

#endif