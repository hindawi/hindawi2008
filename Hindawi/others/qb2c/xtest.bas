REM Program to test multiwindow environment under QB2C Basic
REM Open the first X-window:
 SCREEN (100, 100), 200, 100
 PALETTE 2, 255,   0,   0: REM Red
 PALETTE 3,   0, 255,   0: REM Green
 PALETTE 4,   0,   0, 255: REM Blue
 PALETTE 5, 255, 255, 255: REM Blue
 PAUSE 0.5
 
REM Open the second X-window:
 XWINDOW (200, 200), 200, 100
 XCLS 2
 PAUSE 0.5

REM Open the third X-window:
 XWINDOW (300, 300), 200, 100
 XCLS 3
 PAUSE 0.5

REM Open the fourth X-window:
 XWINDOW (400, 400), 200, 100
 XCLS 4
 PAUSE 0.5

 oldwin% = 0
10
 XPOINTER (x%, y%), win%, answ%

 XSELWI win%
 b$ = "This is the window" + str$(win%)
 SET TXCI 1
 XTEXT (5, 85), b$, 20

 IF win% <> oldwin% THEN
  XSELWI oldwin%
  XCLS
REM Instead of XCLS which clears whole window, you can use this:
REM  b$ = "This is the window" + str$(oldwin%)
REM  SET TXCI 0
REM  XTEXT (5, 85), b$, 20
  oldwin% = win%
 END IF

GOTO 10
END
