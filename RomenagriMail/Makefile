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
#  15-Jul-2006, Created makefile, Ver. 0.2.0,
#                 Abhishek Choudhary <hi_pedler>,
#                 choudhary@indicybers.net

#Makefile for Romenagri Transliteration System Ver. 0.1.0 - E-Dakia

#Directory for header files
INCDIR=/usr/include

#Directory for libraries
LIBDIR=/usr/lib

#Directory to link executables into
EXECDIR=/usr/bin

#Directory to install package into
APPPATH=/usr/share/RomenagriMail

#Default extension for executables. Set to .exe under FreeDOS/DJGPP/Cygwin etc.
#EXECEXT=.exe

#Explain the make targets
explain:
	@echo Romenagri User Interface
	@echo GNU GPL V2.0 or later. No Warranty.
	@echo make install   :- install the application
	@echo make uninstall :- uninstall the application

#Create swatantra
dakia:
	@echo -e "#!/bin/bash\nkmdr-executor $(INSTROOT)$(APPPATH)/RomenagriMail.kmdr"  > $@
	chmod 777 dakia

#Install the stuff
install: dakia RomenagriMail.kmdr mailfilter.awk
	install -d $(INSTROOT)$(APPPATH)
	install RomenagriMail.kmdr $(INSTROOT)$(APPPATH)/RomenagriMail.kmdr
	install RomenagriMail.png $(INSTROOT)$(APPPATH)/RomenagriMail.png
	install mailfilter.awk $(INSTROOT)$(APPPATH)/mailfilter.awk
	install dakhelp.pdf $(INSTROOT)$(APPPATH)/dakhelp.pdf
	install RomenagriMail.desktop $(INSTROOT)/usr/share/applications/RomenagriMail.desktop
	install dakia $(INSTROOT)$(EXECDIR)/dakia
	rm dakia

#Install the stuff
uninstall: 
	rm $(INSTROOT)$(EXECDIR)/dakia
	rm $(INSTROOT)$(APPPATH)/RomenagriMail.kmdr
	rm $(INSTROOT)$(APPPATH)/RomenagriMail.png
	rm $(INSTROOT)$(APPPATH)/mailfilter.awk
	rm $(INSTROOT)/usr/share/applications/RomenagriMail.desktop
	rm $(INSTROOT)$(APPPATH)/dakhelp.pdf

