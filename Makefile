CC=	clang
# Alternative computation method
ALT=	-DALT
CFLAGS=	-Ofast -march=native -gdwarf-5 -g3 -flto -fPIE -fPIC -pipe -Wall -Wextra -std=c89 -pedantic $(ALT) -D_DEFAULT_SOURCE
OBJS=	adpcm.o codec.o common.o riff.o arg.o
ALL=	codec
all: $(ALL)

codec:	$(OBJS)
	$(CC) $(CFLAGS) -o codec $(OBJS)

countline:
	wc -l *.c *.h Makefile

clean:
	rm -f $(OBJS) $(ALL)
