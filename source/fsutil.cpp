#include <cstdio>
#include <vector>
#include "types.h"



std::vector<u8> vectorFromFile(const char *const path)
{
	FILE *f = fopen(path, "rb");
	if(!f)
	{
		fprintf(stderr, "Failed to open '%s'.\n", path);
		return std::vector<u8>(0);
	}

	if(fseek(f, 0, SEEK_END))
	{
		fprintf(stderr, "Failed to seek in file.\n");
		fclose(f);
		return std::vector<u8>(0);
	}
	s32 size;
	if((size = ftell(f)) == -1)
	{
		fprintf(stderr, "Failed to get file size.\n");
		fclose(f);
		return std::vector<u8>(0);
	}
	if(fseek(f, 0, SEEK_SET))
	{
		fprintf(stderr, "Failed to seek in file.\n");
		fclose(f);
		return std::vector<u8>(0);
	}

	std::vector<u8> v((u32)size);
	if(fread(v.data(), 1, (u32)size, f) != (u32)size)
	{
		fprintf(stderr, "Failed to read file!\n");
		fclose(f);
		return std::vector<u8>(0);
	}
	fclose(f);

	return v;
}

bool vectorToFile(const std::vector<u8>& v, const char *const path)
{
	FILE *f = fopen(path, "wb");
	if(!f)
	{
		fprintf(stderr, "Failed to open '%s'.\n", path);
		return false;
	}
	if(fwrite(v.data(), 1, v.size(), f) != v.size())
	{
		fprintf(stderr, "Failed to write to file.\n");
		fclose(f);
		return false;
	}
	fclose(f);

	return true;
}
