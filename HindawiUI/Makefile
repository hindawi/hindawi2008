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

#Makefile for Hindawi Indic Programming System Ver. 0.2.0 - Laghu IDE

#Directory for header files
INCDIR=/usr/include

#Directory for libraries
LIBDIR=/usr/lib

#Directory to link executables into
EXECDIR=/usr/bin

#Directory to install package into
APPPATH=/usr/share/HindawiUI

#Default extension for executables. Set to .exe under FreeDOS/DJGPP/Cygwin etc.
#EXECEXT=.exe

#Explain the make targets
explain:
	@echo laghunagri User Interface
	@echo GNU GPL V2.0 or later. No Warranty.
	@echo make install   :- install the application
	@echo make uninstall :- uninstall the application

#Create swatantra
laghu:
	@echo -e "#!/bin/bash\nkmdr-executor $(INSTROOT)$(APPPATH)/HindawiUI.kmdr"  > $@
	chmod 777 laghu

#Install the stuff
install: HindawiUI.kmdr laghu
	install -d $(INSTROOT)$(APPPATH)
	install HindawiUI.kmdr $(INSTROOT)$(APPPATH)/HindawiUI.kmdr
	install hinlogo.png $(INSTROOT)$(APPPATH)/hinlogo.png
	install hinhelp.pdf $(INSTROOT)$(APPPATH)/hinhelp.pdf
	install HindawiUI.desktop $(INSTROOT)/usr/share/applications/HindawiUI.desktop
	install laghu $(INSTROOT)$(EXECDIR)/laghu
	rm laghu

#Install the stuff
uninstall: 
	rm $(INSTROOT)$(EXECDIR)/laghu
	rm $(INSTROOT)$(APPPATH)/HindawiUI.kmdr
	rm $(INSTROOT)$(APPPATH)/hinlogo.png
	rm $(INSTROOT)/usr/share/applications/HindawiUI.desktop
	rm $(INSTROOT)$(APPPATH)/hinhelp.pdf
