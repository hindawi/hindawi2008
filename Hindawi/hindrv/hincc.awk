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

#Script for Hindawi Indic Programming System Ver. 0.2.0 - hincc backend

BEGIN	{lc=0; cc="praa_tha>"}
/.*/	{
		if ($1=="<shailee")
		{
			if (lc > 0)
				print "शैली का उल्लेख पहली पंक्ति में करें।";
			else
				cc=$2;
		}
		else
		{
			print $0 > "tempfil012345.temphin";
			lc = lc + 1;
		}
	}
END	{
		if (lc > 0)
		{
			if (cc=="praa_tha>")
				system("praathacc tempfil012345.temphin");
			else if (cc=="praa_thamika>")
				system("praathacc tempfil012345.temphin");
			else if (cc=="guru>")
				system("gurucc tempfil012345.temphin");
			else if (cc=="guroo>")
				system("gurucc tempfil012345.temphin");
			else if (cc=="shrae_nnee>")
				system("shraenicc tempfil012345.temphin");
			else if (cc=="shrae_nneeba_d_dha>")
				system("shraenicc tempfil012345.temphin");
			else if (cc=="shab_da>")
				system("shabdacc tempfil012345.temphin");
			else if (cc=="wyaaka>")
				system("wyaakacc tempfil012345.temphin");
			else if (cc=="ya_m_tra>")
				system("yantracc tempfil012345.temphin");
			else if (cc=="yaa_m_trika>")
				system("yantracc tempfil012345.temphin");
			else if (cc=="roaboata>")
				system("robotcc tempfil012345.temphin");
			else if (cc=="k_ri_trima>")
				system("kritrimacc tempfil012345.temphin");
		}
		else
			print "मूल खाता खाली है।"
	}
