SCREEN 10             :REM Open a graphics (X11) window
PALETTE 2, 255, 0, 0  :REM Define red color for line as index 2
                      :REM (color 1 is black by defeault)
SET LWID 5            :REM Setline width to 5 pixels
LINE (100, 100) - (300, 200), 1
LINE (100, 200) - (300, 100), 2
SET LWID 2
SET TXAL 2, 2                          :REM Text alignment
CIRCLE (350, 150), 30, 2               :REM Draw a circle
CIRCLE (450, 150), 60, 1, 0., 6.3, .5  :REM Draw an elipse
XTEXT  (450, 150), "Elipse"            :REM Draw text
LINE (530, 135) - (550, 165), 2, BF    :REM Draw filled rectangle
LINE (530, 135) - (560, 165), 1, B     :REM Draw a rectangle
a$ = INKEY$           :REM Wait until any key is pressed
END
