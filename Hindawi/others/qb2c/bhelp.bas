REM bhelp for QB2C versions 3.2k and later - by Mario Stipcevic
REM This is a QB2C code. Compile it with: bcc
c$ = COMMAND$
line$ = "": nl% = 0
REM local_path$ = 
REM src_path$ = 
REM if exists()
helpf$ = "manual.txt"
IF c$ <> "" THEN
 OPEN helpf$ FOR INPUT AS #1
  key$ = "o " + UCASE$(c$)
  leng% = LEN(key$)
  WHILE (LEFT$(line$, 20) <> "+ Reference Manual +") AND NOT EOF(1)
   LINE INPUT #1, line$: nl% = nl% + 1
  WEND
  WHILE (LEFT$(line$, leng%) <> key$) AND NOT EOF(1)
   LINE INPUT #1, line$: nl% = nl% + 1
  WEND
 CLOSE #1
 SHELL "vi +" + MID$(STR$(nl%), 2) + " " + helpf$
ELSE
 SHELL "vi " + helpf$
END IF
END
