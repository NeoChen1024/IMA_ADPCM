#include "common.h"

#define MODE_ENCODE	0
#define MODE_DECODE	1
#define MODE_TEST_THROUGH	2

extern FILE *infile;
extern FILE *outfile;
extern int mode;
extern void argparse(int argc, char **argv);
