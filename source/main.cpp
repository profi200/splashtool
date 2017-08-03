#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <getopt.h>
#include "types.h"
#include "splash.h"



static void help(void)
{
	printf("splashtool " VERS_STRING " by profi200\n"
	        "Usage: splashtool [OPTION...] [in png file] [out splash file]\n\n"
	        "  -f --format=format   Format. Can be RGB565, RGB8 or RGBA8\n"
	        "  -c --nocompress      Optional. Do not compress data\n"
	        "  -r --rotate          Optional. Rotate 90 degrees CW\n"
	        "  -s --swap            Optional. Swap color components\n"
	        "  -h --help            Give this help list\n"
	        "  -v --version         Print program version\n\n");

	exit(1);
}

int main(int argc, char *const argv[])
{
	static const struct option long_options[] =
	{{"format",     required_argument, 0, 'f'},
	 {"nocompress",       no_argument, 0, 'c'},
	 {"rotate",           no_argument, 0, 'r'},
	 {"swap",             no_argument, 0, 's'},
	 {"help",             no_argument, 0, 'h'},
	 {"version",          no_argument, 0, 'v'},
	 {0,            0,                 0,   0}
	};

	u32 flags = FLAG_COMPRESSED | FORMAT_INVALID;
	while(1)
	{
		int c = getopt_long(argc, argv, "f:crshv", long_options, 0);
		if(c == -1) break;

		switch(c)
		{
			case 'f':
				flags = flags & ~FORMAT_INVALID;
				if(!strncmp(optarg, "RGB565", 6)) flags |= FORMAT_RGB565;
				else if(!strncmp(optarg, "RGB8", 4)) flags |= FORMAT_RGB8;
				else if(!strncmp(optarg, "RGBA8", 5)) flags |= FORMAT_RGBA8;
				else help();
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
			case 'h':
				help();
				break;
			case 'v':
				printf("splashtool " VERS_STRING "\n");
				exit(0);
				break;
			case '?':
			default:
				help();
		}
	}

	if(argc - optind < 2 || argc - optind > 2) help();
	const char *inFile = argv[optind];
	const char *outFile = argv[optind + 1];


	if((flags & FORMAT_INVALID) == FORMAT_INVALID)
	{
		fprintf(stderr, "Warning: No format specified. Assuming RGB565.\n");
		flags &= ~FORMAT_INVALID;
	}

	int res;
	try
	{
		res = (pngToSplash(flags, inFile, outFile) ? 0 : 2);
	}
	catch(const std::exception& e)
	{
		fprintf(stderr, "An exception occured: what(): '%s'\n", e.what());
		res = 3;
	}
	catch(...)
	{
		fprintf(stderr, "Unknown exception. Exiting...\n");
		res = 4;
	}

	return res;
}
