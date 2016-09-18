/*
 * $Id: x11int.c,v 1.6 1996/04/23 08:36:54 couet Exp $
 *
 * $Log: x11int.c,v $
 * Revision 2.0 1999/02/27       Mario Stipcevic, line dash styles 3.4q1
 * Revision 1.9 1999/01/01       Mario Stipcevic
 *  Added graphics rotation (ixrotate)
 * Revision 1.8 1998/10/10       Mario Stipcevic
 * Revision 1.7 1997/07/01       Mario Stipcevic
 * ixldgif able to load gifs using standard 6x6x6 color map
 * (216 color indexes, from 32-247 )
 * Revision 1.6  1996/04/23 08:36:54  couet
 * - The test on "display" in ixopnds was not correct. In the case of Paw++
 *   the display is set outside the ixopnds routine and in that case the
 *   graphics contexts was not initialized. Now the variable isdisp is tested.
 *
 * Revision 1.5  1996/04/19 16:23:52  maartenb
 * - Use XFreeFontNames() to free fontlist
 *
 * Revision 1.4  1996/04/19 13:23:00  couet
 * - free(fontlist) was misplaced
 *
 * Revision 1.3  1996/04/19 12:10:31  couet
 * - Free fontlist to avoid memory leak
 *
 * Revision 1.2  1996/04/18 14:36:12  couet
 * - ixopnds exit with return code 0 if the display is already open.
 *
 * - Modify the way the fonts are managed to avoid memory leak.
 *
 * Revision 1.1.1.1  1996/02/14 13:10:26  mclareni
 * Higz
 *
 */
/*CMZ :  2.07/20 12/12/95  11.17.55  by  O.Couet*/
/*-- Author : O.Couet*/
/*
 * Fortran interface to X11 window routines for HIGZ
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

#define forever while(1)
#define LNULL 0L

#define MAXWN 16                 /* maximum number of windows                 */
static struct {
  int      open;                 /* 1 if the window is open, 0 if not         */
  int      motif;                /* 1 if the window is a motif window         */
  int      wid;                  /* window identification number from ixopnwi */
  int      bgcol;                /* window's background color index (patlette)*/
  Drawable motif_window;         /* adress of the Motif window                */
  int      double_buffer;        /* 1 if the double buffer is on, 0 if not    */
  Drawable drawing;              /* drawing area, equal to window or buffer   */
  Drawable window;               /* X11 window                                */
  Drawable buffer;               /* pixmap used for double buffer             */
  int width;                     /* width of the window                       */
  int height;                    /* height of the window                      */
  int clip;                      /* 1 if the clipping is on                   */
  int xclip;                     /* x coordinate of the clipping rectangle    */
  int yclip;                     /* y coordinate of the clipping rectangle    */
  int wclip;                     /* width of the clipping rectangle           */
  int hclip;                     /* height of the clipping rectangle          */
} windows[MAXWN],                /* windows : list of the opened windows      */
  *cws, *tws, *ttws;             /* cws : pointer to the current window       */
                                 /* tws : temporary pointer                   */
                                 /* ttws: yet another temporary pointer       */
static int bitmask[]={1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096,
      8192, 16384, 32768, 65536};/* powers of 2, bitmask in ixwptrq()          */

Drawable oldwin;                 /* allows to save the current opened window  */
static int ispix = 0;            /* if ispix=1 a pixmap has been opened       */

static XClassHint class_hints = {"higz", "HIGZ"}; /* Class name is HIGZ       */

static Display *display;
static int isdisp = 0;           /* if isdisp=1 the display has been opened   */
static Screen *screen;
static int screen_number;
static char vendor[132];         /* output of XServerVendor                   */

static Window (*motif_open) ();  /* external routine to open a motif window   */
static void (*motif_init) ();    /* external routine to init a motif window   */
static void (*motif_close) ();   /* external routine to close a motif window  */

static int external_int = 0;
static Drawable external_window;

static Pixmap pixmp; /* temporary, Mario, delete if not used */

static int do_raise = 0;         /* if do_raise =1 cws is raised in ixupdwi   */

/*
 * colors[1]           : foreground also used for b/w screen
 * colors[0]           : background also used for b/w screen
 * colors[2..MAXCOL-1] : colors which can be set by IXSETCO
 */
static int colored;              /* Flag if color screen                      */
static Colormap colormap;        /* Maba's Colormap                           */

#define MAXCOL 256
static struct {
  int defined;
  unsigned long pixel;
  float red;
  float green;
  float blue;
} colors[MAXCOL] = {0, 0, 255., 255., 255.};

/*
 * Primitives Graphic Contexts global for all windows
 */
#define MAXGC 7
static GC gclist[MAXGC];
static GC *gcline = &gclist[0];  /* Polylines                                 */
static GC *gcmark = &gclist[1];  /* Polymarker                                */
static GC *gcfill = &gclist[2];  /* Fill areas                                */
static GC *gctext = &gclist[3];  /* Text                                      */
static GC *gcinvt = &gclist[4];  /* Inverse text                              */
static GC *gcdash = &gclist[5];  /* Dashed lines                              */
static GC *gcpxmp = &gclist[6];  /* Pixmap management                         */
static GC GCbuff[1];
static GC *gcbuff = &GCbuff[0];  /* GC for buffer pixmap (M.S. 05.98)         */

static unsigned long gc_mask = GCForeground | GCBackground;
static XGCValues gc_return;

static draw_mode = 0;            /* Current draw mode as set by ixdrmde() M.S.*/

/*
 * Input echo Graphic Context global for all windows
 */
static GC gcecho;                           /* input echo                     */
static XGCValues gcechov;

static int fill_hollow;                     /* Flag if fill style is hollow   */
static int fill_border;                     /* Flag to fill border also       */
static Pixmap fill_pattern = (Pixmap)LNULL; /* Fill pattern                   */
static int current_fasi = 0;                /* Current fill area style index  */
static int text_align = 0;                  /* Align text left, center, right */

static int rotate_fl = 0, scale_fl = 0, trans_fl = 0;
static double r_angle = 0;                  /* Rotation angle in radians      */
static double Rsi = 0, Rco = 1, f10=100000; /* Rotation sine and cosine       */
static int xRO = 0, yRO = 0;                /* Central point for rotation     */
static int xOrig = 0, yOrig = 0;            /* Translated new origin          */
static double scalex=1, scaley=1;           /* Scaling matrix                 */

/*
 * Text management
 */
#define MAXFONT 4
static struct {
  XFontStruct *id;
  char name[80];                            /* font name                      */
} font[MAXFONT];                            /* list of fonts loaded           */
static XFontStruct *text_font;              /* current font                   */
static int current_font_number = 0;         /* current font number in font[]  */

/*
 * keep style values for line GC
 */
static int line_width = 0;
static int line_style = LineSolid;
static int cap_style  = CapButt;
static int join_style = JoinMiter;
static char dash_list[16];
static int dash_length = 0;
static int dash_offset = 0;
static int dash_nseg = 1; /* Mario */

/*
 * event masks
 */
static unsigned long mouse_mask = ButtonPressMask   | ButtonReleaseMask |
                                  EnterWindowMask   | LeaveWindowMask   |
                                  PointerMotionMask | KeyPressMask      |
                                  KeyReleaseMask;
static unsigned long keybd_mask = ButtonPressMask | KeyPressMask |
                                  EnterWindowMask | LeaveWindowMask;

/*
 * last cursor positions
 */
static int xloc  = 0;
static int yloc  = 0;
static int xlocp = 0;
static int ylocp = 0;

/*
 * data to create an invisible cursor
 */
static char null_cursor_bits[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static Cursor null_cursor = (Cursor)LNULL;

/*
 * data to create fill area interior style
 */
#define Hatches_type static unsigned char

Hatches_type         p1_bits[] = {
   0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55,
   0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55,
   0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55};
Hatches_type         p2_bits[] = {
   0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11,
   0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11,
   0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11};
Hatches_type         p3_bits[] = {
   0x00, 0x00, 0x44, 0x44, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00, 0x44, 0x44,
   0x00, 0x00, 0x11, 0x11, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00, 0x11, 0x11,
   0x00, 0x00, 0x44, 0x44, 0x00, 0x00, 0x11, 0x11};
Hatches_type         p4_bits[] = {
   0x80, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04,
   0x02, 0x02, 0x01, 0x01, 0x80, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10,
   0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x01, 0x01};
Hatches_type         p5_bits[] = {
   0x20, 0x20, 0x40, 0x40, 0x80, 0x80, 0x01, 0x01, 0x02, 0x02, 0x04, 0x04,
   0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x40, 0x40, 0x80, 0x80, 0x01, 0x01,
   0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10};
Hatches_type         p6_bits[] = {
   0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
   0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
   0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44};
Hatches_type         p7_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff};
Hatches_type         p8_bits[] = {
   0x11, 0x11, 0xb8, 0xb8, 0x7c, 0x7c, 0x3a, 0x3a, 0x11, 0x11, 0xa3, 0xa3,
   0xc7, 0xc7, 0x8b, 0x8b, 0x11, 0x11, 0xb8, 0xb8, 0x7c, 0x7c, 0x3a, 0x3a,
   0x11, 0x11, 0xa3, 0xa3, 0xc7, 0xc7, 0x8b, 0x8b};
Hatches_type         p9_bits[] = {
   0x10, 0x10, 0x10, 0x10, 0x28, 0x28, 0xc7, 0xc7, 0x01, 0x01, 0x01, 0x01,
   0x82, 0x82, 0x7c, 0x7c, 0x10, 0x10, 0x10, 0x10, 0x28, 0x28, 0xc7, 0xc7,
   0x01, 0x01, 0x01, 0x01, 0x82, 0x82, 0x7c, 0x7c};
Hatches_type         p10_bits[] = {
   0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xff, 0xff, 0x01, 0x01, 0x01, 0x01,
   0x01, 0x01, 0xff, 0xff, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xff, 0xff,
   0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff, 0xff};
Hatches_type         p11_bits[] = {
   0x08, 0x08, 0x49, 0x49, 0x2a, 0x2a, 0x1c, 0x1c, 0x2a, 0x2a, 0x49, 0x49,
   0x08, 0x08, 0x00, 0x00, 0x80, 0x80, 0x94, 0x94, 0xa2, 0xa2, 0xc1, 0xc1,
   0xa2, 0xa2, 0x94, 0x94, 0x80, 0x80, 0x00, 0x00};
Hatches_type         p12_bits[] = {
   0x1c, 0x1c, 0x22, 0x22, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x22, 0x22,
   0x1c, 0x1c, 0x00, 0x00, 0xc1, 0xc1, 0x22, 0x22, 0x14, 0x14, 0x14, 0x14,
   0x14, 0x14, 0x22, 0x22, 0xc1, 0xc1, 0x00, 0x00};
Hatches_type         p13_bits[] = {
   0x01, 0x01, 0x82, 0x82, 0x44, 0x44, 0x28, 0x28, 0x10, 0x10, 0x28, 0x28,
   0x44, 0x44, 0x82, 0x82, 0x01, 0x01, 0x82, 0x82, 0x44, 0x44, 0x28, 0x28,
   0x10, 0x10, 0x28, 0x28, 0x44, 0x44, 0x82, 0x82};
Hatches_type         p14_bits[] = {
   0xff, 0xff, 0x11, 0x10, 0x11, 0x10, 0x11, 0x10, 0xf1, 0x1f, 0x11, 0x11,
   0x11, 0x11, 0x11, 0x11, 0xff, 0x11, 0x01, 0x11, 0x01, 0x11, 0x01, 0x11,
   0xff, 0xff, 0x01, 0x10, 0x01, 0x10, 0x01, 0x10};
Hatches_type         p15_bits[] = {
   0x22, 0x22, 0x55, 0x55, 0x22, 0x22, 0x00, 0x00, 0x88, 0x88, 0x55, 0x55,
   0x88, 0x88, 0x00, 0x00, 0x22, 0x22, 0x55, 0x55, 0x22, 0x22, 0x00, 0x00,
   0x88, 0x88, 0x55, 0x55, 0x88, 0x88, 0x00, 0x00};
Hatches_type         p16_bits[] = {
   0x0e, 0x0e, 0x11, 0x11, 0xe0, 0xe0, 0x00, 0x00, 0x0e, 0x0e, 0x11, 0x11,
   0xe0, 0xe0, 0x00, 0x00, 0x0e, 0x0e, 0x11, 0x11, 0xe0, 0xe0, 0x00, 0x00,
   0x0e, 0x0e, 0x11, 0x11, 0xe0, 0xe0, 0x00, 0x00};
Hatches_type         p17_bits[] = {
   0x44, 0x44, 0x22, 0x22, 0x11, 0x11, 0x00, 0x00, 0x44, 0x44, 0x22, 0x22,
   0x11, 0x11, 0x00, 0x00, 0x44, 0x44, 0x22, 0x22, 0x11, 0x11, 0x00, 0x00,
   0x44, 0x44, 0x22, 0x22, 0x11, 0x11, 0x00, 0x00};
Hatches_type         p18_bits[] = {
   0x11, 0x11, 0x22, 0x22, 0x44, 0x44, 0x00, 0x00, 0x11, 0x11, 0x22, 0x22,
   0x44, 0x44, 0x00, 0x00, 0x11, 0x11, 0x22, 0x22, 0x44, 0x44, 0x00, 0x00,
   0x11, 0x11, 0x22, 0x22, 0x44, 0x44, 0x00, 0x00};
Hatches_type         p19_bits[] = {
   0xe0, 0x03, 0x98, 0x0c, 0x84, 0x10, 0x42, 0x21, 0x42, 0x21, 0x21, 0x42,
   0x19, 0x4c, 0x07, 0xf0, 0x19, 0x4c, 0x21, 0x42, 0x42, 0x21, 0x42, 0x21,
   0x84, 0x10, 0x98, 0x0c, 0xe0, 0x03, 0x80, 0x00};
Hatches_type         p20_bits[] = {
   0x22, 0x22, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x44, 0x44,
   0x44, 0x44, 0x44, 0x44, 0x22, 0x22, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
   0x22, 0x22, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44};
Hatches_type         p21_bits[] = {
   0xf1, 0xf1, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f, 0x1f, 0x01, 0x01,
   0x01, 0x01, 0x01, 0x01, 0xf1, 0xf1, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
   0x1f, 0x1f, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
Hatches_type         p22_bits[] = {
   0x8f, 0x8f, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0xf8, 0xf8, 0x80, 0x80,
   0x80, 0x80, 0x80, 0x80, 0x8f, 0x8f, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
   0xf8, 0xf8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
Hatches_type         p23_bits[] = {
   0xAA, 0xAA, 0x55, 0x55, 0x6a, 0x6a, 0x74, 0x74, 0x78, 0x78, 0x74, 0x74,
   0x6a, 0x6a, 0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0x6a, 0x6a, 0x74, 0x74,
   0x78, 0x78, 0x74, 0x74, 0x6a, 0x6a, 0x55, 0x55};
Hatches_type         p24_bits[] = {
   0x80, 0x00, 0xc0, 0x00, 0xea, 0xa8, 0xd5, 0x54, 0xea, 0xa8, 0xd5, 0x54,
   0xeb, 0xe8, 0xd5, 0xd4, 0xe8, 0xe8, 0xd4, 0xd4, 0xa8, 0xe8, 0x54, 0xd5,
   0xa8, 0xea, 0x54, 0xd5, 0xfc, 0xff, 0xfe, 0xff};
Hatches_type         p25_bits[] = {
   0x80, 0x00, 0xc0, 0x00, 0xe0, 0x00, 0xf0, 0x00, 0xff, 0xf0, 0xff, 0xf0,
   0xfb, 0xf0, 0xf9, 0xf0, 0xf8, 0xf0, 0xf8, 0x70, 0xf8, 0x30, 0xff, 0xf0,
   0xff, 0xf8, 0xff, 0xfc, 0xff, 0xfe, 0xff, 0xff};


#define MAXMK 100
static struct {
  int type;
  int n;
  XPoint xy[MAXMK];
} marker;                        /* point list to draw marker          */

/*
 *    Define some marker types ala PAW
 */
static short boxm[5][2] = {-1,-1, -1,1, 1,1, 1,-1, -1,-1};
static float tupm[4][2] = {0.,1., 1.1547,-1., -1.1547,-1., 0.,1.};
static float tdnm[4][2] = {0.,-1., -1.1547,1., 1.1547,1., 0.,-1.}; 

/*
 * Define some line dash styles (1..4)
 */
static short tdash[5][4] = {0,0,0,0,  6,6,0,0,  6,3,1,3,  1,6,0,0,  1,3,0,0};

/*
 *    Own/standard Color Palette corespondance table (in ixldgif())
 */
static unsigned char tpal[256];
static char stdset=0; /* Std. Palette flag (in ixldgif()) */

/*
 *    Set input on or off
 */
void
  setinput(inp)
int inp;
{
  XSetWindowAttributes attributes;
  unsigned long attr_mask = 0;

  if( inp == 1 ) {
    attributes.event_mask = mouse_mask | keybd_mask;
    attr_mask |= CWEventMask;
    XChangeWindowAttributes ( display, cws->window, attr_mask, &attributes);
  }
  else {
     attr_mask = 0;
     attributes.event_mask = NoEventMask;
     attr_mask |= CWEventMask;
     XChangeWindowAttributes ( display, cws->window, attr_mask, &attributes);
  }
}


/*
 * set the foreground color in GC
 */
void
  setcolor( gc, ci )
GC gc;
int ci;
{
  if( colored && ( ci < 0 || ci >= MAXCOL || !colors[ci].defined ) )
    {
     ci = 0;
    }
  else if (!colored && ci < 0 )
    {
     ci = 0;
    }
  else if (!colored && ci > 0 )
    {
     ci = 1;
    }

  XSetForeground( display, gc, colors[ci].pixel );

  /* make sure that foreground and background are different */
  if ( XGetGCValues( display, gc, gc_mask, &gc_return) ) {
     if ( gc_return.foreground == gc_return.background )
        XSetBackground( display, gc, colors[!ci].pixel );
  } else {
     printf("**** Error: Cannot get GC values \n");
  }
}



/*
 *    INTEGER FUNCTION IXOPNDS(HOST)
 *    CHARACTER*(*) HOST  : host name
 *
 *    Open the display. Return -1 if the opening fails
 */
int
  ixopnds( host )
char host[128];
{
  int lenhst;
  Pixmap pixmp1,pixmp2;
  XColor fore,back;
  char **fontlist;
  int fontcount;
  int i;
  extern char *getenv();
 
  lenhst=strlen(host);

/*
 *              Try to open the DISPLAY
 */
  if ( !isdisp ) {
     if( display == NULL ) {
        if( lenhst == 0 ) {
           display = XOpenDisplay( getenv( "DISPLAY" ) );
        } else {
           display = XOpenDisplay( host );
        }
        if( display == NULL ) { fprintf(stderr, "The DISPLAY %s cannot be opened !\n", getenv("DISPLAY") ); return( -1 ); }
     }
  } else {
     return( 0 );
  }

  screen_number = DefaultScreen( display );
  screen        = ScreenOfDisplay( display, screen_number);
  colored       = DisplayPlanes( display, screen_number ) > 1;

  if (colored) colormap = DefaultColormap( display, screen_number);

  colors[1].defined = 1; /* default foreground */
  colors[1].pixel = BlackPixel( display, screen_number );
  colors[0].defined = 1; /* default background */ 
  colors[0].pixel = WhitePixel( display, screen_number );

/*
 *              Inquire the the XServer Vendor
 */
  strcpy (vendor,XServerVendor(display));

/*
 *              Create primitives graphic contexts
 */
  for ( i = 0; i < MAXGC; i++ )
     gclist[i] = XCreateGC( display, RootWindow( display, screen_number ),
                             0, NULL );
     GCbuff[0] = XCreateGC( display, RootWindow( display, screen_number ),
                             0, NULL );
  if ( XGetGCValues( display, *gctext, gc_mask, &gc_return) ) {
     XSetForeground( display, *gcinvt, gc_return.background );
     XSetBackground( display, *gcinvt, gc_return.foreground );
  } else {
     printf("**** Error: Cannot get GC values \n");
  }

/*
 *              Create input echo graphic context
 */
  gcechov.foreground = BlackPixel( display, screen_number );
  gcechov.background = WhitePixel( display, screen_number );
  if(strstr(vendor,"Hewlett")) {
     gcechov.function   = GXxor;
  } else {
     gcechov.function   = GXinvert;
  }
  gcecho = XCreateGC( display, RootWindow( display, screen_number ),
                      GCForeground | GCBackground | GCFunction,
                      &gcechov );
/*
 *              Load a default Font
 */
  for ( i = 0; i < MAXFONT; i++ ) {
     font[i].id = NULL;
     strcpy( font[i].name, " " );
  }
  fontlist = XListFonts( display, "*courier*", 1, &fontcount );
  if ( fontcount != 0 ) {
     font[current_font_number].id = XLoadQueryFont( display, fontlist[0] );
     text_font = font[current_font_number].id;
     strcpy( font[current_font_number].name, "*courier*" );
     current_font_number++;
     XFreeFontNames(fontlist);
  } else {
     printf("No default font loaded \n");
  }
/*
 *              Create a null cursor
 */
  pixmp1 = XCreateBitmapFromData(display,
                                 RootWindow( display, screen_number ),
                                 null_cursor_bits, 16, 16);
  pixmp2 = XCreateBitmapFromData(display,
                                 RootWindow( display, screen_number ),
                                 null_cursor_bits, 16, 16);
  null_cursor = XCreatePixmapCursor(display,pixmp1,pixmp2,&fore,&back,0,0);

  isdisp = 1;
  return( 0 );
}


/*
 *    INTEGER FUNCTION IXOPNWI(X,Y,W,H,TITLE,FLAG)
 *    INTEGER X,Y         : initial window position
 *    INTEGER W,H         : initial window width and height
 *    CHARACTER*(*) TITLE : window title
 *    INTEGER FLAG        : FLAG<>1 allows to open a non MOTIF window
 *                          even if ixmotif has been called.
 *
 *    Open window and returns window number which can be used with IXSELWIN
 *    return -1 if window creation fails
 */
int
  ixopnwi( x, y, w, h, title, flag)
int x, y;
int w, h;
char title[80];
int flag;
{
  XSetWindowAttributes attributes;
  unsigned long attr_mask = 0;
  char long_title[256];
  char host_name[64];
  XWMHints wm_hints;
  XSizeHints size_hints;
  XEvent event;
  int wid, i;
  int xval, yval;
  unsigned int wval, hval, border, depth;
  Window root;

  if (external_int == 1) {
     XGetGeometry( display, external_window, &root,
                   &xval, &yval, &wval, &hval, &border, &depth );
  }
  else {
     xval = x;
     yval = y;
     wval = w;
     hval = h;
  }

/*
 *              Select next free window number
 */
      for ( wid = 0; wid < MAXWN; wid++ )
         if ( !windows[wid].open ) {
            windows[wid].open = 1;
            cws = &windows[wid];
            cws->wid = wid;
            break;
         }
/*
 *              Create a Motif window
 */
      if (motif_open != NULL && flag == 0) {
          cws->motif_window = (*motif_open) (wid+1, xval, yval, wval, hval);
          if ( !cws->motif_window ) return( -1 );
          XGetGeometry( display, cws->motif_window, &root,
                        &xval, &yval, &wval, &hval, &border, &depth );
      }

      if ( wid == MAXWN ) return( -1 );
/*
 *              Create window
 */
      attributes.background_pixel = colors[0].pixel;
      attr_mask |= CWBackPixel;
      attributes.border_pixel = colors[1].pixel;
      attr_mask |= CWBorderPixel;
      attributes.event_mask = NoEventMask;
      attr_mask |= CWEventMask;
      attributes.backing_store = Always;
      attr_mask |= CWBackingStore;
      if ( colored ) {
         attributes.colormap = colormap;
         attr_mask |= CWColormap;
      }
      if (motif_open != NULL && flag == 0) {
         cws->window = XCreateWindow(display, cws->motif_window,
                       xval, yval, wval, hval, 0, CopyFromParent,
                       InputOutput, CopyFromParent,
                       attr_mask, &attributes );
         cws->motif = 1;
      } else {
         if (external_int == 1) {
            cws->window = XCreateWindow(display, external_window,
                          xval, yval, wval, hval, 0, CopyFromParent,
                          InputOutput, CopyFromParent,
                          attr_mask, &attributes );
            external_int = 0;
            cws->motif = 1;
            cws->motif_window = external_window;
         } else {
            cws->motif = 0;
            cws->window = XCreateWindow(display,
                          RootWindow( display, screen_number),
                          xval, yval, wval, hval, 1, CopyFromParent,
                          InputOutput, CopyFromParent,
                          attr_mask, &attributes );
            strcpy( long_title, title );
            if (long_title[0] != '-') {
               strcat( long_title, " @ " );
               gethostname( host_name, sizeof( host_name ) );
               strcat( long_title, host_name );
            } else {
               long_title[0] = ' ';
            }
            XStoreName( display, cws->window, long_title );
            XSetIconName( display, cws->window, long_title );
         }
      }
/*
 *              Set window manager hints
 */
      memset( &wm_hints, 0, sizeof(wm_hints));
      wm_hints.flags = InputHint;
      wm_hints.input = False;  /* don't grab input focus */
      XSetWMHints( display, cws->window, &wm_hints );
/*
 *              Set window size hints
 */
      memset( &size_hints, 0, sizeof(size_hints));
      size_hints.flags = USPosition | USSize;
      size_hints.x = xval;
      size_hints.y = yval;
      size_hints.width = wval;
      size_hints.height = hval;
      XSetNormalHints( display, cws->window, &size_hints );

      XMoveWindow( display, cws->window, xval, yval );
      XResizeWindow( display, cws->window, wval, hval );
      XMapWindow( display, cws->window );
/*
      XFlush( display );
 *              Set the Class Hint
 */
      XSetClassHint(display, cws->window, &class_hints);
/*
 *              Initialise the window structure
 */
      cws->drawing        = cws->window;
      cws->buffer         = (Drawable)NULL;
      cws->double_buffer  = (int)NULL;
      cws->clip           = (int)NULL;
      cws->width          = wval;
      cws->height         = hval;
      cws->bgcol          = 0;

/*    Raise (and unclip) the window */

      for( i = 0; i < MAXGC; i++ ) XSetClipMask( display, gclist[i], None );
      XRaiseWindow( display, cws->window );

  XFlush( display );
  XSync( display, 1 );

  return( wid );
}

/*
 *    SUBROUTINE IXRSCWI(WID, W, H)
 *    INTEGER WID  : Window identifier.
 *    INTEGER W    : Width
 *    INTEGER H    : Heigth
 *
 *    Rescale the window WID
 */
void
   ixrscwi(wid, w, h)
  int wid;
  int w;
  int h;
{
  int i;

  tws = &windows[wid];
/*
  if (!tws->open) return;
*/

  XResizeWindow( display, tws->window, w, h );

  if( tws->double_buffer ) {
     XFreePixmap(display,tws->buffer);
     tws->buffer = XCreatePixmap(display, RootWindow( display, screen_number),
                   w, h, DefaultDepth(display,screen_number));
     for( i = 0; i < MAXGC; i++ ) XSetClipMask( display, gclist[i], None );
     setcolor( *gcpxmp, 0);
     XFillRectangle( display, tws->buffer, *gcpxmp, 0, 0, w, h);
     setcolor( *gcpxmp, 1);
     tws->drawing = tws->buffer;
  }
  tws->width  = w;
  tws->height = h;
  XFlush( display );
}

/*
 *    SUBROUTINE IXMOVWI(WID, X, Y)
 *    INTEGER WID  : Window identifier.
 *    INTEGER X    : Width
 *    INTEGER Y    : Heigth
 *
 *    Move the window WID
 */
void
   ixmovwi(wid, x, y)
  int wid;
  int x;
  int y;
{
  int i;

  tws = &windows[wid];
  if (!tws->open) return;

  XMoveWindow( display, tws->window, x, y );
}

/*
 *    INTEGER FUNCTION IXOPNPX(W, H)
 *    INTEGER W,H : Width and height of the pixmap.
 *
 *    Open a new pixmap.
 */
int
  ixopnpx(w, h)
int w, h;
{
  Drawable pixtemp;
  Window root;
  unsigned int wval, hval;
  int xx, yy, i ;
  unsigned int ww, hh, border, depth;
  wval = w;
  hval = h;

  XGetGeometry( display, cws->drawing, &root, &xx, &yy, &ww, &hh, &border, &depth );

/*
  oldwin = cws->drawing;
  pixtemp = XCreatePixmap(display, RootWindow( display, screen_number),
            wval, hval, DefaultDepth(display,screen_number));
  ispix = 1;
  for( i = 0; i < MAXGC; i++ )
    XSetClipMask( display, gclist[i], None );
  cws->drawing = pixtemp;
  setcolor( *gcpxmp, 0);
  XFillRectangle( display, pixtemp, *gcpxmp,0 ,0 ,ww ,hh);
  setcolor( *gcpxmp, 1);
*/


  pixtemp = cws->drawing;
  return ( pixtemp );
}


/*
 *    SUBROUTINE IXCLRPX(PIX)
 *    INTEGER PIX : Pixmap address
 *
 *    Clear the pixmap PIX.
 */
void
  ixclrpx(pix )
Drawable pix;
{
  Window root;
  int xx, yy;
  unsigned int ww, hh, border, depth;
  XGetGeometry( display, pix, &root, &xx, &yy, &ww, &hh, &border, &depth );
  setcolor( *gcpxmp, 0);
  XFillRectangle( display, pix, *gcpxmp,0 ,0 ,ww ,hh);
  setcolor( *gcpxmp, 1);
  XFlush( display );
}


/*
 *    SUBROUTINE IXCLPX
 *
 *    Close the current opened pixmap.
 */
void
  ixclpx()
{
  XFlush( display );
  cws->drawing = oldwin;
  ispix = 0;
}


/*
 *    SUBROUTINE IXCPPX(PIX, XPOS, YPOS)
 *    INTEGER PIX : Pixmap address
 *    INTEGER XPOS, YPOS : Pixmap Position
 *
 *    Copy the pixmap PIX at the position XPOS YPOS.
 */
void
  ixcppx(pix, xpos, ypos)
Drawable pix;
int xpos, ypos;
{
  Window root;
  int xx, yy;
  XEvent event;
  unsigned int ww, hh, border, depth;
  XGetGeometry( display, pix, &root, &xx, &yy, &ww, &hh, &border, &depth );

  XCopyArea(display,pix,cws->window,*gcpxmp,0,0,ww,hh,xpos,ypos);
/*
printf("pix geometry =  %d %d %d %d %d %d %d pos = %d %d que=%d\n", pix, xx, yy, ww, hh, border, depth, xpos, ypos, XEventsQueued( display, QueuedAlready));
*/
  XFlush( display );
}


/*
 *    SUBROUTINE IXWRPX(PIX,W,H,LENNAME,PXNAME)
 *    INTEGER PIX : Pixmap address
 *    INTEGER W,H : Width and height of the pixmap.
 *    INTEGER LENNAME     : pixmap name length
 *    CHARACTER*(*) PXNAME: pixmap name
 *
 *    Write the pixmap IPX in the bitmap file PXNAME.
 */
void
  ixwrpx(pix, w, h, pxname )
char pxname[128];
Drawable pix;
int w, h;
{
  unsigned int wval, hval;
  wval = w;
  hval = h;
  XWriteBitmapFile(display, pxname, pix, wval, hval, -1, -1);
}

/*
void
  ixrdpx( pix, w, h, pxname )
Drawable *pix;
int *w, *h;
char pxname[128];
{
  int x_hot_return, y_hot_return;
  unsigned int width, height;
  char data[10000];
  int i;
  Window root;
  int xx, yy;
  unsigned int ww, hh, border, depth=1;
  unsigned long fg = 0, bg = 1;

  
  i=XReadBitmapFileData(pxname,&width,&height,data,&x_hot_return,&y_hot_return);
  *w = width;
  *h = height;
  *pix=XCreatePixmapFromBitmapData(display,RootWindow( display, screen_number),data,width,height,fg,bg,depth);
  XGetGeometry( display, *pix, &root, &xx, &yy, &ww, &hh, &border, &depth );

  printf("XReadBitmapFile: %d %d %d depth = %d\n", i, BitmapSuccess, *pix, depth);
}
*/

/*
 *    SUBROUTINE IXWIPX(PIX, XPOS, YPOS)
 *    INTEGER PIX : Pixmap address
 *    INTEGER XPOS, YPOS : Position in the current window
 *
 *    Copy the area at the position XPOS YPOS in the current
 *    window in the pixmap PIX. The area copied has the size
 *    of the pixmap PIX.
 */
void
  ixwipx(pix, xpos, ypos )
Drawable pix;
int xpos, ypos;
{
  Window root;
  int xx, yy;
  unsigned int ww, hh, border, depth;
  XGetGeometry( display, cws->window, &root, &xx, &yy, &ww, &hh, &border, &depth );

printf("window Depth = %d\n", depth);
  XGetGeometry( display, pix, &root, &xx, &yy, &ww, &hh, &border, &depth );
printf("pix Depth = %d\n", depth);
  XCopyArea(display,cws->window,pix,*gcpxmp,xpos,ypos,ww,hh,0,0);
}

/* IXGETPX 
 *
 */
Drawable
  ixgetpx( xpos, ypos, width, height )
int xpos, ypos, width, height;
{
  Window root;
  Drawable pix;
  int xx, yy;
  unsigned int ww, hh, border, depth;

  pix = XCreatePixmap(display, RootWindow( display, screen_number),
        width, height, DefaultDepth(display,screen_number));
/*
  XGetGeometry( display, cws->window, &root, &xx, &yy, &ww, &hh, &border, &depth );
printf("window Depth = %d\n", depth);
  XGetGeometry( display, pix, &root, &xx, &yy, &ww, &hh, &border, &depth );
printf("pix Depth = %d\n", depth);
*/
  XCopyArea(display, cws->window, pix, *gcpxmp, xpos,ypos, width,height, 0,0);

  return (pix);
}

/*
 *    SUBROUTINE IXRMPX(PIX)
 *    INTEGER PIX : Pixmap adress
 *
 *    Remove the pixmap PIX.
 */
void
  ixrmpx(pix)
Drawable pix;
{
  XFreePixmap(display,pix);
}


/*
 *    SUBROUTINE IXCLSDS
 *
 *    Delete all windows and close connection
 */
void
  ixclsds()
{
  int Motif = (int)NULL;
  int i;

  if(display == NULL) return;

  for( i = 0; i < MAXFONT; i++ ) {
    if ( font[i].id ) {
       XFreeFont ( display, font[i].id);
       font[i].id = NULL;
    }
  }

  for( i = 0; i < MAXWN; i++ ) {
    windows[i].open = 0;
    if( windows[i].motif )
       Motif = 1;
  }
  if(display != NULL && Motif == (int)NULL) XCloseDisplay( display );
  display = NULL;
  cws     = NULL;
  tws     = NULL;
  isdisp  = 0;
}


/*
 *    SUBROUTINE IXCLSWI
 *
 *    Delete current window
 *    Modified by Mario Stipcevic to close specific or current window
 *    win >= MAXWN return without any action
 *    win <  0     close the current window
 *    win          close the window 'wid', if it exists. If not, just exit.
 *                 If closed window is the last, perform ixclsds()
 */
void
  ixclswi(win)
int win;
{
  int wid, flag;
  XEvent event;

  if (win >= MAXWN) return;
  if (win < 0) tws = cws;
  if (win >= 0 &&  windows[win].open) {
   tws = &windows[win];
  } else {
   if (win >= 0) return;
  }
  if (tws == cws) flag = 1;

  XDestroyWindow( display, tws->window );

  if( tws->double_buffer ) XFreePixmap( display, tws->buffer);

  if (tws->motif && motif_close != NULL) {
      for( wid = 0; wid < MAXWN; wid++ ) {
           if (tws->motif_window == windows[wid].motif_window) {
               (*motif_close) (wid+1);
               break; }
      }
  }

  tws->open = (int)NULL;

  XFlush( display );
  while (XEventsQueued(display,QueuedAlready) > 0) XNextEvent(display,&event);

  if (flag) {

   for( wid = MAXWN - 1; wid >= 0; wid-- )
    if( windows[wid].open ) {
     cws = &windows[wid];
     return;
    }

    ixclsds(); /* no open window left */
   }
}


/*
 *    SUBROUTINE IXSELWI(WID)
 *    INTEGER WID : window number returned by IXOPNWI
 *
 *    Select and raise a window to which subsequent output is directed
 */
void
  ixselwi( wid )
int wid;
{
  XRectangle region;
  int i, bcolor;
   
  if (wid != -1) {
   if( wid < 0 || wid >= MAXWN || !windows[wid].open ) return;
   cws = &windows[wid];
  }

  if ( cws->clip && !ispix && !cws->double_buffer ) {
    region.x      = cws->xclip;
    region.y      = cws->yclip;
    region.width  = cws->wclip;
    region.height = cws->hclip;
    for( i = 0; i < MAXGC; i++ )
      XSetClipRectangles( display, gclist[i], 0, 0, &region, 1, YXBanded );
  } else {
    for( i = 0; i < MAXGC; i++ )
      XSetClipMask( display, gclist[i], None );
  }

  /* Set background color for this window (Mario S.) */
  bcolor = cws->bgcol;
  if (bcolor == 0) {
   colors[0].pixel = WhitePixel( display, screen_number );
  } else {
   colors[0] = colors[bcolor];
  }
  XSetWindowBackground( display, cws->drawing, colors[0].pixel );

  XRaiseWindow( display, cws->window );
  XFlush( display );
}


/*
 *    SUBROUTINE IXGETGE(WID,X,Y,W,H)
 *    INTEGER WID : window identifier
 *    INTEGER X,Y : window position (output)
 *    INTEGER W,H : window size (output)
 *
 *    Return position and size of window wid
 *    if wid < 0 the size of the display is returned
 */
void
   ixgetge( wid, x, y, w, h )
int wid;
int *x, *y;
unsigned int *w, *h;
{
  Window temp_win;
  Display *tmp_display;

  /* If workstation not opened, return root display geometry */
  if (display == NULL) {
    *x = 0;
    *y = 0;
    *w = 0;
    *h = 0;
    if( wid < 0 ) {
     tmp_display = XOpenDisplay( getenv( "DISPLAY" ) );
     screen_number = DefaultScreen( tmp_display );
     *w = DisplayWidth(tmp_display,screen_number);
     *h = DisplayHeight(tmp_display,screen_number);
     XCloseDisplay( tmp_display );
     return;
    }
  }
  if( wid < 0 ) {
    *x = 0;
    *y = 0;
    *w = DisplayWidth(display,screen_number);
    *h = DisplayHeight(display,screen_number);
  } else {
    Window root;
    unsigned int border, depth;
    unsigned int width, height;

    tws = &windows[wid];
    XGetGeometry(display, tws->drawing, &root, x, y,
                 &width, &height, &border, &depth);
    (void) XTranslateCoordinates (display, tws->window,
                                 RootWindow( display, screen_number),
                                   0, 0, x, y, &temp_win);
    if( width > 0 && height > 0 ) {
      tws->width  = width;
      tws->height = height;
    }
    if( width > 1 && height > 1 ) {
      *w=tws->width-1;
      *h=tws->height-1;
    } else {
      *w=1;
      *h=1;
    }
  }
}


/*
 *    SUBROUTINE IXGETPL(NPLANES)
 *    INTEGER NPLANES     : number of bit planes
 *
 *    Get maximum number of planes
 */
void
   ixgetpl(nplanes)
int *nplanes;
{
   *nplanes = DisplayPlanes( display, screen_number );
}


/*
 *    SUBROUTINE IXRSIZ(WID)
 *    INTEGER WID     : window to be resized
 *
 *    Resize the current window if necessary
 */
void
   ixrsiz(wid)
int wid;
{
  int i;
  int xval, yval;
  Window root;
  unsigned int wval, hval, border, depth;

  tws = &windows[wid];

  if( !tws->motif ) return;

  XGetGeometry( display, tws->motif_window, &root,
                &xval, &yval, &wval, &hval, &border, &depth );
  XResizeWindow( display, tws->window, wval, hval );

  if( tws->double_buffer ) {
     XFreePixmap(display,tws->buffer);
     tws->buffer = XCreatePixmap(display, RootWindow( display, screen_number),
                   wval, hval, DefaultDepth(display,screen_number));
     for( i = 0; i < MAXGC; i++ ) XSetClipMask( display, gclist[i], None );
     setcolor( *gcpxmp, 0);
     XFillRectangle( display, tws->buffer, *gcpxmp, 0, 0, wval, hval);
     setcolor( *gcpxmp, 1);
     tws->drawing        = tws->buffer;
  }
  tws->width = wval;
  tws->height = hval;
}


/*
 *    SUBROUTINE IXGETWI(WKID,IDG)
 *    INTEGER WKID : Workstation identifier (input)
 *    INTEGER IDG  : Window identifier (output)
 *
 *    Return the X11 window identifier according to the
 *    Window id returned by IXOPNWI.
 */
void
   ixgetwi (wkid, idg)
   int wkid;
   Window *idg;
{
   *idg = windows[wkid].window;
}

/*
 *    SUBROUTINE IXCLRWI
 *
 *    Clear current window
 */
void
  ixclrwi()
{

  if ( !cws->buffer ) {
/*
  if ( !ispix && !cws->double_buffer ) {
     XSetWindowBackground( display, cws->drawing, colors[0].pixel );
*/
     XClearWindow( display, cws->drawing );
  } else {
     setcolor( *gcbuff, 0);
     XFillRectangle( display, cws->buffer, *gcbuff,
                     0, 0, cws->width, cws->height );
     setcolor( *gcbuff, 1);
  }
  do_raise = 1;
  XFlush( display );
}


/*
 * Change the background for the current window to bcolor Pallete index
*/
void
  ixsetbg( bcolor )
  int bcolor;
{
  cws->bgcol = bcolor;
  if (bcolor == 0) { 
   colors[0].pixel = WhitePixel( display, screen_number );
  } else {
   colors[0] = colors[bcolor];
  }

  XSetWindowBackground( display, cws->drawing, colors[0].pixel );
  do_raise = 1;
}

/*
 *    SUBROUTINE IXUPDWI(MODE)
 *    INTEGER MODE : (1) or (11) The window is raised
 *                   (0) or (10) The window is not raised
 *                   (0) or (1)  no synchonisation between client and server
 *                  (10) or (11) synchonisation between client and server
 *
 *    Update display and raise current window to top of stack.
 *    Synchronise client and server once (not permanent).
 *    Copy the pixmap cws->buffer on the window cws-> window
 *    if the double buffer is on.
 */
void
  ixupdwi(mode)
  int mode;
{
  if (display == NULL) return;
  if ( mode == 1 || mode == 11 ) {
     if ( do_raise ) {
        XRaiseWindow( display, cws->window );
        do_raise = 0;
     }
  }
  if ( cws->buffer ) {
     XCopyArea( display, cws->buffer, cws->window,
                *gcbuff, 0, 0, cws->width, cws->height, 0, 0 );
  }
  if ( mode == 0 || mode == 1 ) {
    XFlush( display );
  } else {
    XSync( display ,0);
  }
}


/*
 *    SUBROUTINE IXCLIP(WID,X,Y,W,H)
 *    INTEGER WID : Window indentifier
 *    INTEGER X,Y : origin of clipping rectangle
 *    INTEGER W,H : size of clipping rectangle;
 *
 *    Set clipping region for the window wid
 */
void
  ixclip( wid, x, y, w, h )
int wid;
int x, y;
int w, h;
{
  XRectangle region;
  int i;

  tws = &windows[wid];
  tws->xclip = x;
  tws->yclip = y;
  tws->wclip = w;
  tws->hclip = h;
  tws->clip  = 1;
  if ( cws->clip && !ispix && !cws->double_buffer ) {
    region.x      = cws->xclip;
    region.y      = cws->yclip;
    region.width  = cws->wclip;
    region.height = cws->hclip;
    for( i = 0; i < MAXGC; i++ )
      XSetClipRectangles( display, gclist[i], 0, 0, &region, 1, YXBanded );
  }
}

/* BASIC implementation of clipping function: */
void
  ixclip1( wid, x1, y1, x2, y2 )
int wid;
int x1, y1;
int x2, y2;
{
  int w, h;
  w = x2 - x1 + 1;
  h = y2 - y1 + 1;
  ixclip( wid, x1, y1, w, h);   
}

/*
 *    SUBROUTINE IXNOCLI(WID)
 *    INTEGER WID : Window indentifier
 *
 *    Switch off the clipping for the window wid
 */
void
  ixnocli(wid)
int wid;
{
  int i;

  tws       = &windows[wid];
  tws->clip = (int)NULL;

  for( i = 0; i < MAXGC; i++ )
    XSetClipMask( display, gclist[i], None );
}


/*
 *    SUBROUTINE IXSETCO(CINDEX,R,G,B)
 *    INTEGER CINDEX : color index
 *    REAL R,G,B     : red, green, blue intensities between 0.0 and 1.0
 *
 *    Set color intensities for given color index
 */
#define BIGGEST_RGB_VALUE 65535

void
  ixsetco( cindex, r, g, b )
int cindex;
int r, g, b;
{
  XColor xcol;

  if( colored && cindex >= 0 && cindex < MAXCOL ) {
   xcol.red   = (unsigned short)( (r /255.) * BIGGEST_RGB_VALUE );
   xcol.green = (unsigned short)( (g /255.) * BIGGEST_RGB_VALUE );
   xcol.blue  = (unsigned short)( (b /255.) * BIGGEST_RGB_VALUE );
   xcol.flags = DoRed || DoGreen || DoBlue;
   if( colors[cindex].defined == 1 ) {
    /* printf("Warning: color = %d redefined\n", cindex); */
    colors[cindex].defined = 0;
    XFreeColors(display, colormap, &colors[cindex].pixel, 1, NULL);
   }
   if( XAllocColor( display, colormap, &xcol ) != (Status)NULL ) {
    colors[cindex].defined = 1;
    colors[cindex].pixel   = xcol.pixel;
    colors[cindex].red     = r;
    colors[cindex].green   = g;
    colors[cindex].blue    = b;
   }
  }
}


void
  ixgetcol( index, r, g, b)
int index; /* input */
int *r, *g, *b; /* output */
{
  *r = colors[index].red;
  *g = colors[index].green;
  *b = colors[index].blue;
  return ;
}

/*
 *    SUBROUTINE IXSETLN(WIDTH)
 *    INTEGER WIDTH : line width in pixels
 *
 *    Set line width
 */
void
  ixsetln( width )
int width;
{
  if( width == 1) {
     line_width = 0;
  }
  else {
     line_width = width;
  }
  XSetLineAttributes( display, *gcline, line_width,
              line_style, cap_style, join_style );
  XSetLineAttributes( display, *gcdash, line_width,
              line_style, cap_style, join_style );
}


/*
 *    SUBROUTINE IXSETLS(N,DASH)
 *    INTEGER N       : length of dash list
 *    INTEGER DASH(N) : dash segment lengths
 *
 *    Set line style:
 *    if N.EQ.0 use solid lines
 *    if N.LT.0 use predefined tdash[][] dashed style of type -N (1..4)
 *    if N.GT.0 use dashed lines described by DASH(N), 2 <= N <= 10 (even)
 *    e.g. N=4,DASH=(6,3,1,3) gives a dashed-dotted line with 6 dots,
 *    followed by 3 blanks followed by 1 dot followed by 3 blanks
 */
void
  ixsetls( n, dash )
int n;
short *dash;
{
  int i, j;
  if (n > 16) n = 16;
  if( n == 0 ) {
    line_style = LineSolid;
    dash_nseg = 1;
    XSetLineAttributes( display, *gcline, line_width,
              line_style, cap_style, join_style );
    return;
  }

  dash_length = 0;
  dash_offset = 0;
  line_style = LineOnOffDash;

  if ( n > 0 ) /* input type */
  {
   dash_nseg = n;
   for( i = 0; i < n; i++ ) {
    dash_list[i] = dash[i];
    dash_length += dash_list[i];
   }
  }

  if ( n < 0 ) /* predefined types */
  {
   n = -n;
   switch (n) {
           case 1: j = 2;
                   break;
           case 2: j = 4;
                   break;
           case 3: j = 2;
                   break;
           case 4: j = 2;
                   break;
   }
   dash_nseg = j;
   for( i = 0; i < j; i++ ) {
    dash_list[i] = tdash[n][i];
    dash_length += dash_list[i];
   }
  }
  XSetLineAttributes( display, *gcdash, line_width,
             line_style, cap_style, join_style );
}

void
  ixsetld( lt )
int lt;
/* Set dashing style via 16-bit integer mask. Use lower 16 bits */
{
 int i=0, j, n=0, m, mask=1, flag, offset=0;
 short dash[16];

 flag=lt & mask;
 while(i<16) {
  j = 0;
  m = lt & mask;
  while(i<16 && (lt & mask)==m) {
   i++; j++; lt >>= 1;
  }
  dash[n++]=j;
 }
 if(!flag) {
  offset = dash[0];
  for(i=1; i<n; i++) dash[i-1]=dash[i];
  dash[n-1]=offset;
  offset=-offset;
  if(n%2 != 0) {n--; dash[n-1]+=dash[n];}
 } else {
  if(n%2 != 0) {n--; dash[0]+=dash[n]; offset = dash[n];}
 }
/*
 printf("n =%d offset=%d\n", n, offset);
 for(i=0; i<n; i++) printf("i=%d dash=%d\n", i, dash[i]);
*/
 ixsetls( n, dash );
 dash_offset = offset;
}


/*
 *    SUBROUTINE IXSETLC(CINDEX)
 *    INTEGER CINDEX : color index defined my IXSETCOL
 *
 *    Set color index for lines
 */
void
  ixsetlc( cindex )
int cindex;
{
  setcolor( *gcline, cindex );
  setcolor( *gcdash, cindex );
}


/*
 *    SUBROUTINE IXLINE(N,XY)
 *    INTEGER N         : number of points
 *    INTEGER*2 XY(2,N) : list of points
 *
 *    Draw a line through all points
 */
void
  ixline( n, Rxy )
int n;
XPoint *Rxy;
{
 XPoint *xy;
 int i;

 if (rotate_fl || scale_fl || trans_fl) {
  xy = (XPoint *)calloc(n, sizeof(XPoint));
  for (i = 0; i < n; i++) {
   xy[i].x=xOrig+Rco*scalex*(Rxy[i].x-xRO)+Rsi*scaley*(Rxy[i].y-yRO);
   xy[i].y=yOrig-Rsi*scalex*(Rxy[i].x-xRO)+Rco*scaley*(Rxy[i].y-yRO);
  }
 } else {
  xy = Rxy;
 }
/*
printf("1: %d, %d, %d, %d, %d, %d, %d, %d\n",Rxy[0].x,Rxy[0].y,Rxy[1].x,Rxy[1].y, Rxy[2].x,Rxy[2].y, Rxy[3].x,Rxy[3].y);
printf("2: %d, %d, %d, %d, %d, %d, %d, %d\n",xy[0].x,xy[0].y,xy[1].x,xy[1].y, xy[2].x,xy[2].y,xy[3].x,xy[3].y);
printf("3: Rsi=%f, Rco=%f, sx=%f, sy=%f, xO=%d, yO=%d, xRO=%d, yRO=%d\n",Rsi, Rco, scalex, scaley, xOrig, yOrig, xRO, yRO);
*/

 if ( n > 1 )
    {
       if( line_style == LineSolid )
         if (cws->buffer) {
          XDrawLines( display, cws->buffer, *gcline, xy, n, CoordModeOrigin );
         } else {
          XDrawLines( display, cws->drawing, *gcline, xy, n, CoordModeOrigin );
         }
       else {
         int i;
         XSetDashes( display, *gcdash,
             dash_offset, dash_list, dash_nseg );
         XDrawLines( display, cws->drawing, *gcdash, xy, n, CoordModeOrigin );

         /* calculate length of line to update dash offset */
/* Doesn't work well anyway (Mario)
         for( i = 1; i < n; i++ ) {
          int dx = xy[i].x - xy[i-1].x;
          int dy = xy[i].y - xy[i-1].y;
          if( dx < 0 ) dx = - dx;
          if( dy < 0 ) dy = - dy;
          dash_offset += dx > dy ? dx : dy;
         }
         dash_offset %= dash_length;
*/
       }
    }
 else
    {
     XDrawPoint(display, cws->drawing,
        line_style == LineSolid ? *gcline : *gcdash, xy[0].x, xy[0].y);
    }
   do_raise = 1;
}


/*
 *    SUBROUTINE IXSETMS(TYPE,N,XY)
 *    INTEGER TYPE      : marker type
 *    INTEGER N         : length of marker description
 *    INTEGER*2 XY(2,N) : list of points describing marker shape
 *
 *    Set marker style:
 *    if N.EQ.0 marker is a single point
 *    if TYPE.EQ.0 marker is hollow circle of diameter N
 *    if TYPE.EQ.1 marker is filled circle of diameter N
 *    if TYPE.EQ.2 marker is a hollow polygon describe by line XY
 *    if TYPE.EQ.3 marker is a filled polygon describe by line XY
 *    if TYPE.EQ.4 marker is described by segmented line XY
 *    e.g. TYPE=4,N=4,XY=(-3,0,3,0,0,-3,0,3) sets a plus shape of 7x7 pixels
 */
void
  ixsetms( type, n, xy )
int type;
int n;
XPoint *xy;
{
  int i;

  marker.type = type;
  marker.n = n < MAXMK ? n : MAXMK;
  if( marker.type >= 2 )
    for( i = 0; i < marker.n; i++ )
      marker.xy[i] = xy[i];
}


/*   SUBROUTINE IXSETMTS( TYPE, SIZE )    
 *   Author: Mario Stipcevic           
 * A set of predefined markers ala PAW */
/* type  0 or 20 = filled circle
 *       1 or 21 = filled box
 *       2 or 22 = filled triangle pointing up
 *       3 or 23 = filled triangle pointing down
 *       4 or 24 = hollow circle
 *       5 or 25 = hollow box
 *       6 or 26 = hollow triangle pointing up
 *       7 or 27 = hollow triangle pointing down
*/
void
  ixsetmts( type, size )
int type;
int size;
{
 int i, fill;
 float siz;
 short int xy[10][2];
 
 fill = 3;
 siz = (1 + size) / 2.;
 if (type >= 20) type = type - 20;
 if (type >= 4) fill = 2;
 if (type == 0) { ixsetms( 1, size, xy ); return; }
 if (type == 4) { ixsetms( 0, size, xy ); return; }
 if (type == 1 || type == 5) { 
  for(i = 0; i < 5; xy[i][0]=siz*boxm[i][0], xy[i][1]=siz*boxm[i][1], i++);
  ixsetms( fill, 5, xy ); return; }
 if (type == 2 || type == 6) { 
  for(i = 0; i < 4; xy[i][0]=siz*tupm[i][0], xy[i][1]=siz*tupm[i][1], i++);
  ixsetms( fill, 4, xy ); return; }
 if (type == 3 || type == 7) { 
  for(i = 0; i < 4; xy[i][0]=siz*tdnm[i][0], xy[i][1]=siz*tdnm[i][1], i++);
  ixsetms( fill, 4, xy ); return; }

}


/*
 *    SUBROUTINE IXSETMC(CINDEX)
 *    INTEGER CINDEX : color index defined my IXSETCOL
 *
 *    Set color index for markers
 */
void
  ixsetmc( cindex )
int cindex;
{
  setcolor( *gcmark, cindex );
}


/*
 *    SUBROUTINE IXMARKE(N,XY)
 *    INTEGER N         : number of points
 *    INTEGER*2 XY(2,N) : list of points
 *
 *    Draw a marker at each point
 */
void
   ixmarke( n, Rxy )
int n;
XPoint *Rxy;
{
  XPoint *xy;
  int i;
  Drawable d;

  if(!(d=cws->buffer)) d = cws->drawing; /* Handle buffer ixcrbuff() */

  if (rotate_fl || scale_fl || trans_fl) {
   xy = (XPoint *)calloc(n, sizeof(XPoint));
   for (i = 0; i < n; i++) {
    xy[i].x=xOrig+Rco*scalex*(Rxy[i].x-xRO)+Rsi*scaley*(Rxy[i].y-yRO)+.5;
    xy[i].y=yOrig-Rsi*scalex*(Rxy[i].x-xRO)+Rco*scaley*(Rxy[i].y-yRO)+.5;
   }
  } else {
   xy = Rxy;
  }

  do_raise = 1;
  if( marker.n <= 0 )
    XDrawPoints( display, d, *gcmark, xy, n, CoordModeOrigin );
  else {
    int r = marker.n / 2;
    int m;

    for( m = 0; m < n; m++ ) {
      int hollow = 0;

      switch( marker.type ) {
      int i;

      case 0:        /* hollow circle */
      XDrawArc( display, d, *gcmark,
          xy[m].x - r, xy[m].y - r, marker.n, marker.n, 0, 360 * 64 );
      break;

      case 1:        /* filled circle */
      XFillArc( display, d, *gcmark,
          xy[m].x - r, xy[m].y - r, marker.n, marker.n, 0, 360 * 64 );
    /* For Xlibs with ugly XFillArc: */
      XDrawArc( display, d, *gcmark,
          xy[m].x - r, xy[m].y - r, marker.n, marker.n, 0, 360 * 64 );
      break;

      case 2:        /* hollow polygon */
      hollow = 1;
      case 3:        /* filled polygon */
      for( i = 0; i < marker.n; i++ ) {
        marker.xy[i].x += xy[m].x;
        marker.xy[i].y += xy[m].y;
      }
      if( hollow )
        XDrawLines( display, d, *gcmark,
              marker.xy, marker.n, CoordModeOrigin );
      else
        XFillPolygon( display, d, *gcmark,
                marker.xy, marker.n, Nonconvex, CoordModeOrigin );
      for( i = 0; i < marker.n; i++ ) {
        marker.xy[i].x -= xy[m].x;
        marker.xy[i].y -= xy[m].y;
      }
      break;

      case 4:        /* segmented line */
      for( i = 0; i < marker.n; i += 2 )
        XDrawLine( display, d, *gcmark,
             xy[m].x + marker.xy[i].x, xy[m].y + marker.xy[i].y,
             xy[m].x + marker.xy[i+1].x, xy[m].y + marker.xy[i+1].y );
      break;
      }
    }
  }
}

/*
 *    SUBROUTINE IXCIRC(x0, y0, r, angle1, angle2, aspect)
 *    Author: Mario Stipcevic
 *    Simple interface to ixarc for QB's CIRCLE
 */
int
   ixcirc( x0, y0, r, phi1, phi2, aspect )
int x0, y0, r;
float phi1, phi2, aspect; /* start and stop angles in radians */
{
  static float f = 57.295779512;
  ixarc(x0, y0, r, (int)(aspect*r+.5), (int)(f*phi1+.5), (int)(f*phi2+.5), 0);
}

/*
 *    SUBROUTINE IXARC(x0, y0, rh, rv, angle1, angle2, ifill)
 *    Author: Mario Stipcevic
 *
 *    Draw an arc 
 *    center: (x0, y0) 
 *    horiz. and vert. radii: rh, rv
 *    start and stop angles in degrees: angle1, angle2
 *    ifill= 0 (hollow), 1 (filled)
 */
int
   ixarc( Rx0, Ry0, rx, ry, ang1, ang2, ifill )
int Rx0, Ry0, rx, ry, ang1, ang2, ifill;
{
  int x, y;
  int ang;
  static float f = 57.295779512;
  Drawable d;

  if(!(d=cws->buffer)) d = cws->drawing; /* Handle buffer ixcrbuff() */

  if (rotate_fl || scale_fl || trans_fl) {
   x = xOrig + Rco * scalex * (Rx0 - xRO) + Rsi * scaley * (Ry0 - yRO)+.5;
   y = yOrig - Rsi * scalex * (Rx0 - xRO) + Rco * scaley * (Ry0 - yRO)+.5;
   rx = fabs(scalex) * rx;
   ry = fabs(scaley) * ry;
   if (scalex < 0 && scaley < 0) {
    ang1 = 180 + ang1; ang2 = 180 + ang2;
   } else {
    if (scalex < 0.) {ang = ang2; ang2 = 180 - ang1; ang1 = 180 - ang;} 
    if (scaley < 0.) {ang = ang2; ang2 = -ang1; ang1 = -ang;}
   }
   ang1 = ang1 + r_angle * f;
   ang2 = ang2 + r_angle * f;
  } else {
   x = Rx0;
   y = Ry0;
  }

  do_raise = 1;
  while (ang2 < ang1) ang2 += 360;
  while (ang1 < -360) { ang1 = 360 + ang1; ang2 = 360 + ang2; }
  while (ang2 > 360) { ang1 = -360 + ang1; ang2 = -360 + ang2; }
/* printf("ixarc2: ang1=%d, ang2=%d\n", ang1, ang2); */
  ang2 = ang2 - ang1; /* Length of the arc, as required by XDrawArc */

  switch( ifill ) {
  case 0:        /* hollow arc */
       XDrawArc( display, d, *gcline,
                  x - rx, y - ry, 2 * rx, 2 * ry, ang1 * 64, ang2 * 64 );
  break;

  case 1:        /* filled arc */
       XFillArc( display, d, *gcfill,
                  x - rx, y - ry, 2 * rx, 2 * ry, ang1 * 64, ang2 * 64 );
       XDrawArc( display, d, *gcfill,
                  x - rx, y - ry, 2 * rx, 2 * ry, ang1 * 64, ang2 * 64 );
  break;
  }
}

/*
 *    SUBROUTINE IXSETFS(STYLE,FASI)
 *    INTEGER STYLE : fill area interior style hollow or solid
 *
 *    Set fill area style
 */
void
  ixsetfs( style, fasi )
int style;
int fasi;
{
  fill_border = 0;
  if ( fasi == 0 ) style = 0;
  switch( style ) {

  case 1:         /* fill interior with solid color */
    fill_hollow = 0;
    XSetFillStyle( display, *gcfill, FillSolid );
    break;

  case 2:         /* fill interior + border with solid color */
    fill_hollow = 0;
    fill_border = 1;
    XSetFillStyle( display, *gcfill, FillSolid );
    break;

  case 3:         /* hatch */
    fill_hollow = 0;
    XSetFillStyle( display, *gcfill, FillStippled );
    if( fasi != current_fasi ) {
      if( fill_pattern != (Pixmap)NULL ) {
        XFreePixmap( display, fill_pattern );
        fill_pattern = (Pixmap)NULL;
      }
      switch( fasi ) {
        case 1: fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p3_bits, 16, 16);
                break;
        case 2: fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p2_bits, 16, 16);
                break;
        case 3: fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p1_bits, 16, 16);
                break;
        case 4: fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p4_bits, 16, 16);
                break;
        case 5: fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p5_bits, 16, 16);
                break;
        case 6: fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p6_bits, 16, 16);
                break;
        case 7: fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p7_bits, 16, 16);
                break;
        case 8: fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p8_bits, 16, 16);
                break;
        case 9: fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p9_bits, 16, 16);
                break;
        case 10:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p10_bits, 16, 16);
                break;
        case 11:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p11_bits, 16, 16);
                break;
        case 12:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p12_bits, 16, 16);
                break;
        case 13:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p13_bits, 16, 16);
                break;
        case 14:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p14_bits, 16, 16);
                break;
        case 15:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p15_bits, 16, 16);
                break;
        case 16:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p16_bits, 16, 16);
                break;
        case 17:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p17_bits, 16, 16);
                break;
        case 18:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p18_bits, 16, 16);
                break;
        case 19:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p19_bits, 16, 16);
                break;
        case 20:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p20_bits, 16, 16);
                break;
        case 21:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p21_bits, 16, 16);
                break;
        case 22:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p22_bits, 16, 16);
                break;
        case 23:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p23_bits, 16, 16);
                break;
        case 24:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p24_bits, 16, 16);
                break;
        case 25:fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p25_bits, 16, 16);
                break;
       default: fill_pattern = XCreateBitmapFromData(display,
                RootWindow( display, screen_number), p2_bits, 16, 16);
                break;
      }
      XSetStipple( display, *gcfill, fill_pattern );
      current_fasi = fasi;
    }
    break;

  default:
    fill_hollow = 1;
  }
}


/*
 *    SUBROUTINE IXSETFC(CINDEX)
 *    INTEGER CINDEX : color index defined my IXSETCOL
 *
 *    Set color index for fill areas
 */
void
  ixsetfc( cindex )
int cindex;
{
  setcolor( *gcfill, cindex );

  /* invalidate fill pattern */
  if( fill_pattern != (Pixmap)NULL ) {
    XFreePixmap( display, fill_pattern );
    fill_pattern = (Pixmap)NULL;
  }
}


/*
 *    SUBROUTINE IXFLARE(N,XY)
 *    INTEGER N         : number of points
 *    INTEGER*2 XY(2,N) : list of points
 *
 *    Fill area described by polygon
 */
void
  ixflare( n, Rxy )
int n;
XPoint *Rxy;
{
  XPoint *xy;
  int i;
  Drawable d;

  if(!(d=cws->buffer)) d = cws->drawing; /* Handle buffer ixcrbuff() */

  if (rotate_fl || scale_fl || trans_fl) {
   xy = (XPoint *)calloc(n, sizeof(XPoint));
   for (i = 0; i < n; i++) {
    xy[i].x=xOrig+Rco*scalex*(Rxy[i].x-xRO)+Rsi*scaley*(Rxy[i].y-yRO)+.5;
    xy[i].y=yOrig-Rsi*scalex*(Rxy[i].x-xRO)+Rco*scaley*(Rxy[i].y-yRO)+.5;
   }
  } else {
   xy = Rxy;
  }

  if( fill_hollow )
    XDrawLines( display, d, *gcfill, xy, n, CoordModeOrigin );
  else {
   if( fill_border )
    XDrawLines( display, d, *gcfill, xy, n, CoordModeOrigin );
    XFillPolygon( display, d, *gcfill,
                  xy, n, Nonconvex, CoordModeOrigin );
  }
  do_raise = 1;
}


/*
 *    SUBROUTINE IXSETTA(TXALH,TXALV)
 *    INTEGER TXALH : horizontal text alignment
 *    INTEGER TXALV : vertical text alignment
 */
void
  ixsetta( txalh, txalv )
int txalh;
int txalv;
{
  switch ( txalh ) {

  case 0 :
  case 1 : switch ( txalv) {
           case 0 : 
           case 1 : text_align = 7;
                    break;
           case 2 : text_align = 4;
                    break;
           case 3 : text_align = 1;
                    break;
           }
           break;
  case 2 : switch ( txalv) {
           case 0 : 
           case 1 : text_align = 8;
                    break;
           case 2 : text_align = 5;
                    break;
           case 3 : text_align = 2;
                    break;
           }
           break;
  case 3 : switch ( txalv) {
           case 0 : 
           case 1 : text_align = 9;
                    break;
           case 2 : text_align = 6;
                    break;
           case 3 : text_align = 3;
                    break;
           }
           break;
  }
}


/*
 *    FUNCTION IXSETTF(MODE,LENFNT,FONTNAME)
 *    INTEGER MODE       : loading flag
 *            MODE=0     : search if the font exist
 *            MODE=1     : search the font and load it if it exist
 *    INTEGER LENFNT     : font name length
 *    CHARACTER*(*) FONT : font name
 *
 *    Set text font to specified name. This function returns 0 if
 *    the specified font is found, 1 if not.
 */
int
  ixsettf( mode, fontname )
int mode;
char fontname[128];
{
  char **fontlist;
  int fontcount;
  int i;

  if ( mode != 0 ) {
     for ( i = 0; i < MAXFONT; i++ ) {
        if (strcmp(fontname, font[i].name) == 0) {
           text_font = font[i].id;
           XSetFont( display, *gctext, text_font->fid);
           XSetFont( display, *gcinvt, text_font->fid);
           return(0);
        }
     }
  }

  fontlist = XListFonts( display, fontname, 1, &fontcount);

  if ( fontcount != 0 ) {
     if ( mode != 0 ) {
        if ( font[current_font_number].id )
           XFreeFont ( display, font[current_font_number].id);
        font[current_font_number].id = XLoadQueryFont( display, fontlist[0]);
	text_font = font[current_font_number].id;
        XSetFont( display, *gctext, text_font->fid);
        XSetFont( display, *gcinvt, text_font->fid);
        strcpy (font[current_font_number].name,fontname);
        current_font_number++;
        if ( current_font_number == MAXFONT ) current_font_number = 0;
     }
     XFreeFontNames(fontlist);
     return(0);
  }
  else {
     return(1);
  }
}


/*
 *    SUBROUTINE IXSETTC(CINDEX)
 *    INTEGER CINDEX : color index defined by IXSETCOL
 *
 *    Set color index for text
 */
void
  ixsettc( cindex )
int cindex;
{
  setcolor( *gctext, cindex );

  if ( XGetGCValues( display, *gctext, gc_mask, &gc_return) ) {
     XSetForeground( display, *gcinvt, gc_return.background );
     XSetBackground( display, *gcinvt, gc_return.foreground );
  } else {
     printf("**** Error: Cannot get GC values \n");
  }
  XSetBackground( display, *gctext, colors[0].pixel );
}


/*
 *    SUBROUTINE IXTEXT(MODE,X,Y,ANGLE,MGN,LENTXT,TEXT)
 *    INTEGER MODE       : drawing mode
 *            MODE=0     : the background is not drawn
 *            MODE=1     : the background is drawn
 *    INTEGER X,Y        : text position
 *    REAL    ANGLE      : text angle in degrees
 *    REAL    MGN        : magnification factor
 *    INTEGER LENTXT     : text length
 *    CHARACTER*(*) TEXT : text string
 *
 *    Draw a text string using current font
 */
void
  ixtext( mode, Rx, Ry, Rangle, mgn, text )
char text[256];
int mode;
int Rx, Ry;
float Rangle;
float mgn;
{
  int x, y;
  float angle;
  Drawable d;

  if(!(d=cws->buffer)) d = cws->drawing; /* Handle buffer ixcrbuff() */

  if (rotate_fl || scale_fl || trans_fl) {
   x = xOrig + Rco * scalex * (Rx - xRO) + Rsi * scaley * (Ry - yRO)+.5;
   y = yOrig - Rsi * scalex * (Rx - xRO) + Rco * scaley * (Ry - yRO)+.5;
   angle = Rangle + r_angle * 57.295779512;
  } else {
   x = Rx; y = Ry; angle = Rangle;
  }

  if (mgn != 1.) XRotSetMagnification(mgn);

  switch ( mode ) {

  case 0 : XRotDrawAlignedString( display, text_font, angle,
           d, *gctext, x, y, text, text_align);
           break;

  case 1 : XRotDrawAlignedImageString( display, text_font, angle,
           d, *gctext, x, y, text, text_align);
           break;

  default: break;
  }
  do_raise = 1;
}


/*
 *    SUBROUTINE IXTXTL(IW,IH,LMESS,MESS)
 *    INTEGER IW          : text width
 *    INTEGER IH          : text height
 *    INTEGER LMES        : message length
 *    CHARACTER*(*) MESS  : message
 *
 *    Return the size of a character string
 */
void
  ixtxtl( w, h, mess )
char mess[256];
int *w;
int *h;
{
   *w = XTextWidth( text_font, mess, strlen(mess) );
   *h = text_font->ascent;
}


/*
 *    SUBROUTINE IXBOX(X1,X2,Y1,Y2,MODE)
 *    INTEGER X1,Y1 : left down corner
 *    INTEGER X2,Y2 : right up corner
 *    INTEGER MODE : drawing mode
 *
 *            MODE=0 hollow
 *            MODE=1 solid
 *
 *    Draw a box
 */
void
  ixbox( x1, x2, y1, y2, mode)
int x1, x2, y1, y2;
int mode;
{
 short xy[5][2];
 Drawable d;

 if(!(d=cws->buffer)) d = cws->drawing; /* Handle buffer ixcrbuff() */

 if (rotate_fl || scale_fl || trans_fl || mode == 0) {

     xy[0][0]=x1; xy[0][1]=y1;
     xy[1][0]=x2; xy[1][1]=y1;
     xy[2][0]=x2; xy[2][1]=y2;
     xy[3][0]=x1; xy[3][1]=y2;
     xy[4][0]=x1; xy[4][1]=y1;

     switch ( mode ) {

     case 0 : /* Draw holow rectangle using lines (M.S.) */ 
                         ixline(5, xy);
              break;

     case 1 : /* Draw filled rectangle using flare (M.S.) */ 
                         ixflare(5, xy);
              break;
     }
 } else {
     switch ( mode ) {

/*   case 0 : XDrawRectangle( display, d, *gcline,
                              min(x1,x2), min(y1,y2),
                              abs(x2-x1), abs(y2-y1));
              break;     Doesn't work for dashes.
*/
  
     case 1 : XFillRectangle( display, d, *gcfill,
                              min(x1,x2), min(y1,y2),
                              abs(x2-x1), abs(y2-y1));
              break;
     }
 }
 do_raise = 1;
}


/*
 *    SUBROUTINE IXCA(X1,X2,Y1,Y2,NX,NY,IC)
 *    INTEGER X1,Y1 : left down corner
 *    INTEGER X2,Y2 : right up corner
 *    INTEGER NX,NY : array size
 *    INTEGER IC : array
 *
 *    Draw a cell array. The drawing is done with the pixel presicion
 *    if (X2-X1)/NX (or Y) is not a exact pixel number the position of
 *    the top rigth corner may be wrong.
 */
void
  ixca(x1, x2, y1, y2, nx, ny, ic)
int *x1, *x2, *y1, *y2, *nx, *ny;
int ic[];
{
   int i,j,icol,ix,iy,w,h,current_icol;

   current_icol = -1;
   w            = max((*x2-*x1)/(*nx),1);
   h            = max((*y1-*y2)/(*ny),1);
   ix           = *x1;

   for ( i=0; i<*nx; i++ ) {
      iy = *y1-h;
      for ( j=0; j<*ny; j++ ) {
         icol = ic[i+(*nx*j)];
         if(icol != current_icol){
            XSetForeground( display, *gcfill, colors[icol].pixel );
            current_icol = icol;
         }
         XFillRectangle( display, cws->drawing, *gcfill, ix, iy, w, h);
         iy = iy-h;
      }
      ix = ix+w;
   }
}


/*
 *    SUBROUTINE IXDRMDE(MODE)
 *    INTEGER MODE : drawing mode
 *    Changed (enhanced) by Mario 
 *            MODE=1 copy src
 *            MODE=2 src xor dest
 *            MODE=3 invert (not dest)
 *            MODE=4 src and dest
 *            MODE=5 not (src xor dest)
 *            MODE=16 set the suitable mode for cursor echo according to
 *                   the vendor.
 *
 *    Set the drawing mode
 */
void
  ixdrmde( mode )
  int mode;
{
  int i;
  draw_mode = mode;
  switch ( mode ) {

     case 1 :
     for ( i = 0; i < MAXGC; i++ ) XSetFunction( display, gclist[i], GXcopy);
     break;

     case 2 :
     for ( i = 0; i < MAXGC; i++ ) XSetFunction( display, gclist[i], GXxor);
     break;

     case 3 :
     for ( i = 0; i < MAXGC; i++ ) XSetFunction( display, gclist[i], GXinvert);
     break;

     case 4 :
     for ( i = 0; i < MAXGC; i++ ) XSetFunction( display, gclist[i], GXand);
     break;

     case 5 :
     for ( i = 0; i < MAXGC; i++ ) XSetFunction( display, gclist[i], GXequiv);
     break;

     case 16:
     if(strstr(vendor,"Hewlett")) {
      for ( i = 0; i < MAXGC; i++ ) XSetFunction( display, gclist[i], GXinvert);
     } else {
      for ( i = 0; i < MAXGC; i++ ) XSetFunction( display, gclist[i], GXxor);
     }
     break;

     default:
     mode = -mode;
     if ( mode >= 0 && mode <= 15 ) {
      for ( i = 0; i < MAXGC; i++ ) XSetFunction( display, gclist[i], mode);
     }
     break;
  }
}


/*
 *    SUBROUTINE IXSYNC(MODE)
 *    INTEGER MODE : synchronisation on/off
 *
 *            MODE=1 on
 *            MODE<>0 off
 *
 *    Set synchronisation on or off
 */
void
  ixsync( mode )
  int mode;
{
  switch ( mode ) {

     case 1 :
     XSynchronize(display,1);
     break;

     default:
     XSynchronize(display,0);
     break;
  }
}


/*
 *    SUBROUTINE IXQPTR(IX,IY)
 *
 *    Output parameters:
 *
 *    INTEGER IX : X coordinate of pointer
 *    INTEGER IY : Y coordinate of pointer
 *    (both coordinates are relative to the origin of the root window)
 */
void ixqptr( ix, iy )
int *ix,*iy;
{
   Window idg;

   Window    root_return,child_return;
   int       win_x_return,win_y_return;
   int       root_x_return,root_y_return;
   unsigned int mask_return;

   XQueryPointer(display,cws->window,&root_return,
   &child_return,&root_x_return,&root_y_return,&win_x_return,
   &win_y_return,&mask_return);

   *ix = (int)root_x_return;
   *iy = (int)root_y_return;
}



/*
 *    SUBROUTINE IXWARP(IX,IY)
 *
 *    Input parameters:
 *
 *    INTEGER IX : New X coordinate of pointer
 *    INTEGER IY : New Y coordinate of pointer
 *    (both coordinates are relative to the origin of the current window)
 */
void ixwarp( ix, iy )
int ix,iy;
{
   XWarpPointer(display,0,cws->window,0,0,0,0,ix,iy);
}


/*
 *    INTEGER FUNCTION IXREQLO(MODE,CTYP,X,Y)
 *    INTEGER X,Y : cursor position out moment of button press (output)
 *    INTEGER CTYP : cursor type (input)
 *
 *            CTYP=1 tracking cross
 *            CTYP=2 cross-hair
 *            CTYP=3 rubber circle
 *            CTYP=4 rubber band
 *            CTYP=5 rubber rectangle
 *
 *    INTEGER MODE : input mode
 *
 *            MODE=0 request
 *            MODE=1 sample
 *
 *    Request locator:
 *    return button number  1 = left is pressed
 *                          2 = middle is pressed
 *                          3 = right is pressed
 *         in sample mode:
 *                         11 = left is released
 *                         12 = middle is released
 *                         13 = right is released
 *                         -1 = nothing is pressed or released
 *                         -2 = leave the window
 *                       else = keycode (keyboard is pressed)
 */
int
  ixreqlo( mode, ctyp, x, y )
int mode, ctyp, *x, *y;
{
  static Cursor cursor = (Cursor)LNULL;
  XEvent event;
  int button_press;
  int radius;

  setinput(1);

/*
 *   Change the cursor shape
 */
  if( cursor == (Cursor)NULL && ctyp != 0) {
     if( ctyp > 1 ) {
      XDefineCursor( display, cws->window, null_cursor );
      XSetForeground( display, gcecho, colors[0].pixel );
     } else {
        cursor = XCreateFontCursor( display, XC_crosshair );
        XDefineCursor( display, cws->window, cursor );
     }
  }

/*
 *   Event loop
 */
  button_press = 0;

  while ( button_press == 0 ) {

     switch ( ctyp ) {

     case 1 : break;

     case 2 : XDrawLine( display, cws->window, gcecho,
                         xloc, 0, xloc, cws->height);
              XDrawLine( display, cws->window, gcecho,
                         0, yloc, cws->width, yloc);
              break;

     case 3 : radius = (int) sqrt((double)((xloc-xlocp)*(xloc-xlocp)+
                                           (yloc-ylocp)*(yloc-ylocp)));
              XDrawArc( display, cws->window, gcecho,
                        xlocp-radius, ylocp-radius,
                        2*radius, 2*radius, 0, 23040);

     case 4 : XDrawLine( display, cws->window, gcecho,
                         xlocp, ylocp, xloc, yloc);
              break;

     case 5 : XDrawRectangle( display, cws->window, gcecho,
                              min(xlocp,xloc), min(ylocp,yloc),
                              abs(xloc-xlocp), abs(yloc-ylocp));
              break;

     default: break;
     }

     while ( XEventsQueued( display, QueuedAlready) > 1) {
        XNextEvent( display, &event);
     }
     XWindowEvent( display, cws->window, mouse_mask, &event );

     switch ( ctyp ) {

     case 1 : break;

     case 2 : XDrawLine( display, cws->window, gcecho,
                         xloc, 0, xloc, cws->height);
              XDrawLine( display, cws->window, gcecho,
                         0, yloc, cws->width, yloc);
              break;

     case 3 : radius = (int) sqrt((double)((xloc-xlocp)*(xloc-xlocp)+
                                           (yloc-ylocp)*(yloc-ylocp)));
              XDrawArc( display, cws->window, gcecho,
                        xlocp-radius, ylocp-radius,
                        2*radius, 2*radius, 0, 23040);

     case 4 : XDrawLine( display, cws->window, gcecho,
                         xlocp, ylocp, xloc, yloc);
              break;

     case 5 : XDrawRectangle( display, cws->window, gcecho,
                              min(xlocp,xloc), min(ylocp,yloc),
                              abs(xloc-xlocp), abs(yloc-ylocp));
              break;

     default: break;
     }

     xloc = event.xbutton.x;
     yloc = event.xbutton.y;

     switch ( event.type ) {

     case LeaveNotify :
           if( mode == 0 ) {
              forever {
                 XNextEvent( display, &event);
                 if( event.type == EnterNotify ) break;
              }
           } else {
              button_press = -2;
           }
           break;

     case ButtonPress :
          button_press =  event.xbutton.button ;
          xlocp = event.xbutton.x;
          ylocp = event.xbutton.y;
          if(ctyp != 0) {
           XUndefineCursor( display, cws->window );
           cursor = (Cursor)NULL;
          }
          break;

     case ButtonRelease :
           if( mode == 1 ) {
              button_press =  10+event.xbutton.button ;
              xlocp = event.xbutton.x;
              ylocp = event.xbutton.y;
           }
          break;

     case KeyPress :
           if( mode == 1 ) {
              button_press =  event.xkey.keycode;
              xlocp = event.xbutton.x;
              ylocp = event.xbutton.y;
           }
          break;

     case KeyRelease :
           if( mode == 1 ) {
              button_press =  -event.xkey.keycode;
              xlocp = event.xbutton.x;
              ylocp = event.xbutton.y;
           }
          break;

     default : break;
     }

     if( mode == 1 ) {
         if( button_press == 0 )
            button_press = -1;
         break;
     }
  }
  *x = event.xbutton.x;
  *y = event.xbutton.y;
  setinput(0);
  XFlush( display );
  return( button_press );
}


/*
 *    INTEGER FUNCTION IXREQST(X,Y,TEXT)
 *    INTEGER X,Y        : position where text is displayed
 *    CHARACTER*(*) TEXT : text displayed (input), edited text (output)
 *
 *    Request string:
 *    text is displayed and can be edited with Emacs-like keybinding
 *    return termination code (0 for ESC, 1 for RETURN)
 *    Sep 1997, Apr 1998: bugs fixed by Mario Stipcevic
 */
int
  ixreqst( x, y, text )
char text[256];
int x, y;
{
  static Cursor cursor = (Cursor)LNULL;
  static int percent = 0;  /* bell volume */
  Window focuswindow;
  int focusrevert;
  XEvent event;
  KeySym keysym;
  int key = -1;
  int nt;             /* defined length of text */
  int pt = 0;         /* cursor position in text */
  int len_text = 254; /* input text length */

  setinput(1);
  nt = strlen(text);

  /* change the cursor shape */
  if( cursor == (Cursor)NULL ) {
    XKeyboardState kbstate;
    cursor = XCreateFontCursor( display, XC_question_arrow );
    XGetKeyboardControl( display, &kbstate );
    percent = kbstate.bell_percent;
  }
  if( cursor != (Cursor)NULL )
    XDefineCursor( display, cws->window, cursor );
/*
  for( nt = len_text; nt > 0 && text[nt-1] == ' '; nt-- );
*/
  pt = nt;
  XGetInputFocus( display, &focuswindow, &focusrevert );
  XSetInputFocus( display, PointerRoot, focusrevert, CurrentTime );
/*
  XSetInputFocus( display, cws->window, focusrevert, CurrentTime );
  This causes server to crash if executed before window appears (slow network)
*/
  while( key < 0 ) {
    char keybuf[8];
    char nbytes;
    int dx;
    int i;
    XDrawImageString( display, cws->window, *gctext,
              x, y, text, nt );
    dx = XTextWidth( text_font, text, nt );
    XDrawImageString( display, cws->window, *gctext,
              x + dx, y, " ", 1 );
    dx = pt == 0 ? 0 : XTextWidth( text_font, text, pt );
    XDrawImageString( display, cws->window, *gcinvt,
              x + dx, y, pt < len_text ? &text[pt] : " ", 1 );
    XWindowEvent( display, cws->window, keybd_mask, &event );
    switch( event.type ) {
    case ButtonPress:
    case EnterNotify:
      XSetInputFocus( display, cws->window, focusrevert, CurrentTime );
      break;
    case LeaveNotify:
      XSetInputFocus( display, focuswindow, focusrevert, CurrentTime );
      break;
    case KeyPress:
      nbytes = XLookupString( &event.xkey, keybuf, sizeof( keybuf ),
                 &keysym, NULL );
      switch( keysym ) {      /* map cursor keys */
      case XK_Left:
      keybuf[0] = '\002';  /* Control-B */
      nbytes = 1;
      break;
      case XK_Right:
      keybuf[0] = '\006';  /* Control-F */
      nbytes = 1;
      break;
      }
      if( nbytes == 1 ) {
      if( isascii( keybuf[0] ) && isprint( keybuf[0] ) ) {
        /* insert character */
        if( nt < len_text )
          nt++;
        for( i = nt - 0; i > pt; i-- )
          text[i] = text[i-1];
        if( pt < len_text ) {
          text[pt] = keybuf[0];
          pt++;
        }
      }
      else
        switch( keybuf[0] ) {
          /* Emacs-like editing keys */

        case '\010':    /* backspace */
          /* delete backward */
          if( pt > 0 ) {
            dx = XTextWidth( text_font, text, nt );
            XDrawImageString( display, cws->window, *gctext, x+dx, y, " ", 1 );
            for( i = pt; i <= nt; i++ )
             text[i-1] = text[i];
            nt--;
            pt--;
          }
          break;
        case '\001':    /* ^A */
          /* beginning of line */
          pt = 0;
          break;
        case '\002':    /* ^B */
          /* move backward */
          if( pt > 0 )
            pt--;
          break;
        case '\004':    /* ^D */
        case '\177':    /* delete */
          /* delete forward */
          if( pt < nt ) {
            for( i = pt; i < nt; i++ )
            text[i] = text[i+1];
            text[nt-1] = ' ';
            nt--;
          }
          break;
        case '\005':    /* ^E */
          /* end of line */
          pt = nt;
          break;

        case '\006':    /* ^F */
          /* move forward */
          if( pt < nt )
            pt++;
          break;
        case '\013':    /* ^K */
          /* delete to end of line */
          for( i = pt; i < nt; i++ )
            text[i] = ' ';
          XDrawImageString( display, cws->window, *gctext, x, y, text, nt);
          nt = pt;
          break;
        case '\024':    /* ^T */
          /* transpose */
          if( pt > 0 ) {
            char c = text[pt];
            text[pt] = text[pt-1];
            text[pt-1] = c;
          }
          break;
        case '\012':    /* newline */
        case '\015':    /* return */
          key = 1;
          break;
        case '\033':    /* escape */
          key = 0;
          break;

        default:
          XBell( display, percent );
        }
      }
    }
  }
  XSetInputFocus( display, focuswindow, focusrevert, CurrentTime );

  if( cursor != (Cursor)NULL ) {
    XUndefineCursor( display, cws->window );
    cursor = (Cursor)LNULL;
  }

  setinput(0);

  /* Make sure there are no trailing spaces */
  nt = strlen(text);
  while(text[nt-1] == ' ') nt--;
  text[nt] = '\0';

  XSync( display, 1 );
  return( key );
}

#define MAX_SEGMENT 20
void ixput_image(offset,itran,x0,y0,nx,ny,xmin,ymin,xmax,ymax,image,ipal)
  int offset,itran,x0,y0,nx,ny,xmin,ymin,xmax,ymax,ipal;
  unsigned char *image;
{
  int           i, n, x, y, xcur, x1, x2, y1, y2;
  unsigned char *jimg, *jbase, icol;
  int           nlines[256];
  XSegment      lines[256][MAX_SEGMENT];
  Drawable d;

  if(!(d=cws->buffer)) d = cws->drawing; /* Handle buffer ixcrbuff() */

  for (i=0; i<256; i++) nlines[i] = 0;

  x1 = x0 + xmin; y1 = y0 + ny - ymax - 1;
  x2 = x0 + xmax; y2 = y0 + ny - ymin - 1;
  jbase = image + (ymin-1)*nx + xmin;

  for (y=y2; y>=y1; y--) {
    xcur   = x1; jbase += nx;
    for (jimg = jbase, icol = *jimg++, x=x1+1; x<=x2; jimg++, x++) {
      if (icol != *jimg) {
        if (icol != itran) {
          n = nlines[icol]++;
          lines[icol][n].x1 = xcur; lines[icol][n].y1 = y;
          lines[icol][n].x2 = x-1;  lines[icol][n].y2 = y;
          if (nlines[icol] == MAX_SEGMENT) {
            if(!ipal) { setcolor(*gcline,(int) (icol+offset)); } 
            else
                      { setcolor(*gcline,(int) tpal[icol]);    }
            XDrawSegments(display,d,*gcline,&lines[icol][0],
              MAX_SEGMENT);
            nlines[icol] = 0;
          }
        }
        icol = *jimg; xcur = x;
      }
    }
    if (icol != itran) {
      n = nlines[icol]++;
      lines[icol][n].x1 = xcur; lines[icol][n].y1 = y;
      lines[icol][n].x2 = x-1;  lines[icol][n].y2 = y;
      if (nlines[icol] == MAX_SEGMENT) {
        if(!ipal) { setcolor(*gcline,(int) (icol+offset)); }
        else
                  { setcolor(*gcline,(int) tpal[icol]);    }
        XDrawSegments(display,d,*gcline,&lines[icol][0],
          MAX_SEGMENT);
        nlines[icol] = 0;
      }
    }
  }

  for (i=0; i<256; i++) {
    if (nlines[i] != 0) {
    if(!ipal) { setcolor(*gcline,(int) (i+offset)); }
    else
              { setcolor(*gcline,(int) tpal[i]);    }
     XDrawSegments(display,d,*gcline,&lines[i][0],nlines[i]);
    }
  }
}

/*
 *    SUBROUTINE STDPAL()
 *    Created by: Mario Stipcevic 07.01.1997
 *    Install Standard 6x6x6 Palette
 */
int stdpal()
{
 int j, pofset, r, g, b;

 pofset = 32;
 j = pofset;
 for(r=0; r<=5; r++) {
  for(g=0; g<=5; g++) {
   for(b=0; b<=5; b++) {
    ixsetco(j,r*51,g*51,b*51);
    j++;
   }}}
 return 0;
}

/*
 *    SUBROUTINE GIFINFO(FILE, GInf)
 *    Created by: Mario Stipcevic 07.01.1997
 C    CHAR    FILE: filename
 *    INTEGER GInf: output information: width, height, ncolors 
 *
 */
void gifinfo (file, GInf__)
char file[128];
short *GInf__;
{
  FILE  *fd;
  long  filesize;
  unsigned char *GIFarr;
  int   width, height, ncolor;

  GInf__[0] = 0; GInf__[1] = 0; GInf__[2] = 0;
  fd = fopen(file, "r");
  if (!fd) {
    fprintf(stderr,"gifinfo: Unable to open GIF file '%s'\n", file);
    return;
  }

  fseek(fd, 0L, 2);
  filesize = ftell(fd);
  fseek(fd, 0L, 0);

  if (!(GIFarr = (unsigned char *) calloc(filesize+256,1))) {
    fprintf(stderr,"gifinfo: unable to allocate array for gif\n");
    return;
  }

  if (fread(GIFarr, filesize, 1, fd) != 1) {
    fprintf(stderr,"gifinfo: GIF file read failed\n");
    return;
  }

  GIFinfo(GIFarr, &width, &height, &ncolor);
  *GInf__++ = width; *GInf__++ = height; *GInf__ = ncolor; /* Save GIF Info */

  fclose(fd);
  return;
}

/*
 *    SUBROUTINE IXLDGIF(X0, Y0, FILE, IPAL, BG)
 *    Modified by: Mario Stipcevic 07.01.1997
 *    CHARACTER*(*) FILE : file name
 *    INTEGER X0 Y0      : position
 *    INTEGER ipal: 0 = use Own palette, 1 = use Std 6x6x6 palette
 *    INTEGER bg   specifies background: bg<0 GIF's own, bg>=0 =colorindex
 *
 *    Load the gif a file in the current active window.
 */
void ixldgif (x0, y0, file, ipal, bg)
char file[128];
int x0,y0, ipal, bg;
{
  FILE  *fd;
  long  filesize;
  unsigned char *GIFarr, *PIXarr, R[256], G[256], B[256], *j1, *j2, icol;
  int   i, j, k, width, height, ncolor, irep, offset, pofset;
  int rr, gg, bb;

  fd = fopen(file, "r");
  if (!fd) {
    fprintf(stderr,"ixldgif: Unable to open GIF file '%s'\n", file);
    return;
  }

  /* Set Std. Color Palette if not already set and if needed */
  if (ipal && !stdset) { stdpal(); }
  stdset = ipal; 

  fseek(fd, 0L, 2);
  filesize = ftell(fd);
  fseek(fd, 0L, 0);

  if (!(GIFarr = (unsigned char *) calloc(filesize+256,1))) {
    fprintf(stderr,"ixldgif: unable to allocate array for gif\n");
    return;
  }

  if (fread(GIFarr, filesize, 1, fd) != 1) {
    fprintf(stderr,"ixldgif: GIF file read failed\n");
    fclose(fd);
    return;
  }
  fclose(fd);

  irep = GIFinfo(GIFarr, &width, &height, &ncolor);
  if (irep != 0) return;

  if (!(PIXarr = (unsigned char *) calloc((long)(width*height),1))) {
    fprintf(stderr,"ixldgif: unable to allocate array for image\n");
    return;
  }

  irep = GIFdecode(GIFarr, PIXarr, &width, &height, &ncolor, R, G, B);
  if (irep != 0) return;

/* Debug code: 
  for (j=0; j<10; j++) {
   for (i=0; i<10; i++) {
    printf("%5d", PIXarr[i+j*width]);
   }
   printf("\n");
  }
*/

  /*   S E T   P A L E T T E   */

  offset = 16;
  pofset = 32; /* std. palette offset */ 
  
  if (ipal == 0) 
  {
   /* use Own colormap */
   if (ncolor > 256 - offset) { 
   printf("ixldgif: number of colors in GIF file too large, truncating to %d\n",
      (ncolor=(256-offset)) ); } 
   for (i=0; i<ncolor; i++) {
     rr = R[i];
     gg = G[i];
     bb = B[i];
     j = i+offset;
     ixsetco(j,rr,gg,bb);
   }
   if (bg >= 0) ixsetco(offset,colors[bg].red,colors[bg].green,colors[bg].blue);
  } 
  else
  {
   /* use Std. 6x6x6 colormap */
   for (i=0; i<ncolor; i++) {
     rr = (R[i] + 26) / 51;
     gg = (G[i] + 26) / 51;
     bb = (B[i] + 26) / 51;
     j = pofset + bb + 6 * (gg + 6 * rr);
     tpal[i] = j;          
   }
  }

  /*  O U T P U T   I M A G E  */

  for (i=1; i<=height/2; i++) {
    j1 = PIXarr + (i-1)*width;
    j2 = PIXarr + (height-i)*width;
    for (k=0; k<width; k++) {
      icol = *j1; *j1++ = *j2; *j2++ = icol;
    }
  }
  ixput_image(offset,-1,x0,y0,width,height,0,0,width-1,height-1,PIXarr,ipal);
}

typedef unsigned char byte;     /* type used by subroutines to code&decode GIF*/
XImage *image;                  /* temporary unit for GIFencode()             */
FILE *out;                      /* output/input file for animation            */
static int first = 0;           /* MS: ask for image pointer only once (why??)*/

/*
 * subroutines for GIFencode()
 */

void
  get_scline( y, width, scline )
  int y, width;
  byte scline[];
{
int i;
for(i=0;i< width;i++)
        scline[i] = XGetPixel( image, i, y);
}


void
  put_byte( b)
  byte b;
{
 if( ferror(out) == 0) fputc( b, out);
}

/*
 *    SUBROUTINE IXDOGIF( LEN , FILE)
 *    CHARACTER  NAME
 *    INTEGER LEN
 *
 *    Writes the current window into a GIF file
 *
 */
void
  ixdogif( file )
char file[128];
{

byte  scline[5000], r[256], g[256], b[256];
int R[256], G[256], B[256];
int ncol, maxcol, i;
int ln;
char file_name[100];
unsigned long z;

ImgPickPalette( display ,cws->window, &ncol, R, G, B);

 maxcol = 0;
 for(i=0; i<ncol; i++)
 {
  if(maxcol < R[i] ) maxcol = R[i];
  if(maxcol < G[i] ) maxcol = G[i];
  if(maxcol < B[i] ) maxcol = B[i];
  r[i] = 0;
  g[i] = 0;
  b[i] = 0;
 }
 if( maxcol != 0)
  {
  for(i=0; i<ncol; i++)
  {
   r[i] = (R[i] * 255) / maxcol;
   g[i] = (G[i] * 255) / maxcol;
   b[i] = (B[i] * 255) / maxcol;
  }
 }

 if (first == 0 ) {
  image = XGetImage( display, cws->drawing, 0, 0,
                     cws->width, cws->height,
                     AllPlanes, ZPixmap);
/* first = 1;  Commented by Mario: strange effect (subimages ?) */
 } else {
  image = XGetSubImage(display, cws->drawing, 0, 0,
                       cws->width, cws->height,
                       AllPlanes, ZPixmap, image, 0, 0);
 }

 sprintf( file_name, "%s", file);

 out = fopen ( file_name, "w+");

 GIFencode( cws->width, cws->height,
            ncol, r, g, b, scline, get_scline, put_byte);

 fclose( out );
}
 
/*   answ = IXWPTRQ (x, y, rootx, rooty, win, mode, ctyp)
     Generalized ixreqlo() function
     *win    window the pointer is logically on
     *x      x location of the pointer relative to the window origin
     *y      y location of the pointer relative to the window origin
     mode, ctyp, answ - see ixreqlo(...)
     Author: Mario Stipcevic Jul, 1997
*/
int ixwptrq( x, y, win, mode, ctyp, mask )
int *x, *y, *win, mode, ctyp, mask;
{

   static Cursor cursor = (Cursor)LNULL;
   XEvent event;
   XSetWindowAttributes attributes;
   unsigned long attr_mask = 0;
   int button_press, radius, wid;
   int i, n, u;
   
   /* Set all inputs on */
   attributes.event_mask = mouse_mask | keybd_mask;
   attr_mask |= CWEventMask;
   for ( i = 0; i < MAXWN; i++ ) {
    ttws = &windows[i];
    if ( ttws->open ) {
     attributes.event_mask = mouse_mask | keybd_mask;
     attr_mask |= CWEventMask;
     XChangeWindowAttributes ( display, ttws->window, attr_mask, &attributes);
    }
   }


  /*
   *   Event loop a'la ixreqlo()
   */
   button_press = 0;
   n = 0;
/*
   while ( button_press == 0 ) {
*/
   while ( n == 0 ) {
   n = 1;

/*
   u=XEventsQueued(display,QueuedAlready);
   printf("ixwptrq ulaz: u = %d\n",u);
   XNextEvent( display, &event);
*/

   XMaskEvent( display, mouse_mask, &event);
   for (wid = 0; wid < MAXWN; wid++) {
    ttws = &windows[wid];
    if ( ttws->open && ttws->window == event.xbutton.window ) {
      *win = wid;
      goto Lab_001;
    }
   }
   *win = -1;
   return( button_press );
Lab_001:;

/*
printf("Here 1\n");
    XWindowEvent( display, event.xbutton.window, mouse_mask, &event );
    XWindowEvent( display, ttws->window, mouse_mask, &event );
printf("Here 2\n");
*/

    xloc = event.xbutton.x;
    yloc = event.xbutton.y;

    switch ( event.type ) {

     case LeaveNotify :
           if( mode == 0 ) {
              forever {
                 XNextEvent( display, &event);
                 if( event.type == EnterNotify ) break;
              }
           } else {
              button_press = -2;
           }
           break;

     case ButtonPress :
          button_press =  event.xbutton.button ;
          xlocp = event.xbutton.x;
          ylocp = event.xbutton.y;
/*
          XUndefineCursor( display, ttws->window );
          cursor = (Cursor)NULL;
*/
          break;

     case ButtonRelease :
           if( mode == 1 ) {
              button_press =  10+event.xbutton.button ;
              xlocp = event.xbutton.x;
              ylocp = event.xbutton.y;
           }
          break;

     case KeyPress :
           if( mode == 1 ) {
              button_press =  event.xkey.keycode;
              xlocp = event.xbutton.x;
              ylocp = event.xbutton.y;
           }
          break;

     case KeyRelease :
           if( mode == 1 ) {
              button_press =  -event.xkey.keycode;
              xlocp = event.xbutton.x;
              ylocp = event.xbutton.y;
           }
          break;

     default : break;
    }

    if( mode == 1 ) {
        if( button_press == 0 )
           button_press = -1;
        break;
    }
  }
Lab_99e:
  *x = event.xbutton.x;
  *y = event.xbutton.y;
  setinput(0);
  XFlush( display );
/*
 printf("ixwptrq: x=%d y=%d win=%d answ=%d\n", *x, *y, *win, button_press);
*/
  return( button_press );
}

/*
 *    SUBROUTINE IXCURSOR( ctyp )
 *    int ctyp = Cursor type as defined in <X11/cursorfont.h>
 *    Created by: Mario Stipcevic 07.01.1997
 */
 void ixcursor( ctyp )
int ctyp;
{
  Cursor cursor;

  XUndefineCursor( display, cws->window );
  cursor = XCreateFontCursor( display, ctyp );
  XDefineCursor( display, cws->window, cursor);
}

 void ixclrque()
/* Author: M.S. */
/* Flush and delete the X queue, leaving no events */
{
 XFlush( display );
 XSync( display , 1);
}

 void ixtitle( title, wid )
/* Set title of an existing window wid, if wid=-1 then current window */
int wid;
char title[200];
{
  char long_title[256];
  char host_name[64];

  tws = &windows[wid];
  if (wid < 0) tws = cws;
  if (tws->open) {
   strcpy(long_title, title);
   if (long_title[0] != '-') {
      strcat( long_title, " @ " );
      gethostname( host_name, sizeof( host_name ) );
      strcat( long_title, host_name );
   } else {
      long_title[0] = ' ';
   }
   XStoreName( display, tws->window, long_title );
   XSetIconName( display, tws->window, long_title );
  }
}

/* PIXEL and IMAGE manipulation routines, Auth. Mario Stipcevic, Apr. 1998 */

long 
   ixpget (int x, int y)
/* Get one pixel */
{
 XImage *image;
 long pixel, icol = 0;
/*
 image = XGetImage( display, cws->drawing, 0, 0,
                   cws->width, cws->height,
                   AllPlanes, ZPixmap);

 pixel = ((long) XGetPixel( image, x, y ));
 for (icol=0; icol<255; icol++) {
  if (colors[icol].defined && colors[icol].pixel==pixel) break;
 }
 XDestroyImage(image);
*/
 return ( icol );
}


void
   ixpset (int Rx, int Ry, int cindex )
/* Draw one pixel of a given color (faster than through ixline !) */
/* if cindex = -1 then keep old foreground */
{
 int x, y;
 Drawable d;

 if(!(d=cws->buffer)) d = cws->drawing; /* Handle buffer ixcrbuff() */

 if (rotate_fl || scale_fl || trans_fl) {
  x = xOrig + Rco * scalex * (Rx - xRO) + Rsi * scaley * (Ry - yRO)+.5;
  y = yOrig - Rsi * scalex * (Rx - xRO) + Rco * scaley * (Ry - yRO)+.5;
 } else {
  x = Rx;
  y = Ry;
 }

 if (cindex != -1)
  XSetForeground( display, *gcline, colors[cindex].pixel );
 XDrawPoint(display, d, *gcline, x, y);
}

void 
   ixpicget (x, y, w, h, array)
int x, y;   /* Position of Picture in the window */
int w, h;   /* width and height */
unsigned char *array; /* output array */
/* Get portion of the X-winwow in an (unsigned short int (2b)) array */
{
 XImage *image;
 int i, j;
 unsigned short int *table;
 unsigned long u, k;

 image = XGetImage( display, cws->drawing, 0, 0,
                   cws->width, cws->height,
                   AllPlanes , ZPixmap);
 
 table = (unsigned short int *)calloc(65536, sizeof(unsigned short int));
 /* Create pixel/color table */
 for (i = 0; i < 256; i++) {
  if (colors[i].defined) {
   if ((u = colors[i].pixel) < 65536) {
    table[u] = i;
   } else {
    printf("ixgetpal: Colordepth > 16 bits unsupported !!\n");
   }
  } else { table[u] = 0; }
 }

 /* Output array of colors */
 k = 0;
 for (j = 0; j < h; j++) {
  for (i = 0; i < w; i++) {
   array[k++] = table[ XGetPixel( image, i + x, j + y )];
  }
 }

 XDestroyImage(image);
}

void
 ixpicput(x, y, data, w, h, xoff, yoff, ws, hs)
/* Put picture stored in an (unsigned short int (2b)) array into a portion
 * of the X-winwow (inverse of ixpicget) */
int x, y;       /* Position where to put the image */
int xoff, yoff; /* Offset in the source image */
int ws, hs;     /* Dimensions of the rectangle of the source to be copied */
unsigned int w, h; /* Picture dimension in data */
unsigned char *data; /* input color array */
{
 XImage *image;
 Visual *visual=(Visual *)NULL;
 int i, j, k, depth;
 unsigned char *pxls8;
 unsigned short int *pxls16;
 void *pxls;
 Drawable d;

 if(!(d=cws->buffer)) d = cws->drawing; /* Handle buffer ixcrbuff() */

 /* Convert (input) color array into pixel array */
 depth = 1; if (DefaultDepth(display,screen_number) > 8) depth = 2;
 if (depth==1) pxls = pxls8  = malloc(w * h);
 if (depth==2) pxls = pxls16 = malloc((w * h) * 2);
 if (pxls)
 {
  k = 0;
  for (j = 0; j < h; j++) {
   for (i = 0; i < w; i++) {
    if (depth==1) *(pxls8 + k) = (unsigned char *) colors[data[k]].pixel;
    if (depth==2) *(pxls16 + k) = (unsigned short int *) colors[data[k]].pixel;
    k++;
   }
  }

  image = XCreateImage(display, visual, DefaultDepth(display,screen_number), ZPixmap, 0, pxls, w, h, 16, 0);
  /* XInitImage(image); ??? */
  XPutImage(display, d, *gcpxmp, image, xoff, yoff, x, y, ws, hs);

  XDestroyImage( image );
  /* Empty the event queue: */
  XSync( display, 1 );
 }
}

static int first_img = 1;  /* Animation flags */
static int  last_img = 0;

void
 ixpicanim(x, y, data, w, h, xoff, yoff, ws, hs)
/* Put picture stored in an (unsigned short int (2b)) array into a portion
 * of the X-winwow (inverse of ixpicget) */
int x, y;       /* Position where to put the image */
int xoff, yoff; /* Offset in the source image */
int ws, hs;     /* Dimensions of the rectangle of the source to be copied */
unsigned int w, h; /* Picture dimension in data */
unsigned char *data; /* input color array */
{
 static XImage *image;
 static XImage *image_old;
 Visual *visual=(Visual *)NULL;
 int i, j, k;
 static unsigned short int *pxls16 = NULL;
 static unsigned char *pxls8 = NULL;
 static void *pxls = NULL;
 static int xt, yt, xofft, yofft, wst, hst, wt, ht, depth;
 Drawable d;

 if(!(d=cws->buffer)) d = cws->drawing; /* Handle buffer ixcrbuff() */

 if (!data) {
  /* Check what to do with the last clip */
  if (draw_mode > 0)
   XPutImage(display, d, *gcpxmp, image_old, xofft, yofft, xt, yt, wst, hst);
  /* Free memory and reset flags */
  XDestroyImage( image_old );
  first_img = 1; last_img = 0; wt=0; ht=0; 
  return ;
 }

 if(w * h != wt * ht || !pxls) {
  if (pxls) XDestroyImage(image); /* Memory still leaks a bit ?? */
  depth = 1; if (DefaultDepth(display,screen_number) > 8) depth = 2;
  if (depth==1) pxls = pxls8  = malloc(w * h);
  if (depth==2) pxls = pxls16 = malloc((w * h) * 2);
 }

 /* Convert (input) color array into pixel array */
 k = 0;
 for (j = 0; j < h; j++) {
  for (i = 0; i < w; i++) {
   if (depth==1) *(pxls8 + k) = (unsigned char *) colors[data[k]].pixel;
   if (depth==2) *(pxls16 + k) = (unsigned short int *) colors[data[k]].pixel;
   k++;
  }
 }
 image = XCreateImage(display, visual, DefaultDepth(display,screen_number), ZPixmap, 0, pxls, w, h, 16, 0);
  /* XInitImage( image ); */

 if (!first_img) {
  XPutImage(display, d, *gcpxmp, image_old, xofft, yofft, xt, yt, wst, hst);
 }
 if (!last_img) {
  XPutImage(display, d, *gcpxmp, image, xoff, yoff, x, y, ws, hs);
 }

 xofft = xoff; yofft = yoff; xt = x; yt = y; wst = ws; hst = hs; wt = w; ht = h;
 image_old = image;
 if (first_img) first_img = 0;
 /* Empty the event queue: */
 XSync( display, 1 );
}


void
 xtest(w, h)
unsigned int w, h;
{
  int i;
     
/*
     pixmp = XCreatePixmap(display, RootWindow( display, screen_number),
                100, 100, DefaultDepth(display, screen_number));
     XCopyArea( display, cws->drawing, pixmp,
                *gcbuff, 0, 0, 100, 100, 0, 0 );
     XCopyArea( display, pixmp, cws->drawing,
                *gcbuff, 0, 0, 100, 100, 120, 80  );
*/
     pixmp = XCreatePixmap(display, RootWindow( display, screen_number), 
                100, 100, 1);
     XFillRectangle(display, pixmp, *gcline, 0, 0, 100, 100);
     for( i = 0; i < 6; i++ ) { XSetClipMask( display, gclist[i], pixmp );
     XSetClipOrigin(display, gclist[i], 0, 0); }

printf("depth=%d\n", DefaultDepth(display, screen_number));
/*
     for( i = 0; i < MAXGC; i++ ) { XSetClipMask( display, gclist[i], pixmp );
     XSetClipOrigin(display, gclist[i], 0, 0); }
     XSetClipOrigin(display, *gcline, 0, 0);
     for( i = 0; i < MAXGC; i++ ) XSetClipMask( display, gclist[i], pixmp );
     cws->clip  = 1;
     XFreePixmap(display, pixmp);
     for( i = 0; i < MAXGC; i++ ) XSetClipMask( display, gclist[i], pixmp );
*/
     XFlush( display );
}
