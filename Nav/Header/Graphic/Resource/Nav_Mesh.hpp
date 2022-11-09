#ifndef __NAV_MESH_HPP__
#define __NAV_MESH_HPP__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>
#include <Math/Vector/Single/Nav_Float2.hpp>
#include <Math/Vector/Single/Nav_Float3.hpp>
#include <Math/Vector/Single/Nav_Float4.hpp>

#define LAYOUT_MAX_ELEMENTS 16

#define NONE_ELEMENT ((UInt8)0U)
#define VEC2_ELEMENT ((UInt8)sizeof(Float2))
#define VEC3_ELEMENT ((UInt8)sizeof(Float3))
#define VEC4_ELEMENT ((UInt8)sizeof(Float4))

class NAV_API Mesh
{
    public :
    struct Triangle
    {
        UInt16 _0;
        UInt16 _1;
        UInt16 _2;
    };

    struct Layout
    {
        public :    
        typedef UInt8 Element;

        private :
        Size _peek = 0ULL;
        Element _elements[LAYOUT_MAX_ELEMENTS];

        public :
        inline Size GetElementsCount()
        {
            return this->_peek;
        }
        inline Size GetVertexSize()
        {
            Size structureSize = 0ULL;
            for(Size elIdx = 0ULL; elIdx <= _peek; elIdx++){structureSize += _elements[elIdx];}
            return structureSize;
        }
        inline Bool Write(Element element)
        {
            if(this->_peek >= 15){return False;}

            _elements[this->_peek] = element;
            _peek++;
            return True;
        }
        inline Bool Remove()
        {
            if(this->_peek <= 0ULL){return False;}

            _elements[this->_peek] = NONE_ELEMENT;
            _peek--;
            return True;
        }
        inline Void Reset()
        {
            for(Size elIdx = 0ULL; elIdx <= _peek; elIdx++){this->_elements[elIdx] = NONE_ELEMENT;}
            _peek = 0ULL;
        }
        inline Element GetIndex(Int32 idx)
        {
            return _elements[idx];
        }
        inline Size GetElementMemberCount(Element element)
        {
            switch (element)
            {
                case VEC2_ELEMENT :
                    return 2ULL;
                break;

                case VEC3_ELEMENT :
                    return 3ULL;
                break;

                case VEC4_ELEMENT :
                    return 4ULL;
                break;
            }

            return 0ULL;
        }
    };

    struct Info
    {
        public :
        Layout layout = Layout{};
        Size vertexBufferSize = 0ULL;
        Size triangleCount = 0ULL;
    };
};

#endif