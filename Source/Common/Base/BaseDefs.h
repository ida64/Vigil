/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_BASEDEFS_H
#define VIGILSDK_BASEDEFS_H

//
// Types
//

// Signed 8-bit integer
typedef signed char vgChar;
// Unsigned 8-bit integer
typedef unsigned char vgUChar;
// Signed 16-bit integer
typedef signed short vgShort;
// Unsigned 16-bit integer
typedef unsigned short vgUShort;
// Signed 32-bit integer
typedef signed int vgS32;
// Unsigned 32-bit integer
typedef unsigned int vgU32;
// Signed 64-bit integer
typedef signed long long vgS64;
// Unsigned 64-bit integer
typedef unsigned long long vgU64;
// 32-bit floating point
typedef float vgFloat;
// 64-bit floating point
typedef double vgDouble;
// 32-bit boolean
typedef bool vgBool;
// const char*
typedef const char* vgString;

//
// Macros
//

#define VG_ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

// Inline macro
#ifdef _MSC_VER
#define VG_INLINE __forceinline
#else
#define VG_INLINE inline
#endif

#endif //VIGILSDK_BASEDEFS_H
