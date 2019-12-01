#include "common.h"
#include "codec.h"
#include "riff.h"

FILE *infile;
FILE *outfile;
int mode=MODE_ENCODE;

char *helptext =
"Neo_Chen's ADPCM Encoder / Decoder\n"
"Possible Parameters:\n"
"	-i <file>	Input file\n"
"	-o <file>	Output file\n"
"	-m <mode>	Mode (one of \"encode\", \"decode\", and \"test\")\n"
"	-h		Help\n";

void mode_set(char *str)
{
	if(strcmp("encode", str) == 0)
		mode = MODE_ENCODE;
	else if(strcmp("decode", str) == 0)
		mode = MODE_DECODE;
	else if(strcmp("test", str) == 0)
		mode = MODE_TEST_THROUGH;
	else
		panic("Unknown mode");
}

FILE *fp_open(char *fn, char* fmode)
{
	if(strcmp(fn, "-") == 0)	/* Open stdin */
	{
		if(strstr(fmode, "r") != NULL)
			return stdin;
		else if(strstr(fmode, "w") != NULL)
			return stdout;
		else
			return NULL;	/* This shouldn't happen */
	}
	else
	{
		return fopen(fn, fmode);
	}
}


void argparse(int argc, char **argv)
{
	int opt;

	/* Set default value */
	infile=stdin;
	outfile=stdout;

	while((opt = getopt(argc, argv, "hvm:i:o:")) != -1)
	{
		switch(opt)
		{
			case 'i':	/* Input */
				infile = fp_open(optarg, "r");
				break;
			case 'o':	/* Output */
				outfile = fp_open(optarg, "w");
				break;
			case 'm':	/* Mode */
				mode_set(optarg);
				break;
			case 'v':
				verbose++;
				break;
			default:
				fputs("Invaild option\n", stderr);
				/* Fall through as intended */
			case 'h':	/* Help */
				fputs(helptext, stderr);
				exit(0);
		}
	}
}
