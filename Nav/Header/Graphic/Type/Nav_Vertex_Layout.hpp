// #ifndef __NAV_VERTEX_LAYOUT_HPP__
// #define __NAV_VERTEX_LAYOUT_HPP__

// #include <Math/Vector/Single/Nav_Float2.hpp>
// #include <Math/Vector/Single/Nav_Float3.hpp>

// #define NONE_ELEMENT (UInt8)0U
// #define FLOAT2_ELEMENT (UInt8)sizeof(Float2)
// #define FLOAT3_ELEMENT (UInt8)sizeof(Float3)
// #define FLOAT4_ELEMENT (UInt8)sizeof(Float4)

// struct VertexLayout
// {
//     typedef UInt8 Element;

//     public :
//     Size size = 0ULL;

//     private :
//     Size _peek = 0ULL;
//     Element _elements[16];

//     public :    
//     inline Size GetElementsCount(){return this->_peek;}

//     inline Size GetStructureSize()
//     {
//         Size structureSize = 0ULL;
//         for(Size elIdx = 0ULL; elIdx <= _peek; elIdx++){structureSize += _elements[elIdx];}
//         return structureSize;
//     }

//     inline Bool Write(Element element)
//     {
//         if(this->_peek >= 15){return False;}

//         _elements[this->_peek] = element;
//         _peek++;
//         return True;
//     }
    
//     inline Bool Remove()
//     {
//         if(this->_peek <= 0ULL){return False;}

//         _elements[this->_peek] = NONE_ELEMENT;
//         _peek--;
//         return True;
//     }

//     inline void Reset()
//     {
//         for(Size elIdx = 0ULL; elIdx <= _peek; elIdx++){this->_elements[elIdx] = NONE_ELEMENT;}
//         _peek = 0ULL;
//     }
// };

// #endif