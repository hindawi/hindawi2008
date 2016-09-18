    DIM a?(25000): REM array large enough to hold a GIF (one byte per pixel)
    SCREEN (100, 100), 300, 200   :REM Open a window
    XCLS 1                        :REM Set background
    GIFINFO "toon.gif", w%, h%    :REM Get GIF dimensions
    LOADGIF (0, 0), "toon.gif", 0 :REM Load the GIF into the window
   
    GET (0, 0), a?, 150, 100      :REM Get the picture into array a?()

    SET DRMD 2
    step% = 1

    FOR j=1 TO 100                :REM Repeat animation sequence 100 times
     FOR i% = 50 TO step% STEP -step%
      XANIM (100-2*i%, 100+i%), a?, 150, 100, (i%, i%), 100-2*i%, 100-2*i%
      XUPDATE
      PAUSE 0.005
     NEXT
     FOR i% = 1 TO w%/2
      XANIM (100+i%, 100+i%), a?, 150, 100, (i%, i%), w%-2*i%, h%-2*i%
      XUPDATE
      PAUSE 0.005
     NEXT
     FOR i% = w%/2 TO 1 STEP -1
      XANIM (100+i%, 100+i%), a?, 150, 100, (i%, i%), w%-2*i%, h%-2*i%
      XUPDATE
      PAUSE 0.005
     NEXT
    NEXT j

    SET DRMD 1
    XANIM (0, 0), 0, 0, 0         :REM Release memory used by XANIM

    INPUT "Press <enter> to quit..."; a$
END
