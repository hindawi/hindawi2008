    SCREEN (100, 100), 250, 100, "-Button test"
    PALETTE 2,   0,   0,   0: REM Dark color       (black)
    PALETTE 3, 204, 204, 204: REM Background color (grey)
    PALETTE 4, 255, 255, 255: REM Foreground color (white)
    PALETTE 5, 255,  23,   0: REM Red color
    XCLS 3: REM Set Background color
    x1 = 140: y1 = 60: x2 = 240: y2 = 90:   REM Button position
    SET TXCI 5: XTEXT (20, 40), "Press button to quit!": SET TXCI 2
    CALL Button(x1, y1, x2, y2, "Quit", "up")

50  REM Wait until button pressed
    GCGET (x%, y%), ans%
    log% = (x1<x% and x%<x2 and y1<y% and y%<y2)
    IF log% THEN
     SET DMOD 2, 2
     LINE (x1+4, y1+4)-(x2-4, y2-4), 2, B
     SET DMOD 0
     flag% = 1
     IF ans% = 1 THEN
      LINE (x1+4, y1+4)-(x2-4, y2-4), 0, B
      CALL Button(x1, y1, x2, y2, "Quit", "down")
      PAUSE 0.25
      GOTO 100
     END IF
    ELSE
     IF flag% THEN LINE (x1+4, y1+4)-(x2-4, y2-4), 0, B: flag% = 0
    END IF
    GOTO 50
    
100 REM Out of the button loop

END

SUB Button(x1, y1, x2, y2, text$, tip$)
REM Draw a 3-D button of width=dx, height=dy, at position (x%, y%)
REM and draw text text$ on the button.
REM If tip$="up" draw upward button, else draw depressed button
    IF tip$ = "up" THEN
     light = 4: idark = 2
    ELSE
     light = 2: idark = 4
    END IF
    SET LWID 2: LINE (x1, y1)-(x2, y2), light, B: SET LWID 1
    LINE (x2, y1)-(x2, y2), idark
    LINE (x1, y2)-(x2, y2), idark
    LINE (x2-1, y1+1)-(x2-1, y2), idark
    LINE (x1+1, y2-1)-(x2, y2-1), idark
    SET TXAL 2, 2: REM Text alignment: middle-x, middle-y
    XTEXT ((x1 + x2) / 2, (y1 + y2) / 2 - 1), text$
END SUB
