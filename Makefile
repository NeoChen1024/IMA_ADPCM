CC=	clang
# Alternative computation method
ALT=	-DALT
CFLAGS=	-O2 -march=native -gdwarf-5 -gfull -flto -fPIE -fPIC -pipe -Wall -Wextra -ansi -std=c99 -pedantic $(ALT)
OBJS=	adpcm.o rawencode.o
ALL=	adpcm.o	rawencode
all: $(ALL)

rawencode:	$(OBJS)
	$(CC) $(CFLAGS) -o rawencode $(OBJS)

clean:
	rm -f $(LIBOBJS) $(ALL)
