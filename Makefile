# Cygwin/Unix
CFLAGS=-g -Wall

# Uncomment the following two lines for MingW
#CFLAGS=-g -Wall -mconsole -mno-cygwin
#LDFLAGS=-g -Wall -mconsole -mno-cygwin

all: chmdeco

chmdeco : chmdeco.o hhp.o system.o system.o strings.o convert.o common.o windows.o sitemap.o hhc.o hhk.o misc.o

clean:
	rm -f chmdeco chmdeco.exe core *.stackdump *.o
