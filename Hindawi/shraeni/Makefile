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

#Makefile for Hindawi Indic Programming System Ver. 0.2.0 - Shaili Shraeni

EXECDIR=/usr/bin

all:
	cat h2cpp.uhin | iconv -f utf-8 -t utf-16 | uni2acii | acii2cf > h2cpp.lex
	flex -olexyy.c -8 h2cpp.lex
	gcc -oh2cpp lexyy.c -lfl
	cat cpp2h.uhin | iconv -f utf-8 -t utf-16 | uni2acii | acii2cf > cpp2h.lex
	flex -olexyy.c -8 cpp2h.lex
	gcc -ocpp2h lexyy.c -lfl

clean_all: clean

clean:
	rm lexyy.c
	rm *.lex
	rm h2cpp
	rm cpp2h

install:
	install h2cpp $(INSTROOT)$(EXECDIR)/h2cpp
	install cpp2h $(INSTROOT)$(EXECDIR)/cpp2h
	install shraenicc $(INSTROOT)$(EXECDIR)/shraenicc

uninstall:
	rm $(INSTROOT)$(EXECDIR)/h2cpp
	rm $(INSTROOT)$(EXECDIR)/cpp2h
	rm $(INSTROOT)$(EXECDIR)/shraenicc
