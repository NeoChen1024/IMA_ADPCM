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


int8_t i_table[16] = {-1, -1, -1, -1, 2, 4, 6, 8,
		-1, -1, -1, -1, 2, 4, 6, 8};

/* quantizer lookup table */
int16_t ss_table[89] = {7, 8, 9, 10, 11, 12, 13, 14,
	16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60,
	66, 73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209,
	230, 253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658,
	724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878,
	2066, 2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871,
	5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635,
	13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
	32767};

struct codec_state state =
{
	.ps = 0,
	.i = 0,
	.ss = 7
};

void clip(int *value, int min, int max)
{
	if(*value > max)
		*value = max;
	else if(*value < min)
		*value = min;
}

uint8_t adpcm_encode(int16_t in, struct codec_state *st)
{
	uint8_t s=0;	/* The 4 Bit ADPCM sample to return */
	int diff = in - st->ps;

#ifndef FP
	int i=0;
	uint8_t mask=4;
	int temp = st->ss;
#endif

	if(diff < 0)
	{
		s = 1<<3;
		diff = -diff;
	}

	/* Calculate the sample */
#ifdef FP
	s = (uint8_t)((float)diff / (float)st->ss);
#else
	for(i=0; i < 3; i++)
	{
		if(diff >= temp)
		{
			s |= mask;
			diff -= temp;
		}
		temp >>= 1;
		mask >>= 1;
	}
#endif

	/* Difference = (OriginalSample + 0.5) * StepSize / 4 */
#ifdef FP
	diff = (int)(((float)in + 0.5) * (float)st->ss / 4.0);
#else
	diff = 0;
	if(s & (1<<2))	diff += st->ss;
	if(s & (1<<1))	diff += st->ss >> 1;
	if(s & 1)	diff += st->ss >> 2;
	diff += st->ss >> 3;
#endif
	if(s & 0x8)	/* Test for sign bit */
		diff = -diff;
	/* Update Predicted Sample */
	st->ps += diff;
	clip(&st->ps, -32768, 32767);
	st->i += i_table[s];
	clip(&st->i, 0, 88);
	st->ss = ss_table[st->i];
	return s;
}

int16_t adpcm_decode(uint8_t s, struct codec_state *st)
{
	int pcm=0; /* 16 Bit signed PCM to return */
	int diff=0;
	/* Difference = (OriginalSample + 0.5) * StepSize / 4 */
	diff = (s*st->ss >>2) + (s*st->ss >>3);
	if(s & 0x8)	/* Test for sign bit */
		diff = -diff;
	pcm += diff;
	clip(&pcm, -32768, 32767);
	st->i += i_table[s];
	clip(&st->i, 0, 88);
	st->ss = ss_table[st->i];
	return (int16_t)pcm;
}

void state_reset(struct codec_state *st)
{
	st->ps = 0;
	st->i = 0;
	st->ss = 7;
}
