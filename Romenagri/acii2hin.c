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

#include "acii2hin.h"

char *
acii2hin (char *tok)
{
  char t1, t2, tpush[2];
  int i, j, fnd;
  stack_reset ();
  for (i = 0; i < strlen (tok); i++)
    {
      fnd = 0;
      for (j = 0; j < 75; j++)
	{
	  if (tok[i] == *(acii_hin_chrt[j][1]))
	    {
	      if (*acii_hin_chrt[j][0] == '^' && stack_len () > 0)	/*matra */
		{
		  pop (1);
		  if (*(acii_hin_chrt[j][0] + 1) == 'z')
		    {
		      t1 = *pop (1);
		      if (t1 == 'h')
			{
			  t2 = *pop (1);
			  if (t2 == 'd')
			    push ((char *) "_rdha");
			  else
			    {
			      tpush[0] = t2;
			      tpush[1] = '\0';
			      push ((char *) tpush);
			      push ((char *) "hza");
			    }
			}
		      else
			{
			  if (t1 == 'd')
			    push ((char *) "_rda");
			  else
			    {
			      tpush[0] = t1;
			      tpush[1] = '\0';
			      push ((char *) tpush);
			      push ((char *) "za");
			    }
			}
		    }
		  else
		    {
		      push ((char *) (acii_hin_chrt[j][0] + 1));
		    }
		}
	      else
		push (acii_hin_chrt[j][0]);
	      fnd = 1;
	      break;
	    }
	}
      if (fnd == 0 && tok[i - 1] != acii_hin_chrt[0][1][0])
	{
	  tpush[0] = tok[i];
	  tpush[1] = '\0';
	  push (tpush);
	}
    }
  strcpy (msg, pop (stack_len ()));
  return msg;
}
