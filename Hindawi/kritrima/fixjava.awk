#!/bin/gawk -f

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
#  31-Jul-2006, Created script, Ver. 0.2.0,
#                 Abhishek Choudhary <hi_pedler>,
#                 choudhary@indicybers.net

#Script for Hindawi Indic Programming System Ver. 0.2.0 - Java classname fixer

BEGIN	{
		ccnt=0;
		srcfn="";
	}
		
/class/	{
		if(ccnt>1)
			print "कृप्या एक मूल खाते में केवल एक ही श्रेणी (क्लास) घोषित करें।";
		ccnt = ccnt + 1
		split($0,a,"class "); 
		sub(/^ */,"",a[2]);
		split(a[2],b," extends");
		split(b[1],c,"{");
		split(c[1],d," ");
		sub(/ *$/,"",d[1]);
		srcfn=d[1];
		sub(/\r/,"",srcfn);
	}

END	{
		comm="cp tempfil0123.tmphin.java "srcfn".java";
		system(comm);
		comm="gcj "srcfn".java --main="srcfn" -ohin.exe 2>> tempfil0123.tmphin";
		system(comm);
	}
