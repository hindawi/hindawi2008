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

#ifndef __ACI2CISR_H__
#define __ACI2CISR_H__
#include "cisr2asc.h"
#include "apcisr.h"
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

  extern cisr retval;
  extern cisr acii2cisr (unsigned char *aci_in, int cpos);
  extern cisr cisr_trim (cisr dvn);
  extern int cisr_printf_con (cisr dvn);

#ifdef __DJGPP
#include <conio.h>
  extern int cisr_printf (int x, int y, cisr dvn);
  extern int cisr_printf_clean (int x, int y, cisr dvn);	/*clean to 78 + print dvn */
  extern int cisr_clean (int x, int y);	/*only clean to 78 */
  extern int c3n_printf (int x, int y, char *txt);
#endif

#ifdef __cplusplus
}
#endif

#endif
