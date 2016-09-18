/*
 * $Id: imgpickpalette.c,v 1.3 1996/03/19 16:10:28 couet Exp $
 *
 * $Log: imgpickpalette.c,v $
 * Revision 1.3  1996/03/19 16:10:28  couet
 * The variable "window" is now compared with 0 instead of NULL.
 *
 * Revision 1.2  1996/03/19 15:27:36  couet
 * The variable "window" is now compared with (Window)NULL instead of NULL.
 * Some compilers need this cast.
 *
 * Revision 1.1.1.1  1996/02/14 13:10:26  mclareni
 * Higz
 *
 */
/*CMZ :  1.20/08 19/01/94  14.30.44  by  O.Couet*/
/*-- Author :    E.Chernyaev   19/01/94*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <stdio.h>
#include <time.h>

#define MAXCOL  256

/***********************************************************************
 *                                                                     *
 * Name: ImgPickPalette                              Date:    22.02.93 *
 * Author: E.Chernyaev (IHEP/Protvino)               Revised:          *
 *                                                                     *
 * Function: Pick palette for image output                             *
 *                                                                     *
 * Input: theDsp - pointer to display structure                        *
 *        window - window                                              *
 *                                                                     *
 * Output: Ncol  - number of colors                                    *
 *         R[]   - red components                                      *
 *         G[]   - green components                                    *
 *         B[]   - blue components                                     *
 *                                                                     *
 ***********************************************************************/
void ImgPickPalette(theDsp, window, Ncol, R, G, B)
           Display *theDsp;
           Window   window;
           int     *Ncol, *R, *G, *B;
{
  int               i, theScr, nmaps, ncolors;
  Window            theRoot, theWin;
  Colormap          cmap, *cmaps;
  XColor            colors[MAXCOL];
  XWindowAttributes theAtt;

  theScr  = DefaultScreen(theDsp);
  theRoot = RootWindow(theDsp, theScr);

  if (window != 0 && window != theRoot)
    theWin  = window;
  else
    theWin  = theRoot;

  XGetWindowAttributes(theDsp, theWin, &theAtt);

  if (theAtt.colormap && theAtt.map_installed) {
    ncolors = theAtt.visual->map_entries;
    cmap    = theAtt.colormap;
  }else{
    ncolors = DisplayCells(theDsp, theScr);
    cmap    = DefaultColormap(theDsp, theScr);
    cmaps   = XListInstalledColormaps(theDsp, theRoot, &nmaps);
    if (nmaps != 0) {
      for (i=0; i<nmaps; i++) {
        if (cmap == cmaps[i]) break;
        if (i == nmaps-1) cmap = cmaps[0];
      }
    }
    XFree((void *) cmaps);
  }

  for (i=0; i<ncolors; i++) {
    colors[i].pixel = i;
    colors[i].pad   = 0;
  }
  XQueryColors(theDsp, cmap, colors, ncolors);

  *Ncol   = ncolors;
  printf("Read %d colors.\n", ncolors);
  for (i=0; i<ncolors; i++) {
    R[i] = colors[i].red;
    G[i] = colors[i].green;
    B[i] = colors[i].blue;
  }
}
