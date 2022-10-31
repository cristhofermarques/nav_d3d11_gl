#ifndef __NAV_VALUE_TYPE_H__
#define __NAV_VALUE_TYPE_H__

#define NullPtr nullptr
#define NullStruct {0}
#define Null 0

typedef unsigned char Bool;

#define False 0x00
#define True 0xff

typedef signed char Int8;
typedef signed short Int16;
typedef signed long int Int32;
typedef signed long long int Int64;

typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned long int UInt32;
typedef unsigned long long int UInt64;

typedef signed char Bit8;
typedef signed short Bit16;
typedef signed int Bit32;
typedef signed long long Bit64;

typedef float Single;
typedef double Double;

typedef UInt64 Size;

#endif