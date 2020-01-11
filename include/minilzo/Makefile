#
# a very simple Makefile for miniLZO
#

PROGRAM = libminilzo.so
SOURCES = minilzo.c

default:
	gcc -fPIC -g -c -Wall $(SOURCES)
	gcc -shared -Wl,-soname,$(PROGRAM) *.o -o $(PROGRAM)

install:
	cp $(PROGRAM) /usr/local/lib
	mkdir -p /usr/local/include/minilzo
	cp *.h /usr/local/include/minilzo

clean:
	rm -f $(PROGRAM) $(PROGRAM).exe $(PROGRAM).map $(PROGRAM).tds
	rm -f *.err *.o *.obj

.PHONY: default clean

