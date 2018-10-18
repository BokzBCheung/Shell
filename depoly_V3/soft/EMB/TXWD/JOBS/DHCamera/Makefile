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

EXPORT=DHCamera.map
TARGET=libDHCamera.so
CFLAGS=-c -O2 -fPIC -Wall -fshort-wchar -ffunction-sections -fdata-sections -I../../../CrHack/inc
LFLAGS=-o $(TARGET) -shared -fPIC -Wl,--gc-sections $(NOWIDE) -Wl,--version-script=$(EXPORT) -L../../../CrHack/bin/gcc_linux_all
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
