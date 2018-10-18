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

EXPORT=Holidays.map
TARGET=libHolidays.so
CFLAGS=-c -O2 -fPIC -Wall -fshort-wchar -ffunction-sections -fdata-sections -I../../../CrHack/inc -I../jni/linux
LFLAGS=-o $(TARGET) -shared -fPIC -Wl,--gc-sections $(NOWIDE) -Wl,--version-script=$(EXPORT) -L../../../CrHack/bin/gcc_linux_all
SFLAGS=--strip-unneeded $(TARGET)

SRCS=Holidays.c
OBJS=Holidays.o -lUTILs -lCOREs -lc -lrt -lpthread

all:
	$(CC) $(CFLAGS) $(SRCS)
	$(LD) $(LFLAGS) $(OBJS)
	$(SP) $(SFLAGS)

clean:
	rm -f *.o
	rm -f $(TARGET)
