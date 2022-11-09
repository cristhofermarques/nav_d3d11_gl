#ifndef __NAV_SHADER_HPP__
#define __NAV_SHADER_HPP__

#define SHADER_LAYOUT_MAX_ELEMENTS 16

#define NONE_ELEMENT ((UInt8)0U)
#define VEC2_ELEMENT ((UInt8)sizeof(Float2))
#define VEC3_ELEMENT ((UInt8)sizeof(Float3))
#define VEC4_ELEMENT ((UInt8)sizeof(Float4))

#include <Api/Nav_Api.hpp>
#include <Type/Text/Nav_String.hpp>

class NAV_API Shader
{
    public :
    struct Layout
    {
        public :
        struct Element
        {
            public :
            UInt8 type;
            String8 name;

            public :
            inline static Element Empty()
            {
                Element element = NullStruct;
                element.type = NONE_ELEMENT;
                element.name = String8::Empty();
                return element;
            };
        };

        private :
        Bool _isLocked = False;
        Size _peek = 0ULL;
        Element _elements[SHADER_LAYOUT_MAX_ELEMENTS] = NullStruct;

        public :
        inline Void Reset()
        {
            if(this->_isLocked){return;}
            memset(this, 0, SHADER_LAYOUT_MAX_ELEMENTS * sizeof(Element));
        }
        inline Void Add(Element* element)
        {
            if(element == NullPtr || element->type == NONE_ELEMENT){return;}
            if(this->_isLocked || this->_peek >= SHADER_LAYOUT_MAX_ELEMENTS - 1){return;}

            this->_elements[_peek] = *element;
            this->_peek++;
        }
        inline Void RemoveLast()
        {
            if(this->_isLocked || this->_peek >= 0ULL){return;}

            memset(& this->_elements[_peek], 0, sizeof(Element));
            this->_peek--;
        }
        inline Size GetCount()
        {
            return this->_peek;
        }
        inline Element* GetIndex(Int32 idx)
        {
            if(idx < 0 || idx > this->_peek){return NullPtr;}
            return & this->_elements[idx];
        }
        inline Bool SetIsLocked()
        {
            return this->_isLocked;
        }
        inline Void SetLockedState(Bool state)
        {
            this->_isLocked = state;
        }
    
        public :
        inline static Layout Empty()
        {
            Layout layout = Layout{};
            layout._isLocked = False;
            layout._peek = 0ULL;
            for(Size i = 0ULL; i < SHADER_LAYOUT_MAX_ELEMENTS; i++)
            {
                layout._elements[i] = Element::Empty();
            }
            return layout;
        }
    };

    struct Info
    {
        public :
        Mesh::Layout layout = Mesh::Layout{};
    };
};

#endif