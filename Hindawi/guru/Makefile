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

#Makefile for Hindawi Indic Programming System Ver. 0.2.0 - Shaili Guru

EXECDIR=/usr/bin

all:
	cat h2c.uhin | iconv -f utf-8 -t utf-16 | uni2acii | acii2cf > h2c.lex
	flex -olexyy.c -8 h2c.lex
	gcc -oh2c lexyy.c -lfl
	cat c2h.uhin | iconv -f utf-8 -t utf-16 | uni2acii | acii2cf > c2h.lex
	flex -olexyy.c -8 h2c.lex
	gcc -oc2h lexyy.c -lfl

clean_all: clean

clean:
	rm lexyy.c
	rm *.lex
	rm h2c
	rm c2h

install:
	install h2c $(INSTROOT)$(EXECDIR)/h2c
	install c2h $(INSTROOT)$(EXECDIR)/c2h
	install gurucc $(INSTROOT)$(EXECDIR)/gurucc

uninstall:
	rm $(INSTROOT)$(EXECDIR)/h2c
	rm $(INSTROOT)$(EXECDIR)/c2h
	rm $(INSTROOT)$(EXECDIR)/gurucc
