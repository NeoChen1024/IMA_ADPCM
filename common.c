/* ============================== *\
|| common.c: Some useful routines ||
\* ============================== */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include "common.h"

void panic(char *str)
{
	fputs(str, stderr);
	fputc('\n', stderr);
	exit(100);
}
