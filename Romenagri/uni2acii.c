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
#include <fcntl.h>

#include "unicode.h"

int
main (int argc, char *argv[])
{
  FILE *fin, *fout;
  unsigned char t1, t2, tmp[4];
  int i = 0, flag = 0;

#ifdef __DJGPP
  _fmode = O_BINARY;
#endif

  tmp[0] = 0;
  tmp[1] = 0;
  tmp[2] = 0;
  tmp[3] = 0;

  if (argc < 2)
    {
      fin = stdin;
      fout = stdout;
    }
  else
    {
      if (argc < 3)
	{
	  printf ("Usage: uni2acii <UNICODE_filename> <ISCII_filename>\n");
	  return 1;
	}


      if ((fin = fopen (argv[1], "r+")) == NULL)
	{
	  fprintf (stderr, "Cannot open input file, using stdin.\n");
	  fin = stdin;
	}

      if ((fout = fopen (argv[2], "w")) == NULL)
	{
	  fprintf (stderr, "Cannot open output file, using stdout.\n");
	  fout = stdout;
	}
    }

  while (!feof (fin))
    {
      t1 = fgetc (fin);
      if (feof (fin))
	break;
      t2 = fgetc (fin);
      if (feof (fin))
	break;

      if (t1 == 255 && t2 == 254)
	continue;		/*eat the control */

      if (t1 == 13 && t2 == 0)
	continue;		/*eat cr */

      tmp[0] = tmp[2];
      tmp[1] = tmp[3];
      tmp[2] = t1;
      tmp[3] = t2;

      flag = 0;
      for (i = 0; i < UNI_LEN; i++)
	{
	  if (t1 == unicode_hin[i][0] && t2 == unicode_hin[i][1])	/*match */
	    {
	      flag = 1;
	      break;
	    }
	}
      if (flag == 1)
	{
	  fputc (unicode_hin[i][2], fout);
	}
      else
	{
	  fputc (t1, fout);	/*presuming 0 to 127 */
	}
    }

  fclose (fin);
  fclose (fout);

  return 0;
}
