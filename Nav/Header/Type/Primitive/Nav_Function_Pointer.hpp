#ifndef __NAV_FUNCTION_POINTER_H__
#define __NAV_FUNCTION_POINTER_H__

#define DEFINE_FUNCTION_POINTER(returnType, functionName, ...) returnType (* functionName)(__VA_ARGS__)
#define CALL_FUNCTION_POINTER(function, ...) (* function)(__VA_ARGS__)
#define FUNCTION_POINTER_DEFINE(returnType, functionName, ...) returnType (* functionName)(__VA_ARGS__)
#define FUNCTION_POINTER_CALL(function, ...) (* function)(__VA_ARGS__)

#endif