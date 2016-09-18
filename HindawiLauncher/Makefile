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

#Makefile for Hindawi Indic Programming System Ver. 0.2.0 - Swatantra Applications

#Directory for header files
INCDIR=/usr/include

#Directory for libraries
LIBDIR=/usr/lib

#Directory to link executables into
EXECDIR=/usr/bin

#Directory to install package into
APPPATH=/usr/share/HindawiLauncher

#Default extension for executables. Set to .exe under FreeDOS/DJGPP/Cygwin etc.
#EXECEXT=.exe

#Explain the make targets
explain:
	@echo Swatantra Application Launcher
	@echo GNU GPL V2.0 or later. No Warranty.
	@echo make install   :- install the application
	@echo make uninstall :- uninstall the application

#Create swatantra
swatantra:
	@echo -e "#!/bin/bash\nkmdr-executor $(INSTROOT)$(APPPATH)/HindawiLauncher.kmdr"  > $@
	chmod 777 swatantra

#Install the stuff
install: HindawiLauncher.kmdr swatantra
	install -d $(INSTROOT)$(APPPATH)
	install HindawiLauncher.kmdr $(INSTROOT)$(APPPATH)/HindawiLauncher.kmdr
	install swatantra.png $(INSTROOT)$(APPPATH)/swatantra.png
	install HindawiLauncher.desktop	$(INSTROOT)/usr/share/applications/HindawiLauncher.desktop
	install swatantra $(INSTROOT)$(EXECDIR)/swatantra
	rm swatantra

#Install the stuff
uninstall: 
	rm $(INSTROOT)$(EXECDIR)/swatantra
	rm $(INSTROOT)$(APPPATH)/HindawiLauncher.kmdr
	rm $(INSTROOT)$(APPPATH)/swatantra.png
	rm $(INSTROOT)/usr/share/applications/HindawiLauncher.desktop

