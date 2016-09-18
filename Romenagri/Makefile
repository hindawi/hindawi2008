#Copyright (C) 2006 Abhishek Choudhary
#This file is part of the Romenagri Transliteration System.
#
#The Romenagri Transliteration System is free software; 
#you can redistribute it and/or modify it under the terms of the 
#GNU General Public License as published by the 
#Free Software Foundation; either version 2 of the License, or 
#(at your option) any later version.
#
#The Romenagri Transliteration System is distributed in the hope 
#that it will be useful, but WITHOUT ANY WARRANTY; without 
#even the implied warranty of MERCHANTABILITY or FITNESS FOR 
#A PARTICULAR PURPOSE. See the GNU General Public License for 
#more details.
#
#You should have received a copy of the GNU General Public
#License along with this file; see the file COPYING. If
#not, write to the Free Software Foundation,
#51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#
#Modifications: (Please maintain in reverse chronological order)
#
#  dd-mmm-yyyy, Nature of change,
#                 Changed by <alias>,
#                 email@server.tld
#
#  18-Jan-2006, Added unicode and clean_unic targets,
#                 Abhishek Choudhary <hi_pedler>,
#                 choudhary@indicybers.net
#
#  15-Jan-2006, Created makefile, Ver. 0.2.0,
#                 Abhishek Choudhary <hi_pedler>,
#                 choudhary@indicybers.net

#Makefile for Romenagri Transliteration System Ver. 0.1.0

#Targets:
#  make all :- makes library, filters and test
#  make librmnagri :- makes library
#  make filters :- makes filters
#  make test :- makes test
#  make clean :- cleans object-files and library
#  make clean_filt :- cleans filters
#  make clean_test :- cleans test
#  make clean_all :- cleans object-files, filters, test and library
#  make explain :- explain targets
#  make install :- install the headers, libraries and executables
#  make uninstall :- install the headers, libraries and executables

#Compiler to be used
CC=gcc

#Compiler flags
CFLAGS=-Wall -Wno-unused

#Archiver to be used
LIB=ar

#Archiver options
LIBFLAGS=-cru

#Lex compiler to be used
LEX=flex

#Lex compiler options
LEXFLAGS=-8 -olexyy.c

#List of Lex filters to compile
LEXSRC=\
        acii2cf.lex \
        acii2pcf.lex \
        acii2rmn.lex \
        acii2hin.lex \
	fixuninum.lex\
        rmn2acii.lex

#Directory for header files
INCDIR=/usr/include

#Directory for libraries
LIBDIR=/usr/lib

#Directory to install executables into
EXECDIR=/usr/bin

#Default extension for executables. Set to .exe under FreeDOS/DJGPP/Cygwin etc.
#EXECEXT=.exe

#Test executable
TESTEX=testme

#List of header files
HEADERS=\
         acii.h \
         rmn2acii.h \
         acii2rmn.h \
         acii2hin.h \
         stack.h

#List of object files
OBJS=\
      rmn2acii.o \
      acii2rmn.o \
      acii2hin.o \
      stack.o 

#Default target 'all'creates the library, filters and test
all: librmnagri filters test unicode

#Target to build the Romenagri library - librmnagri
librmnagri: $(HEADERS) $(OBJS)
	$(LIB) $(LIBFLAGS) librmnagri.a $(OBJS)

#Target for building the filters
filters: rmn2acii.h acii2rmn.h librmnagri $(LEXSRC)
	$(LEX) $(LEXFLAGS) acii2cf.lex
	$(CC) $(CFLAGS) -o acii2cf$(EXECEXT) lexyy.c -lfl -lrmnagri -L ./
	$(LEX) $(LEXFLAGS) acii2pcf.lex
	$(CC) $(CFLAGS) -o acii2pcf$(EXECEXT) lexyy.c -lfl -lrmnagri -L ./
	$(LEX) $(LEXFLAGS) acii2hin.lex
	$(CC) $(CFLAGS) -o acii2hin$(EXECEXT) lexyy.c -lfl -lrmnagri -L ./
	$(LEX) $(LEXFLAGS) acii2rmn.lex
	$(CC) $(CFLAGS) -o acii2rmn$(EXECEXT) lexyy.c -lfl -lrmnagri -L ./
	$(LEX) $(LEXFLAGS) fixuninum.lex
	$(CC) $(CFLAGS) -o fixuninum$(EXECEXT) lexyy.c -lfl -lrmnagri -L ./
	$(LEX) $(LEXFLAGS) rmn2acii.lex
	$(CC) $(CFLAGS) -o rmn2acii$(EXECEXT) lexyy.c -lfl -lrmnagri -L ./

#Target for the Romenagri library tester program
test: rmn2acii.h acii2rmn.h librmnagri main.o 
	$(CC) -o $(TESTEX)$(EXECEXT) main.o -lrmnagri -L ./

#Default C compilation 
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

#Cleanup after library make
clean:
	rm *.o
	rm *.a

#Cleanup with filters
clean_filt:
	rm lexyy.c
	rm acii2cf$(EXECEXT)
	rm acii2pcf$(EXECEXT)
	rm acii2hin$(EXECEXT)
	rm acii2rmn$(EXECEXT)
	rm fixuninum$(EXECEXT)
	rm rmn2acii$(EXECEXT)

#Cleanup after test make
clean_test:
	rm $(TESTEX)$(EXECEXT)

#Cleanup after compilation under FreeDOS / DJGPP (*.exe)
clean_dos:
	rm *.exe

#Cleanup after 'all' make
clean_all: clean clean_test clean_filt clean_unic

#Explain the make targets
explain:
	@echo Romenagri Transliteration System Ver. 0.2.0
	@echo GNU GPL V2.0 or later. No Warranty.
	@echo make all :- makes library, filters and test
	@echo make librmnagri :- makes library
	@echo make filters :- makes filters
	@echo make test :- makes test
	@echo make clean :- cleans object-files and library
	@echo make clean_filt :- cleans filters
	@echo make clean_test :- cleans test
	@echo make clean_all :- cleans object-files, filters, test and library
	@echo make explain :- explain targets
	@echo make install :- install the executables and library
	@echo make unicode :- make UNICODE - ISCII converters
	@echo make clean_unic :- clean UNICODE filters

#Install the stuff
install: 
	install librmnagri.a $(INSTROOT)$(LIBDIR)/librmnagri.a
	install acii.h $(INSTROOT)$(INCDIR)/acii.h
	install rmn2acii.h $(INSTROOT)$(INCDIR)/rmn2acii.h
	install acii2rmn.h $(INSTROOT)$(INCDIR)/acii2rmn.h
	install acii2hin.h $(INSTROOT)$(INCDIR)/acii2hin.h
	install aciihin.h $(INSTROOT)$(INCDIR)/aciihin.h
	install stack.h $(INSTROOT)$(INCDIR)/stack.h
	install unicode.h $(INSTROOT)$(INCDIR)/unicode.h
	install rmn2acii$(EXECEXT) $(INSTROOT)$(EXECDIR)/rmn2acii$(EXECEXT)
	install acii2rmn$(EXECEXT) $(INSTROOT)$(EXECDIR)/acii2rmn$(EXECEXT)
	install acii2cf$(EXECEXT) $(INSTROOT)$(EXECDIR)/acii2cf$(EXECEXT)
	install acii2pcf$(EXECEXT) $(INSTROOT)$(EXECDIR)/acii2pcf$(EXECEXT)
	install acii2hin$(EXECEXT) $(INSTROOT)$(EXECDIR)/acii2hin$(EXECEXT)
	install acii2uni$(ECECEXT) $(INSTROOT)$(EXECDIR)/acii2uni$(ECECEXT)
	install fixuninum$(EXECEXT) $(INSTROOT)$(EXECDIR)/fixuninum$(EXECEXT)
	install uni2acii$(EXECEXT) $(INSTROOT)$(EXECDIR)/uni2acii$(EXECEXT)

#Uninstall the stuff
uninstall: 
	rm $(INSTROOT)$(LIBDIR)/librmnagri.a
	rm $(INSTROOT)$(INCDIR)/acii.h
	rm $(INSTROOT)$(INCDIR)/rmn2acii.h
	rm $(INSTROOT)$(INCDIR)/acii2rmn.h
	rm $(INSTROOT)$(INCDIR)/acii2hin.h
	rm $(INSTROOT)$(INCDIR)/aciihin.h
	rm $(INSTROOT)$(INCDIR)/stack.h
	rm $(INSTROOT)$(INCDIR)/unicode.h
	rm $(INSTROOT)$(EXECDIR)/rmn2acii$(EXECEXT)
	rm $(INSTROOT)$(EXECDIR)/acii2rmn$(EXECEXT)
	rm $(INSTROOT)$(EXECDIR)/acii2cf$(EXECEXT)
	rm $(INSTROOT)$(EXECDIR)/acii2pcf$(EXECEXT)
	rm $(INSTROOT)$(EXECDIR)/acii2hin$(EXECEXT)
	rm $(INSTROOT)$(EXECDIR)/acii2uni$(ECECEXT)
	rm $(INSTROOT)$(EXECDIR)/fixuninum$(EXECEXT)
	rm $(INSTROOT)$(EXECDIR)/uni2acii$(EXECEXT)

#Target to make UNICODE <-> ISCII converters for Devnagri
unicode: unicode.h acii2uni.c uni2acii.c
	$(CC) $(CFLAGS) -o acii2uni$(EXECEXT) acii2uni.c
	$(CC) $(CFLAGS) -o uni2acii$(EXECEXT) uni2acii.c

#Cleanup UNICODE converters
clean_unic:
	rm acii2uni$(EXECEXT)
	rm uni2acii$(EXECEXT)

#Target to make dependencies
depend dep: librmnagri
