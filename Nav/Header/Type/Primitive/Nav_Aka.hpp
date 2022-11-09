#ifndef __NAV_AKA_HPP__
#define __NAV_AKA_HPP__

#include <stdio.h>

#define NullPtr    nullptr
#define NullStruct {0}
#define Null       0

typedef void                   Void;
typedef char                   Bool;
typedef char                   Char;
typedef wchar_t                WChar;
typedef char                   Int8;
typedef short int              Int16;
typedef long int               Int32;
typedef long long int          Int64;
typedef unsigned char          UInt8;
typedef unsigned short int     UInt16;
typedef unsigned long int      UInt32;
typedef unsigned long long int UInt64;
typedef size_t                 Size;
typedef float                  Float;
typedef double                 Double;

#define False ((Bool)0x00)
#define True  ((Bool)0xff)

#endif