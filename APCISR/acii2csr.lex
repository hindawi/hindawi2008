%{

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
	

#include "apcisr.h"
#include "acii2csr.h"
#include <string.h>
cisr dvn, retval; /*retval used by acii2cisr for returning the rendered cisr*/

%}
%%
.*      { dvn=acii2cisr(yytext,1); cisr_printf_con(dvn); }
%%
