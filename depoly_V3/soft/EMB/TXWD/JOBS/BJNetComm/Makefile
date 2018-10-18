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

EXPORT=BJNetComm.map
TARGET=libBJNetComm.so
CFLAGS=-c -O2 -fPIC -Wall -fshort-wchar -ffunction-sections -fdata-sections -I../../../CrHack/inc -I../jni/linux
LFLAGS=-o $(TARGET) -shared -fPIC -Wl,--gc-sections $(NOWIDE) -Wl,--version-script=$(EXPORT) -L../../../CrHack/bin/gcc_linux_all
SFLAGS=--strip-unneeded $(TARGET)

SRCS=BJNetComm.c
OBJS=BJNetComm.o -lUTILs -lCOREs -lc -lrt -lpthread

all:
	$(CC) $(CFLAGS) $(SRCS)
	$(LD) $(LFLAGS) $(OBJS)
	$(SP) $(SFLAGS)
	$(CC) -o test -O2 -Wall -fshort-wchar -D_CR_NO_PRAGMA_LIB_ $(NOWIDE) -L./ test.c -lBJNetComm
	$(SP) test

clean:
	rm -f *.o
	rm -f test
	rm -f $(TARGET)
