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

TARGET=BJNetServ
CFLAGS=-c -O2 -Wall -fshort-wchar -ffunction-sections -fdata-sections -I../../../CrHack/inc
LFLAGS=-o $(TARGET) -Wl,--gc-sections $(NOWIDE) -L../../../CrHack/bin/gcc_linux_all
SFLAGS=$(TARGET)

SRCS=BJNetServ.c
OBJS=BJNetServ.o -lUTILs -lCOREs -lc -lrt -lpthread

all:
	$(CC) $(CFLAGS) $(SRCS)
	$(LD) $(LFLAGS) $(OBJS)
	$(SP) $(SFLAGS)

clean:
	rm -f *.o
	rm -f $(TARGET)
