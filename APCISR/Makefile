#Copyright (C) 2006 Abhishek Choudhary
#This file is part of the AP Code for Indian Script Representation.
#
#The AP Code for Indian Script Representation is free software; 
#you can redistribute it and/or modify it under the terms of the 
#GNU General Public License as published by the 
#Free Software Foundation; either version 2 of the License, or 
#(at your option) any later version.
#
#The AP Code for Indian Script Representation is distributed in the hope 
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
#  20-Jan-2006, Created makefile, Ver. 0.2.0,
#                 Abhishek Choudhary <hi_pedler>,
#                 choudhary@indicybers.net

#Makefile for AP Code for Indian Script Representation Ver. 0.2.0

#Targets:
#  make all :- makes library, filters and test
#  make libapcisr :- makes library
#  make filters :- makes filters
#  make test :- makes test
#  make clean :- cleans object-files and library
#  make clean_filt :- cleans filters
#  make clean_test :- cleans test
#  make clean_all :- cleans object-files, filters, test and library
#  make explain :- explain targets
#  make install :- install the headers, libraries and executables

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
        acii2csr.lex

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
         apcisr.h \
         acii2csr.h \
         cisr2asc.h

#List of object files
OBJS=\
      acii2csr.o

#Default target 'all'creates the library, filters and test
all: libapcisr filters

#Target to build the Romenagri library - libapcisr
libapcisr: $(HEADERS) $(OBJS)
	$(LIB) $(LIBFLAGS) libapcisr.a $(OBJS)

#Target for building the filters
filters: $(HEADERS) libapcisr $(LEXSRC)
	$(LEX) $(LEXFLAGS) acii2csr.lex
	$(CC) $(CFLAGS) -o acii2csr$(EXECEXT) lexyy.c -lfl -lapcisr -L ./

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
	rm acii2csr$(EXECEXT)

#Cleanup after compilation under FreeDOS / DJGPP (*.exe)
clean_dos:
	rm *.exe

#Cleanup after 'all' make
clean_all: clean clean_filt

#Explain the make targets
explain:
	@echo AP Code for Indian Script Representation Ver. 0.2.0
	@echo GNU GPL V2.0 or later. No Warranty.
	@echo make all :- makes library, filters and test
	@echo make libapcisr :- makes library
	@echo make filters :- makes filters
	@echo make clean :- cleans object-files and library
	@echo make clean_filt :- cleans filters
	@echo make clean_all :- cleans object-files, filters, test and library
	@echo make explain :- explain targets
	@echo make install :- install the executables and library
	@echo make uninstall :- uninstall the executables and library

#Install the stuff
install: 
	install -d $(INSTROOT)/usr/share/APCISR
	install devnagri.f08 $(INSTROOT)/usr/share/APCISR/devnagri.f08
	install devnagri.f16 $(INSTROOT)/usr/share/APCISR/devnagri.f16
	install libapcisr.a $(INSTROOT)$(LIBDIR)/libapcisr.a
	install acii2csr.h $(INSTROOT)$(INCDIR)/acii2csr.h
	install apcisr.h $(INSTROOT)$(INCDIR)/apcisr.h
	install cisr2asc.h $(INSTROOT)$(INCDIR)/cisr2asc.h
	install acii2csr$(EXECEXT) $(INSTROOT)$(EXECDIR)/acii2csr$(EXECEXT)

#Install the stuff
uninstall: 
	rm $(INSTROOT)/usr/share/APCISR/devnagri.f08
	rm $(INSTROOT)/usr/share/APCISR/devnagri.f16
	rm $(INSTROOT)$(LIBDIR)/libapcisr.a
	rm $(INSTROOT)$(INCDIR)/acii2csr.h
	rm $(INSTROOT)$(INCDIR)/apcisr.h
	rm $(INSTROOT)$(INCDIR)/cisr2asc.h
	rm $(INSTROOT)$(EXECDIR)/acii2csr$(EXECEXT)

#Target to make dependencies
depend dep: libapcisr
