#include <argp.h>
#include <cstdio>
#include <cstring>
#include "types.h"
#include "splash.h"


static u32 flags = FLAG_COMPRESSED | FORMAT_INVALID;
static const char *inFile = nullptr;
static const char *outFile = nullptr;


const char *argp_program_version = "splashtool " VER_STRING;
static const char *doc           = "A tool to create splashscreen files";

static const struct argp_option options[] =
{ {"format",     'f', "format",                   0, "Format. Can be RGB565, RGB8 or RGBA8", 1},
  {"nocompress", 'c',        0, OPTION_ARG_OPTIONAL, "Optional. Do not compress data",       1},
  {"rotate",     'r',        0, OPTION_ARG_OPTIONAL, "Optional. Rotate 90 degrees CW",       1},
  {"swap",       's',        0, OPTION_ARG_OPTIONAL, "Optional. Swap color components",      1},
  {0, 0, 0, 0, 0, 0}
};

static const char *args_doc = "[in png file] [out splash file]";


static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	switch(key)
	{
		case 'f':
			flags = flags & ~FORMAT_INVALID;
			if(!strncmp(arg, "RGB565", 6)) flags |= FORMAT_RGB565;
			else if(!strncmp(arg, "RGB8", 4)) flags |= FORMAT_RGB8;
			else if(!strncmp(arg, "RGBA8", 5)) flags |= FORMAT_RGBA8;
			else argp_usage(state);
			break;
		case 'c':
			flags = flags & ~FLAG_COMPRESSED;
			break;
		case 'r':
			flags |= FLAG_ROTATED;
			break;
		case 's':
			flags |= FLAG_SWAPPED;
			break;
		case ARGP_KEY_ARG:
			if(state->arg_num > 1) argp_usage(state);
			if(state->arg_num == 0) inFile = arg;
			else if(state->arg_num == 1) outFile = arg;
			break;
		case ARGP_KEY_END:
			if(state->arg_num < 2) argp_usage(state);
			break;
		default:
			return ARGP_ERR_UNKNOWN;
    }

	return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};


int main(int argc, char **argv)
{
	argp_parse(&argp, argc, argv, 0, 0, 0);

	if((flags & FORMAT_INVALID) == FORMAT_INVALID)
	{
		fprintf(stderr, "Warning: No format specified. Assuming RGB565.\n");
		flags &= ~FORMAT_INVALID;
	}

	return (pngToSplash(flags, inFile, outFile) ? 0 : 1);
}
