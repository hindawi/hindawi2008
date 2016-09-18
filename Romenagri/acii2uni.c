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
  unsigned char t;
  int i = 0, flag = 0;

#ifdef __DJGPP
  _fmode = O_BINARY;
#endif

  if (argc < 2)
    {
      fin = stdin;
      fout = stdout;
    }
  else
    {
      if (argc < 3)
	{
	  printf ("Usage: acii2uni <ISCII_filename> <UNICODE_filename>\n");
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

  fputc (255, fout);
  fputc (254, fout);
  while (!feof (fin))
    {
      t = fgetc (fin);
      if (feof (fin))
	break;
      if (t == '\n')
	{
	  fputc (13, fout);
	  fputc (0, fout);
	  fputc (10, fout);
	  fputc (0, fout);
	  continue;
	}
      flag = 0;
      for (i = 0; i < UNI_LEN; i++)
	{
	  if (t == unicode_hin[i][2])
	    {
	      flag = 1;
	      break;
	    }
	}
      if (flag == 1)
	{
	  fputc (unicode_hin[i][0], fout);
	  fputc (unicode_hin[i][1], fout);
	}
      else
	{
	  fputc (t % 128, fout);
	  fputc (0, fout);
	}
    }

  fclose (fin);
  fclose (fout);

  return 0;
}
