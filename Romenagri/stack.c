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

#include "stack.h"
char stack2[10240], popper2[10240];
char *stack = stack2, *popper = popper2;
int ts = 0;

int
push (char *data)
{
  if ((ts + strlen (data)) > 10240)
    return 0;
  strcat (stack, data);
  ts += strlen (data);
  return 1;
}

char *
pop (int n)
{
  if ((ts - n) < 0)
    return NULL;
  strcpy (popper, (char *) (stack + ts - n));
  ts -= n;
  stack[ts] = '\0';
  return popper;
}

int
stack_reset ()
{
  ts = 0;
  return 1;
}

int
stack_len ()
{
  return ts;
}
