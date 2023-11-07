/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_BASEDEFS_H
#define VIGILSDK_BASEDEFS_H

//
// Types
//

typedef signed char vgChar;
typedef unsigned char vgUChar;
typedef unsigned char vgByte;
typedef unsigned char vgU8;
typedef signed char vgS8;
typedef signed short vgShort;
typedef unsigned short vgUShort;
typedef signed int vgS32;
typedef unsigned int vgU32;
typedef signed long long vgS64;
typedef unsigned long long vgU64;
typedef float vgFloat;
typedef double vgDouble;
typedef bool vgBool;
typedef const char* vgString;

//
// Macros
//

#define VG_ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#ifdef _MSC_VER
#define VG_INLINE __forceinline
#else
#define VG_INLINE inline
#endif

#define VG_FROM_PTR_UNSAFE(Name) Name.get()

//
// Markers
//


#define VG_CLASS_REFLECTION

#endif //VIGILSDK_BASEDEFS_H
