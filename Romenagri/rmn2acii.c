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

#include "rmn2acii.h"
char tst2[2];
char *tst = tst2;
char *lev1 = "|kgcjtdnpbmyrlwsxh_^";

char *
c2s (char t)
{
  tst[0] = t;
  tst[1] = '\0';
  return tst;
}

char *
lookup (char *key)
{
  int i;
  for (i = 0; i < 74; i++)
    {
      if (strcmp (acii_chrt[i][0], key) == 0)
	return acii_chrt[i][1];
    }
  return NULL;
}

int
matra (char *tok)
{
  if (strlen (tok) < 1)
    {
      strcat (msg, lookup ("^"));	/*terminating 'halant' */
      return 1;
    }
  switch (tok[0])
    {
    case 'i':
      strcat (msg, lookup ("^i"));
      tok++;
      level1 (tok);
      break;
    case 'u':
      strcat (msg, lookup ("^u"));
      tok++;
      level1 (tok);
      break;
    case 'e':
      strcat (msg, lookup ("^ee"));
      tok += 2;
      level1 (tok);
      break;
    case 'z':
      strcat (msg, lookup ("^za"));
      level2a (++tok);
      break;
    case '_':			/*redundant */
      strcat (msg, lookup ("^_ri"));
      tok += 3;
      level1 (tok);
      break;
    case 'o':
      tok++;
      switch (tok[0])
	{
	case 'o':
	  strcat (msg, lookup ("^oo"));
	  tok++;
	  level1 (tok);
	  break;
	case 'a':
	  strcat (msg, lookup ("^oa"));
	  tok++;
	  level1 (tok);
	  break;
	case 'u':
	  strcat (msg, lookup ("^ou"));
	  tok++;
	  level1 (tok);
	  break;
	default:
	  strcat (msg, c2s (tok[0]));
	  level1 (++tok);
	};
      break;
    case 'a':
      tok++;
      if (strstr (lev1, c2s (tok[0])) != NULL)
	level1 (tok);
      else
	switch (tok[0])
	  {
	  case 'a':
	    strcat (msg, lookup ("^aa"));
	    tok++;
	    level1 (tok);
	    break;
	  case 'e':
	    strcat (msg, lookup ("^ae"));
	    tok++;
	    level1 (tok);
	    break;
	  case 'i':
	    strcat (msg, lookup ("^ai"));
	    tok++;
	    level1 (tok);
	    break;
	  case 'o':
	    strcat (msg, lookup ("^ao"));
	    tok++;
	    level1 (tok);
	    break;
	  case 'u':
	    strcat (msg, lookup ("^au"));
	    tok++;
	    level1 (tok);
	    break;
	  default:
	    strcat (msg, c2s (tok[0]));
	    level1 (++tok);
	  };
      break;
    default:
      strcat (msg, c2s (tok[0]));
      level1 (++tok);
    };
  return 1;
}

int
level2a (char *tok)
{
  if (strlen (tok) < 1)
    {
      strcat (msg, lookup ("^"));
      return 1;
    }
  if (strstr (lev1, c2s (tok[0])) != NULL)
    {
      strcat (msg, lookup ("^"));
      level1 (tok);
    }
  else
    matra (tok);
  return 1;
}

int
level2b (char *tok, char *lk)
{
  char lup[7];
  strcpy (lup, lk);
  if (strlen (tok) < 1)
    {
      strcat (lup, "a");
      strcat (msg, lookup (lup));
      strcat (msg, lookup ("^"));
      return 1;
    }
  if (tok[0] == 'h')
    {
      strcat (lup, "ha");
      strcat (msg, lookup (lup));
      level2a (++tok);
    }
  else if (tok[0] == 'v')	/*lvra-case */
    {
      strcat (lup, "vra");
      strcat (msg, lookup (lup));
      level2a ((tok + 2));
    }
  else
    {
      strcat (lup, "a");
      strcat (msg, lookup (lup));
      level2a (tok);
    }
  return 1;
}

int
level2c (char *tok)
{
  if (strlen (tok) < 1)
    {
      strcat (msg, lookup ("^"));
      return 1;
    }
  switch (tok[0])
    {
    case 'u':
      strcat (msg, lookup ("_u"));
      tok++;
      level1 (tok);
      break;
    case 'm':
      strcat (msg, lookup ("_m"));
      tok++;
      level1 (tok);
      break;
    case 'i':
      strcat (msg, lookup ("_i"));
      tok++;
      level1 (tok);
      break;
    case 'e':
      strcat (msg, lookup ("_ee"));
      tok += 2;
      level1 (tok);
      break;
    case 'y':
      strcat (msg, lookup ("_yna"));
      tok++;
      level2a (++tok);
      break;
    case 't':
      level2b (++tok, "_t");
      break;
    case 'd':
      level2b (++tok, "_d");
      break;
    case 'h':
      if (tok[1] == 'h')
	{
	  strcat (msg, lookup ("_hh"));
	  tok++;
	  level1 (++tok);
	}
      else
	{
	  strcat (msg, lookup ("_ha"));
	  level2a (++tok);
	}
      break;
    case 'o':
      tok++;
      switch (tok[0])
	{
	case 'o':
	  strcat (msg, lookup ("_oo"));
	  tok++;
	  level1 (tok);
	  break;
	case 'a':
	  strcat (msg, lookup ("_oa"));
	  tok++;
	  level1 (tok);
	  break;
	case 'u':
	  strcat (msg, lookup ("_ou"));
	  tok++;
	  level1 (tok);
	  break;
	default:
	  strcat (msg, c2s (tok[0]));
	  level1 (++tok);
	};
      break;
    case 'n':
      tok++;
      switch (tok[0])
	{
	case 'a':
	  strcat (msg, lookup ("_na"));
	  tok++;
	  level1 (tok);
	  break;
	case 'n':
	  strcat (msg, lookup ("_nna"));
	  level2a (++tok);
	  break;
	case 'g':
	  strcat (msg, lookup ("_nga"));
	  level2a (++tok);
	  break;
	default:
	  strcat (msg, c2s (tok[0]));
	  level1 (++tok);
	};
      break;
    case 'r':
      tok++;
      switch (tok[0])
	{
	case 'i':/*_ri*/
	  if (strlen (msg) > 0 && msg[strlen (msg) - 1] == *lookup ("^"))	/*push-pop */
	    {
	      msg[strlen (msg) - 1] = '\0';
	      strcat (msg, lookup ("^_ri"));
	    }
	  else
	    strcat (msg, lookup ("_ri"));
	  tok++;
	  level1 (tok);
	  break;
	case 'd':
	  level2b (++tok, "_rd");
	  break;
	default:
	  strcat (msg, c2s (tok[0]));
	  level1 (++tok);
	};
      break;
    case 'a':
      tok++;
      if (strstr (lev1, c2s (tok[0])) != NULL)
	{
	  strcat (msg, lookup ("_a"));
	  level1 (tok);
	}
      else
	switch (tok[0])
	  {
	  case 'a':
	    strcat (msg, lookup ("_aa"));
	    tok++;
	    level1 (tok);
	    break;
	  case 'e':
	    strcat (msg, lookup ("_ae"));
	    tok++;
	    level1 (tok);
	    break;
	  case 'i':
	    strcat (msg, lookup ("_ai"));
	    tok++;
	    level1 (tok);
	    break;
	  case 'o':
	    strcat (msg, lookup ("_ao"));
	    tok++;
	    level1 (tok);
	    break;
	  case 'u':
	    strcat (msg, lookup ("_au"));
	    tok++;
	    level1 (tok);
	    break;
	  default:
	    strcat (msg, c2s (tok[0]));
	    level1 (++tok);
	  };
      break;
    default:
      strcat (msg, c2s (tok[0]));
      level1 (++tok);
    }
  return 1;
}

int
level1 (char *tok)
{
  if (strlen (tok) < 1)
    return 1;
  switch (tok[0])
    {
    case '|':
      strcat (msg, lookup ("|"));
      tok++;
      level1 (tok);
      break;
    case 'n':
      strcat (msg, lookup ("na"));
      level2a (++tok);
      break;
    case 'm':
      strcat (msg, lookup ("ma"));
      level2a (++tok);
      break;
    case 'y':
      strcat (msg, lookup ("ya"));
      level2a (++tok);
      break;
    case 'w':
      strcat (msg, lookup ("wa"));
      level2a (++tok);
      break;
    case 'x':
      strcat (msg, lookup ("xa"));
      level2a (++tok);
      break;
      /*case 'h':
         strcat(msg,lookup("ha"));
         level2a(++tok);
         break; */
    case 'r':
      strcat (msg, lookup ("ra"));
      level2a (++tok);
      break;
    case 'g':
      level2b (++tok, "g");
      break;
    case 'c':
      tok++;
      level2b (++tok, "ch");
      break;
    case 't':
      level2b (++tok, "t");
      break;
    case 'd':
      level2b (++tok, "d");
      break;
    case 'p':
      level2b (++tok, "p");
      break;
    case 'b':
      level2b (++tok, "b");
      break;
    case 's':
      level2b (++tok, "s");
      break;
    case 'l':
      level2b (++tok, "l");
      break;
    case 'k':
      level2b (++tok, "k");
      break;
    case 'j':
      level2b (++tok, "j");
      break;
    case '_':
      level2c (++tok);
      break;
    case '^':			/*discrete 'matra' */
      matra (++tok);
      break;
    default:
      strcat (msg, c2s (tok[0]));
      level1 (++tok);
    };
  return 1;
}

char *
rmn2acii (char *tok)
{
  *msg = '\0';
  level1 (tok);
  return msg;
}
