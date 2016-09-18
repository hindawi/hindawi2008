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

        20-Jan-2006, Added the modifications section,
                        Abhishek Choudhary <hi_pedler>,
                        choudhary@indicybers.net

End of modifications.
*/

/* ISCII to APCISR mapping table */

#ifndef __APCISR_H__
#define __APCISR_H__
#define APCISR_LEN 82


#ifdef __cplusplus
extern "C"
{
#endif


  static int _urdha = 0;
  static int _madhya = 3;
  static int _nimna = 6;

  static int _matrik = 0;
  static int _lipik = 1;
  static int _purak = 2;

  typedef struct
  {
    char urdha[1024];		/*originaly 1024 each of the three */
    char madhya[1024];
    char nimna[1024];
    int curpos;
  } cisr;

  typedef struct
  {
    char *rmn;
    char *acii;
    int purna;
    int grid[9];
  } grid9;

  static grid9 apcisr_chrt[APCISR_LEN] = {

    {"_hra", "ØèÏ", 1, {0, 0, 0,
			0, 103, 32,
			0, 0, 0}},

    {"kxa", "³èÖ", 2, {0, 0, 0,
		       0, 40, 2,
		       0, 0, 0}},

    {"_tra", "ÂèÏ", 2, {0, 0, 0,
			0, 41, 2,
			0, 0, 0}},

    {"j_yna", "ºè¼", 2, {0, 0, 0,
			 0, 42, 2,
			 0, 0, 0}},

    {"_dya", "ÄèÍ", 2, {0, 0, 0,
			0, 43, 2,
			0, 0, 0}},

    {"_t_ta", "ÂèÂ", 2, {0, 0, 0,
			 0, 51, 2,
			 0, 0, 0}},

    {"k_ta", "³èÂ", 2, {0, 0, 0,
			0, 48, 1,
			0, 0, 0}},

    {"shra", "ÕèÏ", 2, {0, 0, 0,
			0, 44, 2,
			0, 0, 0}},

    {"_ee_m", "§¢", 1, {0, 1, 16,
			0, 2, 32,
			0, 0, 0}},

    {"_h_ri", "Øß", 1, {0, 0, 0,
			0, 104, 32,
			0, 0, 0}},

    {"_rda", "¿é", 1, {0, 0, 0,
		       0, 19, 32,
		       0, 32, 0}},

    {"_rdha", "Àé", 1, {0, 0, 0,
			0, 20, 32,
			0, 32, 0}},

    {"ru", "ÏÝ", 1, {0, 0, 0,
		     0, 46, 32,
		     0, 0, 0}},

    {"roo", "ÏÞ", 1, {0, 0, 0,
		      0, 47, 32,
		      0, 0, 0}},

    {"r", "Ïè", 1, {0, 0, 0,
		    0, 33, 32,
		    0, 64, 0}},

    {"^ra", "èÏ", 0, {0, 0, 0,
		      0, 64, 0,
		      0, 0, 0}},

    {"_h", "Øè", 1, {0, 0, 0,
		     0, 39, 32,
		     0, 64, 0}},

    {"_a", "¤", 2, {0, 0, 0,
		    0, 1, 2,
		    0, 0, 0}},

    {"_aa", "¥", 2, {0, 0, 0,
		     0, 1, 6,
		     0, 0, 0}},

    {"^aa", "Ú", 0, {0, 0, 0,
		     0, 0, 4,
		     0, 0, 0}},

    {"_i", "¦", 1, {0, 0, 0,
		    0, 2, 32,
		    0, 0, 0}},

    {"^i", "Û", 0, {1, 2, 0,
		    8, 0, 0,
		    0, 0, 0}},

    {"_ee", "§", 1, {0, 1, 0,
		     0, 2, 32,
		     0, 0, 0}},

    {"^ee", "Ü", 0, {0, 1, 2,
		     0, 0, 4,
		     0, 0, 0}},

    {"_u", "¨", 1, {0, 0, 0,
		    0, 3, 32,
		    0, 0, 0}},

    {"^u", "Ý", 0, {0, 0, 0,
		    0, 0, 0,
		    0, 0, 2}},

    {"_oo", "©", 2, {0, 0, 0,
		     0, 4, 1,
		     0, 0, 0}},

    {"^oo", "Þ", 0, {0, 0, 0,
		     0, 0, 0,
		     0, 0, 1}},

    {"_ri", "ª", 1, {0, 0, 0,
		     0, 6, 32,
		     0, 16, 0}},

    {"^_ri", "ß", 0, {0, 0, 0,
		      0, 0, 0,
		      0, 16, 0}},

    {"_ae", "¬", 1, {0, 0, 0,
		     0, 5, 32,
		     0, 0, 0}},

    {"^ae", "á", 0, {0, 4, 0,
		     0, 0, 0,
		     0, 0, 0}},

    {"_ai", "­", 1, {0, 4, 0,
		     0, 5, 32,
		     0, 0, 0}},

    {"^ai", "â", 0, {0, 8, 0,
		     0, 0, 0,
		     0, 0, 0}},

    {"_oa", "°", 2, {0, 0, 4,
		     0, 1, 6,
		     0, 0, 0}},

    {"^oa", "å", 0, {0, 0, 4,
		     0, 0, 4,
		     0, 0, 0}},

    {"_ou", "±", 2, {0, 0, 8,
		     0, 1, 6,
		     0, 0, 0}},

    {"^ou", "æ", 0, {0, 0, 8,
		     0, 0, 4,
		     0, 0, 0}},

    {"_au", "²", 2, {0, 0, 64,
		     0, 1, 6,
		     0, 0, 0}},

    {"^au", "ç", 0, {0, 0, 64,
		     0, 0, 4,
		     0, 0, 0}},

    {"_ao", "®", 1, {0, 64, 0,
		     0, 5, 32,
		     0, 0, 0}},

    {"^ao", "ã", 0, {0, 64, 0,
		     0, 0, 0,
		     0, 0, 0}},

    {"ka", "³", 3, {0, 0, 0,
		    0, 7, 1,
		    0, 0, 0}},

    {"kha", "´", 2, {0, 0, 0,
		     0, 8, 2,
		     0, 0, 0}},

    {"ga", "µ", 2, {0, 0, 0,
		    0, 9, 2,
		    0, 0, 0}},

    {"gha", "¶", 2, {0, 0, 0,
		     0, 10, 2,
		     0, 0, 0}},

    {"_nga", "·", 1, {0, 0, 0,
		      0, 11, 32,
		      0, 0, 0}},

    {"cha", "¸", 2, {0, 0, 0,
		     0, 12, 2,
		     0, 0, 0}},

    {"chha", "¹", 1, {0, 0, 0,
		      0, 13, 32,
		      0, 0, 0}},

    {"ja", "º", 2, {0, 0, 0,
		    0, 14, 2,
		    0, 0, 0}},

    {"jha", "»", 2, {0, 0, 0,
		     0, 15, 2,
		     0, 0, 0}},

    {"_yna", "¼", 2, {0, 0, 0,
		      0, 16, 2,
		      0, 0, 0}},

    {"ta", "½", 1, {0, 0, 0,
		    0, 17, 32,
		    0, 0, 0}},

    {"tha", "¾", 1, {0, 0, 0,
		     0, 18, 32,
		     0, 0, 0}},

    {"da", "¿", 1, {0, 0, 0,
		    0, 19, 32,
		    0, 0, 0}},

    {"dha", "À", 1, {0, 0, 0,
		     0, 20, 32,
		     0, 0, 0}},

    {"_nna", "Á", 2, {0, 0, 0,
		      0, 21, 2,
		      0, 0, 0}},

    {"_ta", "Â", 2, {0, 0, 0,
		     0, 22, 2,
		     0, 0, 0}},

    {"_tha", "Ã", 2, {0, 0, 0,
		      0, 23, 2,
		      0, 0, 0}},

    {"_da", "Ä", 1, {0, 0, 0,
		     0, 24, 32,
		     0, 128, 0}},

    {"_dha", "Å", 2, {0, 0, 0,
		      0, 25, 2,
		      0, 0, 0}},

    {"na", "Æ", 2, {0, 0, 0,
		    0, 26, 2,
		    0, 0, 0}},

    {"pa", "È", 2, {0, 0, 0,
		    0, 27, 2,
		    0, 0, 0}},

    {"pha", "É", 3, {0, 0, 0,
		     0, 28, 1,
		     0, 0, 0}},

    {"ba", "Ê", 2, {0, 0, 0,
		    0, 29, 2,
		    0, 0, 0}},

    {"bha", "Ë", 2, {0, 0, 0,
		     0, 30, 2,
		     0, 0, 0}},

    {"ma", "Ì", 2, {0, 0, 0,
		    0, 31, 2,
		    0, 0, 0}},

    {"ya", "Í", 2, {0, 0, 0,
		    0, 32, 2,
		    0, 0, 0}},

    {"ra", "Ï", 1, {0, 0, 0,
		    0, 33, 32,
		    0, 0, 0}},

    {"la", "Ñ", 2, {0, 0, 0,
		    0, 34, 2,
		    0, 0, 0}},

    {"wa", "Ô", 2, {0, 0, 0,
		    0, 35, 2,
		    0, 0, 0}},

    {"lvra", "Ò", 1, {0, 0, 0,
		      0, 105, 32,
		      0, 0, 0}},

    {"sha", "Õ", 2, {0, 0, 0,
		     0, 36, 2,
		     0, 0, 0}},

    {"xa", "Ö", 2, {0, 0, 0,
		    0, 37, 2,
		    0, 0, 0}},

    {"sa", "×", 2, {0, 0, 0,
		    0, 38, 2,
		    0, 0, 0}},

    {"_ha", "Ø", 1, {0, 0, 0,
		     0, 39, 32,
		     0, 0, 0}},

    {"_m", "¢", 0, {0, 0, 16,
		    0, 0, 0,
		    0, 0, 0}},

    {"_hh", "£", 0, {0, 0, 0,
		     0, 0, 16,
		     0, 0, 0}},

    {"_na", "¡", 0, {0, 0, 32,
		     0, 0, 0,
		     0, 0, 0}},

    {"^za", "é", 0, {0, 0, 0,
		     0, 0, 0,
		     0, 32, 0}},

    {"|", "ê", 1, {0, 0, 0,
		   0, 50, 0,
		   0, 0, 0}},

    {"^", "è", 0, {0, 0, 0,
		   0, 0, 0,
		   0, 0, 64}}
  };
#ifdef __cplusplus
}
#endif

#endif
