/*
Copyright (C) 2003,2004,2005,2006 Abhishek Choudhary
This file is part of the Romenagri Transliteration System.

The Romenagri Transliteration System is free software; 
you can redistribute it and/or modify it under the terms of the 
GNU General Public License as published by the 
Free Software Foundation; either version 2 of the License, or 
(at your option) any later version.

The Romenagri Transliteration System is distributed in the hope 
that it will be useful, but WITHOUT ANY WARRANTY; without 
even the implied warranty of MERCHANTABILITY or FITNESS FOR 
A PARTICULAR PURPOSE. See the GNU General Public License for 
more details.

You should have received a copy of the GNU General Public
License along with this file; see the file COPYING. If
not, write to the Free Software Foundation,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

/*
Modifications: (Please maintain reverse chronological order)

	dd-mmm-yyyy, Nature of modification,
                        Name of modifier <alias>,
			email adress of modifier

	19-Jan-2006, Added the modifications section,
                        Abhishek Choudhary <hi_pedler>,
			choudhary@indicybers.net

End of modifications.
*/

#include <stdio.h>
#include "acii2rmn.h"
#include "rmn2acii.h"

/*msg is used by the Romenagri routines to return the results*/
char msg2[10240], *msg = msg2;

int
main (int argc, char *argv[])
{
  char text[1024];
  printf ("Romenagri test: \n");
  printf ("Enter some text for Romenagri to ISCII conversion:");
  scanf ("%s", text);
  strcat (text, " ");		/*this is done for safety as the parser is LA(1) */
  printf ("The text in ISCII is %s\n", rmn2acii (text));
  printf ("Verifying the reverse conversion we get %s\n",
	  acii2rmn (rmn2acii (text)));

  return 0;
}
