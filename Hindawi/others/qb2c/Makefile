# Modified by Abhishek Choudhary for distribution with Hindawi
# The license terms remain same as that for QB2C

#Directory for header files
INCDIR=/usr/include

#Directory for libraries
LIBDIR=/usr/lib

#Directory to install executables into
EXECDIR=/usr/bin

all:    bcpp qb2c calib x11

clean_all:	
	rm -f  *.o libqbX11.a bcc brun run bhelp
	rm bcpp
	rm qb2c

bcpp:	bcpp.c
	gcc -ansi -O2 -o bcpp bcpp.c

qb2c:	qb2c.c
	gcc -O2 -o qb2c qb2c.c -lm

calib:	calib.c
	gcc -ansi -o calib calib.c 

x11:	x11int.c rotated.c gifencode.c gifdecode.c pickpalette.c
	gcc -c -ansi -w -O2 x11int.c rotated.c gifencode.c gifdecode.c pickpalette.c
	ar -cr libqbX11.a x11int.o rotated.o gifencode.o gifdecode.o pickpalette.o

clean:	
	rm -f *.o tmp-*.b2c

install:	qb2c bcpp calib
	install qb2c $(INSTROOT)$(EXECDIR)/qb2c
	install bcpp $(INSTROOT)$(EXECDIR)/bcpp
	install calib $(INSTROOT)$(EXECDIR)/calib
	install libqbX11.a $(INSTROOT)$(LIBDIR)/libqbX11.a 

uninstall:
	rm $(INSTROOT)$(EXECDIR)/qb2c
	rm $(INSTROOT)$(EXECDIR)/bcpp
	rm $(INSTROOT)$(EXECDIR)/calib
	rm $(INSTROOT)$(LIBDIR)/libqbX11.a 
