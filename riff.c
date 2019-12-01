/* ================================ *\
|| riff.c: RIFF Supporting Routines ||
\* ================================ */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <endian.h>
#include "riff.h"
#include "common.h"

size_t try_read(void *ptr, size_t size, size_t nmemb, FILE *fp)
{
	size_t ret=0;
	if((ret = fread(ptr, size, nmemb, fp)) != 0)
	{
		panic("I/O Error");
	}
}

int read_riff_header(FILE *fp, RiffChunkHeader *ck)
{
	uint32_t buf=0;
	fread(&ck->ckID, sizeof(ck->ckID), 1, fp);

}
