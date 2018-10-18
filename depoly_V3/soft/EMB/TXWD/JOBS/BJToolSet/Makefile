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

CFLAGS=-c -O2 -Wall -fshort-wchar -ffunction-sections -fdata-sections -I../../../CrHack/inc
LFLAGS=-Wl,--gc-sections $(NOWIDE) -L../../../CrHack/bin/gcc_linux_all

all:
	$(CC) $(CFLAGS) BJResSig.c BJResEnc.c BJResDec.c BJResLib.c
	$(LD) $(LFLAGS) -o BJResSig BJResSig.o BJResLib.o -lCOREs -lc -lrt -lpthread
	$(SP) BJResSig
	$(LD) $(LFLAGS) -o BJResEnc BJResEnc.o BJResLib.o -lCOREs -lc -lrt -lpthread
	$(SP) BJResEnc
	$(LD) $(LFLAGS) -o BJResDec BJResDec.o BJResLib.o -lCOREs -lc -lrt -lpthread
	$(SP) BJResDec

clean:
	rm -f *.o
	rm -f BJResSig
	rm -f BJResEnc
	rm -f BJResDec
