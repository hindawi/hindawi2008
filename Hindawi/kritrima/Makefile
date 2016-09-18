#Copyright (C) 2006 Abhishek Choudhary
#This file is part of the Hindawi Indic Programming System.
#
#The Hindawi Indic Programming System is free software; 
#you can redistribute it and/or modify it under the terms of the 
#GNU General Public License as published by the 
#Free Software Foundation; either version 2 of the License, or 
#(at your option) any later version.
#
#The Hindawi Indic Programming System is distributed in the hope 
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
#  31-Jul-2006, Created makefile, Ver. 0.2.0,
#                 Abhishek Choudhary <hi_pedler>,
#                 choudhary@indicybers.net

#Makefile for Hindawi Indic Programming System Ver. 0.2.0 - Shaili Kritrima

EXECDIR=/usr/bin

all:
	cat h2j.uhin | iconv -f utf-8 -t utf-16 | uni2acii | acii2cf > h2j.lex
	flex -olexyy.c -8 h2j.lex
	gcc -oh2j lexyy.c -lfl
	cat j2h.uhin | iconv -f utf-8 -t utf-16 | uni2acii | acii2cf > j2h.lex
	flex -olexyy.c -8 j2h.lex
	gcc -oj2h lexyy.c -lfl

clean_all: clean

clean:
	rm lexyy.c
	rm *.lex
	rm h2j
	rm j2h

install:
	install h2j $(INSTROOT)$(EXECDIR)/h2j
	install j2h $(INSTROOT)$(EXECDIR)/j2h
	install kritrimacc $(INSTROOT)$(EXECDIR)/kritrimacc
	install fixjava.awk $(INSTROOT)$(EXECDIR)/fixjava.awk

uninstall:
	rm $(INSTROOT)$(EXECDIR)/h2j
	rm $(INSTROOT)$(EXECDIR)/j2h
	rm $(INSTROOT)$(EXECDIR)/kritrimacc
	rm $(INSTROOT)$(EXECDIR)/fixjava.awk
