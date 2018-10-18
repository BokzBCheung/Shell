ifndef CROSS
CC=gcc
LD=gcc
SP=strip
else
CC=$(CROSS)gcc
LD=$(CROSS)gcc
SP=$(CROSS)strip
endif
ifndef ARM32
NOWIDE=
else
NOWIDE=-Wl,--no-wchar-size-warning
endif

TARGET=DHCamera
CFLAGS=-c -O2 -fPIC -Wall -fshort-wchar -I../../../CrHack/inc -DDHTEST
LFLAGS=-o $(TARGET) -fPIC -Wl,--gc-sections $(NOWIDE) -L../../../CrHack/bin/gcc_linux_all
SFLAGS=--strip-unneeded $(TARGET)

SRCS=DHCamera.c
OBJS=DHCamera.o -lUTILs -lCOREs -lc -lm -lrt -lpthread

all:
	$(CC) $(CFLAGS) $(SRCS)
	$(LD) $(LFLAGS) $(OBJS)
	$(SP) $(SFLAGS)

clean:
	rm -f *.o
	rm -f $(TARGET)
