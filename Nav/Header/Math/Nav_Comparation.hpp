#ifndef __NAV_COMPARATION_HPP__
#define __NAV_COMPARATION_HPP__

#include <Api/Nav_Api.hpp>
#include <Type/Primitive/Nav_Value_Type.hpp>

#define IS_EVEN(n) (((n % 2) == 0) ? True : False)
#define IS_ODD(n) (((n % 2) > 0) ? True : False)

#define IS_IN_RANGE(n, min, max) ((n >= min && n <= max) ? True : False)
#define IS_NOT_IN_RANGE(n, min, max) ((n >= min && n <= max) ? False : True)

class NAV_API Math
{
    public :
    inline static Size Major(Size* sizes, Size size)
    {
        if(sizes == NullPtr){return 0ULL;}

        Size major = 0ULL;
        for(Size idx = 0ULL; idx < size; idx++)
        {
            Size currSize = sizes[idx];
            if(currSize > major){major = currSize;}
        }

        return major;
    }
};

#endif