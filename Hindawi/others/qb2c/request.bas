REM Ask user his name, via an X-window
    SCREEN 10: REM Open the main window
    text$=""
    CALL Request2(text$, "Name request", "What is your name ?", 50, 50) 
    ?: ? "Answer was: "; text$
END


SUB Request2(answ$, wname$, req$, x%, y%)
REM  Open an interactive window for text input
REM  req$   - Answer to the user
REM  wname$ - Window label
REM  answ$  - User's answer
REM  x%, y% - Window position
     i% = 0: width% = 300
     XWINDOW (x%, y%), width%, 50, wname$
     PALETTE 3, 255, 255, 51
     LINE (0, 0)-(width%, 50), 3, BF
     PAUSE 0.1
     LINE (8, 22)-(width% - 8, 45), 0, BF
     LINE (8, 22)-(width% - 8, 45), 1, B
     SET TXCI 1
     XTEXT (10, 18), req$
     XUPDATE
     XWARP (width% / 2, 32) 

     XREQST (10, 39), answ$, status%
     IF status% = 0 THEN answ$ = ""
     XCLOSE
END SUB

