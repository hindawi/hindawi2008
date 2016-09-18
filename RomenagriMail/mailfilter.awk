#!/bin/gawk -f

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
#  31-Jul-2006, Created script, Ver. 0.2.0,
#                 Abhishek Choudhary <hi_pedler>,
#                 choudhary@indicybers.net

#Script for Romenagri Transliteration System Ver. 0.2.0 - mailfilter

BEGIN				{
					rmnstart=0;
					rmnend=0;
				}
/^Reply-to:/			{split($0,a,":");print a[2] > "rttmp.temphin"; next}
/^To:/				{split($0,a,":");print a[2] > "totmp.temphin"; next}
/^CC:/				{split($0,a,":");print a[2] > "cctmp.temphin"; next}
/^Date:/			{
					a[1]="";
					a[2]="";
					b[1]="";
					b[2]="";
					split($0,a,"te:");
					split(a[2],b,"/rmn/");
					gsub(" ","\" \"e_",b[1]);
					printf ("\"e_%s\"%s"), b[1], b[2] >\
					"d_tmp.temphin";
					next
				}
/^Subject:/			{
					a[1]="";
					a[2]="";
					b[1]="";
					b[2]="";
					split($0,a,"ect:");
					split(a[2],b,"/rmn/");
					gsub(" ","\" \"e_",b[1]);
					printf ("\"e_%s\"%s"), b[1], b[2] >\
					"s_tmp.temphin";
					next
				}
/^BCC:/				{next}
/^Content-type:/		{next}
/roamstart/			{rmnstart=1; rmnend=0; next}
/roamend/ 			{rmnstart=0; rmnend=1; next}
/^Mail version.*Type.*for help/	{next}
/.*/				{
					if (rmnstart==1 && rmnend==0)
						print $0 > "rmsgtmp.temphin";
					else
						print $0 > "msgtmp.temphin";
				}
END				{
					system("cat d_tmp.temphin |\
					fixuninum | iconv -f utf-8\
					-t utf-16 | uni2acii | rmn2acii | acii2uni\
					| iconv -f utf-16 -t utf-8 > dttmp.temphin");

					system("cat s_tmp.temphin |\
					fixuninum | iconv -f utf-8\
					-t utf-16 | uni2acii | rmn2acii | acii2uni\
					| iconv -f utf-16 -t utf-8 > sjtmp.temphin");

					system("cat rmsgtmp.temphin |\
					fixuninum | iconv -f utf-8\
					-t utf-16 | uni2acii | rmn2acii | acii2uni\
					| iconv -f utf-16 -t utf-8 > message.temphin");

					system("echo -e \"\\n~~~~~~~~~~~~~~~~~~~~\\n\"\
					>>message.temphin");

					system("cat msgtmp.temphin >>message.temphin");
				}
