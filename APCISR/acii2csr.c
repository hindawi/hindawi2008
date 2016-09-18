/*
Copyright (C) 2003,2004,2005,2006 Abhishek Choudhary
This file is part of the AP Code for Indian Script Representation.

The AP Code for Indian Script Representation is free software; 
you can redistribute it and/or modify it under the terms of the 
GNU General Public License as published by the 
Free Software Foundation; either version 2 of the License, or 
(at your option) any later version.

The AP Code for Indian Script Representation is distributed in the hope 
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

        24-Jan-2006, Removed variable grid - LM is directly looked-up now,
                        Abhishek Choudhary <hi_pedler>,
                        choudhary@indicybers.net

        23-Jan-2006, Moved the font substitution function outside
                     of the rendering loop - a major flaw earlier,
                        Abhishek Choudhary <hi_pedler>,
                        choudhary@indicybers.net

        22-Jan-2006, Annotated code,
                        Abhishek Choudhary <hi_pedler>,
                        choudhary@indicybers.net

        20-Jan-2006, Added the modifications section,
                        Abhishek Choudhary <hi_pedler>,
                        choudhary@indicybers.net
End of modifications.
*/

/*
Abbreviations:
AP - Anamika Press
APCISR - Anamika Press Code for Indic Script Representation

Origin:
M/S Anamika Press, Katihar are printers and publishers operating from the
eastern Indian town of Katihar, located in the state of Bihar. It is around
500 km from the cites of Kolkata (Calcutta) and Patna. The APCISR standard
was developed at Anamika Press, initially for impression printing purposes,
and was later adopted for fixed width console display. The originator of
this standard is Abhishek Choudhary, then an employee of Anamika Press. All
the rights for this work, including copyright, are held by Abhishek Choudhary
and not Anamika Press, or any other individual or entity. This text is
included here for information only.
*/


/*
ISCII to APCISR conversion and support functions.
Filename: acii2csr.c
Original implementation: ANSI C
Implemented by: Abhishek Choudhary
Period of initial implementation: Oct 2003 to May 2004
Dependencies: None
Special: Seperate facility for loading VGA fonts required such
as setfont or fontsel.
*/


/* Include the interface */
#include "acii2csr.h"


/*
ISCII to APCISR conversion routine.
Input: 0 delimited ISCII string (unsigned char) and a cursor into it (int)
Output: Rendered APCISR map. Variable retval of type cisr must be declared
Background: Based on Abhishek Choudhary's 9-grid theory for Brahmi derived
scripts. The algorithm is discussed briefly in the file apcisr.txt, which
also introduces APCISR.
*/

cisr
acii2cisr (unsigned char *aci_in, int cpos)
{
  /*
     Variable actmp is used as a temporary copy of input string aci_in, aci
     is a right-sided window into actmp, l_aci holds the length of the original
     ISCII after adjusting initial 'ref', curpos is a cursor into actmp
   */
  unsigned char actmp[1024], *aci;
  int l_aci = 0, curpos = 0;

  /*
     Variables urdha, madhya and nimna together form the working map (WM),
     varibles matrik, lipik, purak are cursors into the WM
   */
  unsigned char urdha[1024], madhya[1024], nimna[1024];
  int matrik = 0, lipik = 0, purak = 0;

  /*
     Variable flag tells whether apcisr_chart entry has been found, purna
     gives the purna status, varna_purna tells whether last symbol completed
     a varna including a sanyuktakshara, prev_hal tells whether the last symbol
     encountered was a hal, prev_ra_hal tells whether the last symbol encontered
     was a ra+hal (as a following varna will show it on top), lnfnd tells if the
     last symbol was not found, lasttok is the previous index into apcisr_chrt,
     grid_job tells whether to render the current symbol or look-ahead
   */
  int flag = 0, purna = 2, varna_purna = 1, prev_hal = 0, prev_ra_hal = 0,
    lnfnd = 0, lasttok = 0, grid_job = 1;

  /* House-keeping variable used for loops and indexing into apcisr_chrt */
  int i = 0;

  /*
     Variable pos is a cursor into retval, check keeps a count of the numbers
     of cisr columns eliminated during font substituion, fpos holds the value of
     the position where the original ISCII at cpos occurs. Variable fpos should
     be initialised to less than 0 as this condition is used to check whether it
     has been set before, owing to the fact that the exact cpos ISCII may be
     consumed non-productively.
   */
  int pos = 0, check = 0, fpos = -1;

  /* End of declarations */

  /* Initialisation */

  /*
     Copy the input string into the temporary working string and set the
     right-sided window to begin at initial location.
   */
  strcpy (actmp, aci_in);
  aci = actmp;

  /* Initialise the strings to zeroes */
  for (i = 0; i < 1024; i++)
    {
      urdha[i] = madhya[i] = nimna[i] = 0;
      retval.urdha[i] = retval.madhya[i] = retval.nimna[i] = 0;
    }

  /* End of initialisation */

  /* Start processing the ISCII input */

  /* Remove first 'ref' if it exists -> bad ISCII */
  if (strstr (aci, "èÏ") == (char *) aci)
    {
      /* Shift window right and reduce cpos as we are redefing input */
      aci++;
      cpos--;
    }

  /* Strore the length of the input string in l_aci */
  l_aci = strlen (aci);

  /* Process the input string till the window length is not zero */
  while (strlen (aci))
    {
      /* Look-up next ISCII symbol in apcisr_chrt (speed bottleneck) */

      /* Locate the 9-grid pattern in the APCISR look-up table */
      flag = 0;
      for (i = 0; i < APCISR_LEN; i++)
	{
	  /*
	     Method of lookup is to check whether the ISCII character is the
	     leftmost in the window
	   */
	  if (strstr (aci, apcisr_chrt[i].acii) == (char *) aci)
	    {
	      flag = 1;
	      break;
	    }
	}

      /*
         Variable varna_purna gives purna status of previous ISCII char.
         A non-zero varna_purna leads to a new matrik; i.e. varna is complete
       */
      if (purna)
	{
	  varna_purna = purna;
	}

      /* Purna value is retrived from apcisr_chart */
      purna = apcisr_chrt[i].purna;

      /*
         Repeated matra is bad ISCII and hence ignored. Matra over-do
         prevention needs to be improved
       */
      if (i == lasttok && purna == 0)
	{
	  flag = 0;
	}

      /* Set lasttok equal to the index in apcisr_chrt */
      lasttok = i;

      if (flag == 1)
	/* If ISCII symbol found... */
	{
	  /* Advance window to exclude identified token */
	  aci += strlen (apcisr_chrt[i].acii);

	  /* Advance cursor to correspond to the window */
	  curpos += strlen (apcisr_chrt[i].acii);

	  /* Signal that last symbol was found (for next iteration use) */
	  lnfnd = 0;
	}
      else
	/* If ISCII symbol not found... */
	{
	  /* As this is not ISCII a prev_hal is inconsequential */
	  prev_hal = 0;

	  /*
	     If last character was ISCII and this is not the first character
	     then the previous cisr needs to be completed.
	   */
	  if (lnfnd == 0 && strlen (aci) < l_aci)
	    {
	      purak++;
	    }

	  /* The non-ISCII input char is placed in urdha[purak] */
	  urdha[purak] = aci[0];

	  /*
	     The madhya[purak] is filled with a special marker to identify
	     a non-ISCII character.
	   */
	  madhya[purak] = 192;

	  /*
	     Since other cisr-advancing rules will not apply here, we need to
	     manually advance the cisr positions. As we do not know in advance
	     whether the nex character is ISCII or not we have to keep the next
	     cisr collapsed for now. Otherwise the resulting empty matrik would
	     be rendered.
	   */
	  matrik = purak + 1;
	  lipik = purak = matrik;

	  /*
	     Set the next cisr madhya position to represent a non-lipik by
	     setting it equal to 128;i.e. set bit 7 to indicate matrik/purak
	   */
	  madhya[purak] = 128;

	  /* Advance window to consume the non-ISCII */
	  aci++;
	  curpos++;

	  /*
	     A non-ISCII by definition cannot be a purna as it does not
	     generate a new cisr, hence, we set purna equal to 0
	   */
	  purna = 0;

	  /*
	     Now check whether the character at cpos has been consumed. If
	     it has been, then set fpos to previous cisr, where we rendered the
	     non-ISCII character
	   */
	  if (curpos >= cpos && fpos < 0)
	    {
	      fpos = purak - 1;
	    }

	  /*
	     Indicate that the last cisr was completed as the non-ISCII has
	     been rendered to a collapsed cisr
	   */
	  varna_purna = 1;

	  /* Flag that last characer was not found; i.e. non-ISCII */
	  lnfnd = 1;
	}

      /* End of look-up phase */


      /* Apply the LM - WM combination rules for Devnagri and combine them */

      /*
         These rules are applicable only to ISCII characters and not to other
         non-ISCII characters or numerals. This is where the WM cursors are
         positioned. Dynamic behaviour is handled by the rules here.
       */
      if (lnfnd == 0)
	{
	  /* General LM - WM combination rules */

	  if (prev_ra_hal == 1)
	    /* If the sequence ra_hal was encountered earlier... */
	    {
	      if ((purna % 2) == 1)
		/* If the current character has an even purna status... */
		{
		  /* ...add 'ref' to urdha-lipik */
		  urdha[lipik] = urdha[lipik] | 128;
		}
	      else
		/* If the current character has an odd purna status... */
		{
		  /* ...add 'ref' to urdha-purak */
		  urdha[purak] = urdha[purak] | 128;
		}
	    }
	  else if (purna && prev_hal == 0)
	    /* Under non-'hal' conditions; i.e. the trivial purna method */
	    {
	      /* Old purak becomes new matrik */
	      matrik = purak;

	      /* Lipik is next to matrik */
	      lipik = matrik + 1;

	      /* Purak is next to lipik */
	      purak = lipik + 1;

	      /* Set bit 7 on madhya-matrik and madhya-purak to signal
	         non-lipik */
	      madhya[matrik] = madhya[matrik] | 128;
	      madhya[purak] = madhya[purak] | 128;
	    }
	  else if (purna && prev_hal == 1)
	    /* If current is purna but after a hal then... */
	    {
	      /*
	         Perform a delayed 'hal' operation; i.e. the half-form of a
	         consonant is rendered only if there is a following ISCII, else
	         the previous character is displayed in full form, but with a
	         'hal' below it.
	       */
	      if (varna_purna > 1)
		/* If the last character had a purna of 1 then... */
		{
		  if (nimna[lipik] >= 64)
		    /* ...if the nimna-lipik has bit 6 ('hal')on then... */
		    {
		      /* ...turn 'hal'-mark off */
		      nimna[lipik] -= 64;
		    }
		  else if (nimna[purak] >= 64)
		    /* ...if the nimna-purak has bit 6 ('hal')on then... */
		    {
		      /* ...turn 'hal'-mark off */
		      nimna[purak] -= 64;
		    }
		}

	      /* Matrik position remains unchanged */
	      /* matrik = matrik; *//* nop; no operation */

	      /* Lipik is right shifted; i.e. cisr is expanded */
	      lipik++;

	      /* Purak is next to lipik */
	      purak = lipik + 1;

	      /* Bit 7 is turned off on the madhya-lipik to indicate
	         a lipik */
	      madhya[lipik] = 0;

	      /* Bit 7 is set on  madhya-matrik and madhya-purak to indicate
	         a non-lipik */
	      madhya[matrik] = madhya[matrik] | 128;
	      madhya[purak] = madhya[purak] | 128;
	    }

	  /*
	     Check to see whether the character at cpos has been consumed. If
	     it has been, then set fpos to current cisr lipik.
	   */
	  if (curpos >= cpos && fpos < 0)
	    {
	      fpos = lipik;
	    }

	  /* Previous hal and ra_hal ('ref' above) have been considered */
	  prev_hal = 0;
	  prev_ra_hal = 0;

	  /*
	     Ensure that LM-WM combination is activated by default. However,
	     dynamic rules may modify this.
	   */
	  grid_job = 1;

	  /* The dynamic rules and sequence specific rules appear here */

	  /* Dynamic rule #1 - ^ra */
	  /*
	     Rule: Adding a lower ref to a vyanjan without a purak requires
	     addition of the ref to the nimna-lipik instead of nimna-purak
	   */
	  if (strcmp (apcisr_chrt[i].acii, "èÏ") == 0 && varna_purna == 1)
	    {
	      /* Add the ref bit to the nimna-lipik */
	      nimna[lipik] = nimna[lipik] | 8;

	      /* De-activate LM-WM combination for this character */
	      grid_job = 0;
	    }

	  /* Dynamic rule #2 - ^u */
	  /*
	     Rule: Adding a harshva-u-kar (^u or chota-oo) to a vyanjan
	     without a purak requires addition of the matra to the nimna-lipik
	     instead of nimna-purak
	   */
	  if (strcmp (apcisr_chrt[i].acii, "Ý") == 0
	      && (varna_purna % 2) == 1)
	    {
	      /* Add the 'chota-oo matra'-bit to the nimna-lipik */
	      nimna[lipik] = nimna[lipik] | 2;

	      /* De-activate LM-WM combination for this character */
	      grid_job = 0;
	    }

	  /* Dynamic rule #3 - visargaha */
	  /*
	     Rule: When adding a visargaha to a vyanjan without a purak,
	     which already has o-kar (^oa), shift the a-kar part of the o-kar
	     from the urdha-purak to urdha-lipik
	   */
	  if (strcmp (apcisr_chrt[i].acii, "£") == 0 && varna_purna == 1
	      && (madhya[purak] && 4) && (urdha[purak] && 4))
	    {
	      /* Shift the a-kar part from the urdha-purak to urdha-lipik */
	      urdha[lipik] = urdha[lipik] | 4;
	      urdha[purak] = urdha[purak] - 4;

	      /* Add visargaha bit to the madhya-purak */
	      madhya[purak] = madhya[purak] | 16;

	      /* De-activate LM-WM combination for this character */
	      grid_job = 0;
	    }

	  /* Dynamic rule #4 - chandra bindu */
	  /*
	     Rule: For vyanjans without a purak, the chandra bindu is added
	     to urdha-lipik instead of urdha-purak, unless a matra is present
	   */
	  if (strcmp (apcisr_chrt[i].acii, "¡") == 0 && varna_purna == 1
	      && madhya[purak] == 160)
	    {
	      /* Add chandra bindu to urdha-lipik */
	      urdha[lipik] = urdha[lipik] | 32;

	      /* De-activate LM-WM combination for this character */
	      grid_job = 0;
	    }

	  /* Dynamic rule #5 - bindu */
	  /*
	     Rule: For vyanjans without a purak, the bindu is added to
	     urdha-lipik instead of urdha-purak, unless a matra is present
	   */
	  if (strcmp (apcisr_chrt[i].acii, "¢") == 0 && varna_purna == 1
	      && madhya[purak] == 160)
	    {
	      /* Add bindu to urdha-lipik */
	      urdha[lipik] = urdha[lipik] | 16;

	      /* De-activate LM-WM combination for this character */
	      grid_job = 0;
	    }

	  /* Dynamic rule #6 - ^oo */
	  /*
	     Rule: For vyanjans without a purak, the oo-kar (^oo) is added to
	     nimna-lipik instead of nimna-purak
	   */
	  if (strcmp (apcisr_chrt[i].acii, "Þ") == 0
	      && (varna_purna % 2) == 1)
	    {
	      /* Add ^oo to nimna-lipik */
	      nimna[lipik] = nimna[lipik] | 1;

	      /* De-activate LM-WM combination for this character */
	      grid_job = 0;
	    }

	  /* Dynamic rule #7 - ref */
	  /*
	     Rule: If ra+hal sequence is found then indicate it for
	     the next iteration
	   */
	  if (strcmp (apcisr_chrt[i].acii, "Ïè") == 0 && aci[0] > 127)
	    {
	      /* Indicate ra + hal sequence found */
	      prev_ra_hal = 1;

	      /* De-activate LM-WM combination for this character */
	      grid_job = 0;
	    }

	  /* Dynamic rule #8 - _ha^ */
	  /*
	     Rule: If _ha+hal sequence is found and next charcter in the
	     window is ISCII then insert half-_ha glyph into madhya-lipik
	   */
	  if (strcmp (apcisr_chrt[i].acii, "Øè") == 0 && aci[0] > 127)
	    {
	      /* Replace with half - _ha glyph */
	      madhya[lipik] = madhya[lipik] | 45;

	      /* De-activate LM-WM combination for this character */
	      grid_job = 0;
	    }

	  /* Dynamic rule #9 - hal */
	  /*
	     Rule: On encountering a nascent hal, it is indicated by setting
	     prev_hal to 1. The hal symbol is added to nimna-lipik for varnas
	     without a khardi-pai and to nimna-purak for others. Conjunction
	     if required is performed during the next iteration.
	   */
	  if (strcmp (apcisr_chrt[i].acii, "è") == 0)
	    {
	      /* Indicate that hal was encountered */
	      prev_hal = 1;

	      /* Add the hal symbol as per rule */
	      if ((varna_purna % 2) == 1)
		{
		  nimna[lipik] = nimna[lipik] | 64;
		}
	      else
		{
		  nimna[purak] = nimna[purak] | 64;
		}

	      /* De-activate LM-WM combination for this character */
	      grid_job = 0;
	    }

	  /* Dynamic rule #10 - ^i */
	  /*
	     Rule: If a harshva-ee-kar (^i or choti-ee) is encountered after
	     matrik and lipik have become seperated owing to conjoint varnas
	     then the upper dome of the ^i has to be adjusted such that its
	     second half lies in a cell adjacent to urdha-matrik
	   */
	  if (strcmp (apcisr_chrt[i].acii, "Û") == 0 && (matrik != lipik - 1))
	    {
	      urdha[matrik] = urdha[matrik] | 1;

	      /* Here matrik and lipik are not contiguous but the second
	         part of the dome of ^i needs to be in the cell adjacent to
	         urdha-matrik */
	      urdha[matrik + 1] = urdha[matrik + 1] | 2;

	      /* 'Khardi-pai' goes as usual */
	      madhya[matrik] = madhya[matrik] | 8;

	      /* De-activate LM-WM combination for this character */
	      grid_job = 0;
	    }

	  /* End of dynamic rules */

	  /* Combine the LM and WM */
	  if (grid_job == 1)
	    /* If further LM-WM combination required then... */
	    {
	      /* ...combine matriks - logical OR */
	      urdha[matrik] = urdha[matrik] |
		apcisr_chrt[i].grid[_urdha + _matrik];
	      madhya[matrik] = madhya[matrik] |
		apcisr_chrt[i].grid[_madhya + _matrik];
	      nimna[matrik] = nimna[matrik] |
		apcisr_chrt[i].grid[_nimna + _matrik];

	      /* ...combine lipiks - logical OR */
	      urdha[lipik] = urdha[lipik] |
		apcisr_chrt[i].grid[_urdha + _lipik];
	      madhya[lipik] = madhya[lipik] |
		apcisr_chrt[i].grid[_madhya + _lipik];
	      nimna[lipik] = nimna[lipik] |
		apcisr_chrt[i].grid[_nimna + _lipik];

	      /* ...combine puraks - logical OR */
	      urdha[purak] = urdha[purak] |
		apcisr_chrt[i].grid[_urdha + _purak];
	      madhya[purak] = madhya[purak] |
		apcisr_chrt[i].grid[_madhya + _purak];
	      nimna[purak] = nimna[purak] |
		apcisr_chrt[i].grid[_nimna + _purak];
	    }
	  /* LM - WM merger completed */
	}
    }
  /* End of rules application and LM - WM combination phase */

  /* Font substitution on the rendered APCISR */
  check = 0;

  /* Iterate till the final WM purak */
  for (pos = 0; pos <= purak; pos++)
    {
      /*
         Visibly empty cisr columns are eliminated to produce more
         consolidated rendition
       */
      if ((madhya[pos] == 160 || madhya[pos] == 128) && urdha[pos] == 0
	  && nimna[pos] == 0)
	{
	  /* Increment count of eliminated lines */
	  check++;

	  /* Increment retval cursor */
	  pos++;
	}

      /* Set curpos at (pos - check) at fpos. Add one as base is 1. */
      if (pos == fpos)
	{
	  retval.curpos = pos - check + 1;
	}

      /*
         Now check for and render non-ISCII characters. This is indicated
         by a character value 192 in madhya[pos]. The required character-
         code is in urdha[pos]
       */
      if (madhya[pos] == 192)
	{
	  retval.urdha[pos - check] = 32;
	  retval.madhya[pos - check] = urdha[pos];
	  retval.nimna[pos - check] = 32;
	  continue;
	}

      /*
         Adjustment needed for upper-'raekha' for matra in non-lipik as
         it is redundant when appearing by itself
       */
      if ((madhya[pos] > 160) && (madhya[pos] != 192))
	{
	  madhya[pos] -= 32;
	}

      /*
         Move the top-'ref' to urdha-purak if there is a matra in purak,
         else let it be above urdha-lipik
       */
      if ((pos < purak) && (urdha[pos] >= 128) && (madhya[pos] < 128)
	  && (madhya[pos + 1] != 160) && (madhya[pos + 1] != 129)
	  && (madhya[pos + 1] != 128))
	{
	  urdha[pos] -= 128;
	  urdha[pos + 1] += 128;
	}

      /*
         Substitute the font-codes with charset glyph-codes, through
         direct cisr indexing. Nimna codes are repeated after value 128.
       */
      retval.madhya[pos - check] = cisr_madhya[madhya[pos]][1];
      retval.urdha[pos - check] = cisr_urdha[urdha[pos]][1];
      if (nimna[pos] <= 128)
	{
	  retval.nimna[pos - check] = cisr_nimna[nimna[pos]][1];
	}
      else
	{
	  retval.nimna[pos - check] = cisr_nimna[nimna[pos] - 128][1];
	}
    }
  /* End of font substitution phase */


  /* Trim the rendered cisr to valid limit */
  retval.urdha[pos - check] = 0;
  retval.madhya[pos - check] = 0;
  retval.nimna[pos - check] = 0;

  if (retval.curpos < 1)
    /* Cursor cannot be positioned before the first cisr character */
    {
      retval.curpos = 1;
    }
  else if (retval.curpos > pos - check)
    /* Cursor cannot be positioned beyond the last cisr character */
    {
      retval.curpos = pos - check;
    }

  /* End of ISCII to APCISR conversion */

  /* Return the rendered cisr */
  return retval;
}

/* End of ISCII to APCISR conversion routine */

/* APCISR support functions */

/* Function to 'rtrim' a cisr */

cisr
cisr_trim (cisr dvn)
{
  int l = strlen (dvn.urdha) - 1;
  if ((dvn.madhya[l] == '\32') || (dvn.madhya[l] == '\255'))
    {
      dvn.urdha[l] = 0;
      dvn.madhya[l] = 0;
      dvn.nimna[l] = 0;
    }
  return dvn;
}

/* Generic function for printing cisr on console */

int
cisr_printf_con (cisr dvn)
{
  printf ("%s\n", dvn.urdha);
  printf ("%s\n", dvn.madhya);
  printf ("%s", dvn.nimna);
  return 0;
}

/*
The following functions utilise conio.h routines gotoxy, cprintf etc. As,
these are not available under standard GNU GCC, we need to check availability.
These functions can be ported to ncurses or similar library. If your system
has conio.h then add a #define __GOT_CONIO below.
*/

#ifdef __DJGPP
#define __GOT_CONIO
#endif

#ifdef __GOT_CONIO

/*
Function to render an apcisr at a location identified by top-left corodinates
specified in screen co-ordinates and not cisr co-ordinates.
*/

int
cisr_printf (int x, int y, cisr dvn)
{
  gotoxy (x, y);
  cprintf ("%s", dvn.urdha);
  gotoxy (x, y + 1);
  cprintf ("%s", dvn.madhya);
  gotoxy (x, y + 2);
  cprintf ("%s", dvn.nimna);
  return 0;
}

/*
Function as cisr_printf, but clean apcisr-line for 78 positions. We clean
only till 78 positions since when used for display in box the two extremes
may have a border character which we would not like to wipe out.
*/

int
cisr_printf_clean (int x, int y, cisr dvn)
{
  int i, l;
  l = strlen (dvn.madhya);
  gotoxy (x, y);
  cprintf ("%s", dvn.urdha);
  for (i = l; i < 78; i++)
    {
      cprintf (" ");
    }
  gotoxy (x, y + 1);
  cprintf ("%s", dvn.madhya);
  for (i = l; i < 78; i++)
    {
      cprintf (" ");
    }
  gotoxy (x, y + 2);
  cprintf ("%s", dvn.nimna);
  for (i = l; i < 78; i++)
    {
      cprintf (" ");
    }
  return 0;
}

/* Function to clear out a line till 78 positions. */

int
cisr_clean (int x, int y)	/*only clean to 78 */
{
  int i;
  gotoxy (x, y);
  for (i = 0; i < 78; i++)
    {
      cprintf (" ");
    }
  gotoxy (x, y + 1);
  for (i = 0; i < 78; i++)
    {
      cprintf (" ");
    }
  gotoxy (x, y + 2);
  for (i = 0; i < 78; i++)
    {
      cprintf (" ");
    }
  return 0;
}

/* Function to print a given string on a cisr-line, repeated 3 times */

int
c3n_printf (int x, int y, char *txt)
{
  gotoxy (x, y);
  cprintf ("%s", txt);
  gotoxy (x, y + 1);
  cprintf ("%s", txt);
  gotoxy (x, y + 2);
  cprintf ("%s", txt);
  return 0;
}

#endif
