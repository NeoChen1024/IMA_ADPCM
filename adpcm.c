/* ========================================================================== *\
||			IMA ADPCM Codec Implementation in C		      ||
||                                 Neo_Chen                                   ||
\* ========================================================================== */

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
#include "common.h"


static int8_t i_table[16] = {-1, -1, -1, -1, 2, 4, 6, 8,
		-1, -1, -1, -1, 2, 4, 6, 8};

/* quantizer lookup table */
static int16_t ss_table[89] = {7, 8, 9, 10, 11, 12, 13, 14,
	16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60,
	66, 73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209,
	230, 253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658,
	724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878,
	2066, 2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871,
	5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635,
	13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
	32767};

static void clamp(int *value, int min, int max)
{
	if(*value > max)
		*value = max;
	else if(*value < min)
		*value = min;
}

static int diffcalc(uint8_t s, int ss)
{
	int diff=0;
	int smp = s & 0x7; /* Strip sign */

	/*
	Calculate the difference, the original IMA document, "Recommended Practices
	for Enhancing Digital Audio Compatibility in Multimedia Systems", says:

		Difference = (OriginalSample + 0.5) * StepSize / 4

	But, that causes a significant DC offset, so I changed it to:

		Difference = (OriginalSample + 0.25) * StepSize / 4

	Then, after a brief moment, I looked FFmpeg's encoding code......
	I just realized that 0.5 doesn't need to be there......, so,
	Here's our working formula:

		Difference = OriginalSample * StepSize / 4
	
	Sometimes, document lies......
	*/
	diff = smp * ss >> 2;
	if(s & 0x8)	/* Is negative */
		return -diff;
	else
		return diff;
}

uint8_t adpcm_encode_sample(int16_t in, STATE *st)
{
	uint8_t s=0;	/* The 4 Bit ADPCM sample to return */
	int diff = in - st->ps;

	if(diff < 0)
		s = 1<<3;
	diff = abs(diff);
	/* Calculate the sample:
		Sample = 4 * Difference / StepSize
	*/
	s |= ((diff<<2) / st->ss) & 0x7;
	st->ps += diffcalc(s, st->ss);	/* Update Predicted Sample */
	clamp(&st->ps, -32768, 32767);
	st->i += i_table[s];
	clamp(&st->i, 0, 88);
	st->ss = ss_table[st->i];
	return s;
}

int16_t adpcm_decode_sample(uint8_t s, STATE *st)
{
	int pcm=0; /* 16 Bit signed PCM to return */
	pcm = st->ps += diffcalc(s, st->ss);	/* Use ps to store current sample */
	clamp(&pcm, -32768, 32767);
	st->i += i_table[s];
	clamp(&st->i, 0, 88);
	st->ss = ss_table[st->i];
	return (int16_t)pcm;
}

STATE *state_init(int predsmp, int index, int stepsize)
{
	STATE *state = calloc(1, sizeof(struct codec_state));
	state->ps = predsmp;
	state->i = index;
	state->ss = stepsize;

	return state;
}

void state_free(STATE *st)
{
	free(st);
}
