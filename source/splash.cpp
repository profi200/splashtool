#include <cstdio>
#include <vector>
#include "types.h"
#include "splash.h"
#include "fsutil.h"
#include "lodepng.h"
#include "compress.h"



// Thanks mtheall
static void rotate(std::vector<u8>& splash, u16 width, u16 height)
{
	std::vector<u8> tmp;
	tmp.swap(splash);
	splash.resize(tmp.size());

	const u8 * const src = tmp.data() + sizeof(SplashHeader);
	u8 *dst = splash.data() + sizeof(SplashHeader);

	for(size_t col = 0; col < width; ++col)
	{
		for(size_t row = 0; row < height; ++row)
		{
			const u8 *p = &src[((height-row)*width + col)*4];
			*dst++ = *p++;
			*dst++ = *p++;
			*dst++ = *p++;
			*dst++ = *p++;
		}
	}
}

static void rgba8ToRgb565(std::vector<u8>& splash, u16 width, u16 height, bool swap)
{
	u8 *data = splash.data() + sizeof(SplashHeader);

	for(u32 i = 0; i < width * height * 4; i += 4)
	{
		u8 r = data[i + 0];
		u8 g = data[i + 1];
		u8 b = data[i + 2];
		u8 a = data[i + 3];

		r = 1.0f * r * a / 255.0f;
		g = 1.0f * g * a / 255.0f;
		b = 1.0f * b * a / 255.0f;

		r = r >> 3;
		g = g >> 2;
		b = b >> 3;

		u16 pixel;
		if(swap) pixel = (b << 11) | (g << 5) | r;
		else     pixel = (r << 11) | (g << 5) | b;

		data[i / 2 + 0] = pixel;
		data[i / 2 + 1] = pixel>>8;
	}

	splash.resize(width * height * 2);
}

static void rgba8ToRgb8(std::vector<u8>& splash, u16 width, u16 height, bool swap)
{
	u8 *data = splash.data() + sizeof(SplashHeader);

	for(u32 i = 0; i < width * height * 4; i += 4)
	{
		u8 r = data[i + 0];
		u8 g = data[i + 1];
		u8 b = data[i + 2];
		u8 a = data[i + 3];

		r = 1.0f * r * a / 255.0f;
		g = 1.0f * g * a / 255.0f;
		b = 1.0f * b * a / 255.0f;

		u32 pixel;
		if(swap) pixel = (r << 16) | (g << 8) | b;
		else     pixel = (b << 16) | (g << 8) | r;

		data[i / 4 * 3 + 0] = pixel;
		data[i / 4 * 3 + 1] = pixel>>8;
		data[i / 4 * 3 + 2] = pixel>>16;
	}

	splash.resize(width * height * 3);
}

static void swapRgba(std::vector<u8>& splash, u16 width, u16 height)
{
	u8 *data = splash.data() + sizeof(SplashHeader);

	for(u32 i = 0; i < width * height * 4; i += 4)
	{
		u8 r = data[i + 0];
		u8 g = data[i + 1];
		u8 b = data[i + 2];
		u8 a = data[i + 3];

		data[i + 0] = a;
		data[i + 1] = b;
		data[i + 2] = g;
		data[i + 3] = r;
	}
}

bool pngToSplash(u32 flags, const char *const inFile, const char *const outFile)
{
	std::vector<u8> png = vectorFromFile(inFile);
	if(png.empty()) return false;

	unsigned err;
	std::vector<u8> splash(sizeof(SplashHeader), 0);
	u32 width, height;
	if((err = lodepng::decode(splash, width, height, png)) != 0)
	{
		fprintf(stderr, "Failed to decode png file: %s\n", lodepng_error_text(err));
		return false;
	}
	if(width > 65535u || height > 65535u)
	{
		fprintf(stderr, "Error: Pictures with width/height higher than 65535 are not supported!\n");
		return false;
	}

	SplashHeader header;
	memcpy(&header.magic, "SPLA", 4);
	header.width = width & 0xFFFFu;
	header.height = height & 0xFFFFu;

	if(flags & FLAG_ROTATED) rotate(splash, width, height);

	switch(flags & FORMAT_INVALID)
	{
		case FORMAT_RGB565:
			rgba8ToRgb565(splash, width, height, flags & FLAG_SWAPPED);
			break;
		case FORMAT_RGB8:
			rgba8ToRgb8(splash, width, height, flags & FLAG_SWAPPED);
			break;
		case FORMAT_RGBA8:
			if(flags & FLAG_SWAPPED) swapRgba(splash, width, height);
			break;
		default: ;
	}

	if(flags & FLAG_COMPRESSED)
	{
		size_t size = 0;
		void *tmp = lz11_encode(splash.data() + sizeof(SplashHeader), splash.size() - sizeof(SplashHeader), &size);
		if(tmp && size > 5 && size - 5 < splash.size() - sizeof(SplashHeader))
		{
			splash.resize(size + sizeof(SplashHeader) - 5);
			memcpy(splash.data() + sizeof(SplashHeader), (u8*)tmp + 5, size - 5);
		}
		else
		{
			if(tmp)
				fprintf(stderr, "Compression doesn't decrease size.\n");
			else
				fprintf(stderr, "Failed to compress data.\n");
			fprintf(stderr, " Disabling compression...\n");
			flags &= ~FLAG_COMPRESSED;
		}

		free(tmp);
	}

	header.flags = flags;
	memcpy(splash.data(), &header, sizeof(SplashHeader));

	return vectorToFile(splash, outFile);
}
