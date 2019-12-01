/* ============ *\
|| testencode.c ||
\* ============ */

/* ========================================================================== *\
||   This is free and unencumbered software released into the public domain.  ||
||									      ||
||   Anyone is free to copy, modify, publish, use, compile, sell, or	      ||
||   distribute this software, either in source code form or as a compiled    ||
||   binary, for any purpose, commercial or non-commercial, and by any	      ||
||   means.								      ||
||									      ||
||   In jurisdictions that recognize copyright laws, the author or authors    ||
||   of this software dedicate any and all copyright interest in the	      ||
||   software to the public domain. We make this dedication for the benefit   ||
||   of the public at large and to the detriment of our heirs and	      ||
||   successors. We intend this dedication to be an overt act of	      ||
||   relinquishment in perpetuity of all present and future rights to this    ||
||   software under copyright law.					      ||
||									      ||
||   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,	      ||
||   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF       ||
||   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   ||
||   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR        ||
||   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,    ||
||   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR    ||
||   OTHER DEALINGS IN THE SOFTWARE.					      ||
||									      ||
||   For more information, please refer to <http://unlicense.org/>            ||
\* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include "adpcm.h"
#include "riff.h"
#include "common.h"
#include "codec.h"

int verbose=0;

int main(int argc, char **argv)
{
	int16_t pcmin = 0;
	int16_t pcmout=0;
	uint8_t adsmp = 0;
	size_t count=0;
	STATE* encoder_state = state_init(0, 0, 7);
	STATE* decoder_state = state_init(0, 0, 7);

	argparse(argc, argv);

	switch(mode)
	{
		case MODE_TEST_THROUGH:
			while(fread(&pcmin, sizeof(pcmin), 1, infile) != 0)
			{
				pcmout = adpcm_decode_sample(adpcm_encode_sample(pcmin, encoder_state), decoder_state);
				if(verbose >= 2)
					fprintf(stderr, "PCM IN = %10hd, PCM OUT = %10hd\n", pcmin, pcmout);
				fwrite(&pcmout, sizeof(pcmout), 1, outfile);
				count++;
			}
			break;
		case MODE_DECODE:
			while(fread(&adsmp, sizeof(adsmp), 1, infile) != 0)
			{
				pcmout = adpcm_decode_sample(adsmp & 0x7, decoder_state);
				fwrite(&pcmout, sizeof(pcmout), 1, outfile);
				pcmout = adpcm_decode_sample(adsmp >> 4, decoder_state);
				fwrite(&pcmout, sizeof(pcmout), 1, outfile);
				count++;
			}
			break;
		case MODE_ENCODE:
			panic("Not Implemented");
	}

	return 0;
}
