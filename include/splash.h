#pragma once

#include "types.h"


#define FLAG_ROTATED     (1u<<3)
#define FLAG_COMPRESSED  (1u<<4)
#define FLAG_SWAPPED     (1u<<5)


enum
{
	FORMAT_RGB565  = 0,
	FORMAT_RGB8    = 1,
	FORMAT_RGBA8   = 2,
	FORMAT_INVALID = 7
};

struct SplashHeader final
{
	u32 magic;
	u16 width;
	u16 height;
	u32 flags;
};



bool pngToSplash(u32 flags, const char *const inFile, const char *const outFile);
