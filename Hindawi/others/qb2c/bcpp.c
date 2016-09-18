#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>

/* This is a Basic Text Preprocessor intended for preparing BASIC texts  */ 
/* before they are passed to QB2C (Microsoft QuickBasic to C) translator */
/* Last revision: 4.14 27.Oct 1999 */

#define LMAX 1024
#define CMAX 175 /* Max number of keywords */

/* Function declarations */
extern char  *MID_S(char *, int, int);
extern int    LEN(char *);
extern int    eof(FILE *);
extern char  *COMMAND_S(int, char *argv[]);
extern void lcmds__def(void);

/* Shared variables and arrays declarations */
static char   w__S[16][LMAX];
static int    i__w = 0;
static double x__d[16];
static int    i__x = 0;
static char tws__S[LMAX];
static char *cmds_S[] = { 
"END SUB",  "FUNCTION", "END FUNCTION", "EXIT SUB", "SHARED", "SUB", "DATA",
"CONST", "DECLARE", "DIM", "DYNAMIC", "IF", "THEN", "ELSEIF", "ELSE", "END IF",
"ENDIF", "GOTO",  "GO TO",  "GOSUB",  "RETURN",  "CALL",  "READ",  "RESTORE",
"FOR",   "TO",       "NEXT",   "DO WHILE", "LOOP",   "WHILE",  "WEND",
"DO UNTIL", "EOF", "LOF(", "PRINT", "EPRINT", "LINE INPUT", "INPUT", "STEP", 
"OPEN", "INPUT", "SSCAN", "OUTPUT", "USING", "APPEND", "RANDOM", "CLOSE", "AS",
"COMMAND$", "INKEY$", "DATE$",    "TIME$",   "TIMER",  "SHELL",  "REM",
"END",   "STATIC", "LET", "CLS", "COLOR", "LOCATE", "RANDOMIZE", "DEFINT",
"SPACE$","MID$", "LEFT$", "RIGHT$",  "STR$", "CHR$", "HEX$(", "OCT$(", "ASC",
"VAL", "LEN(", "LOG", "SIN", "COS", "TAN", "ATN", "SQR", "ABS", "EXP", 
"SGN", "RND", "INT(", "CINT","AND", "OR",  "NOT", "XOR", "MIN", "MAX", "PAUSE",
"SCREEN", "LINE", "PSET", "PALETTE", "SAVEGIF", "LOADGIF", "GIFINFO", "DEFDBL",
"XTEXT", "MARKER", "PMARKER", "PLINE", "XUPDATE", "XCLS", "FAREA", "CIRCLE",
"XWINDOW", "XSELWI", "XCLOSEDS", "XCLOSE", "GCGET", "XPOINTER", "XCURSOR",
"XTITLE", "XREQST", "XCLIP", "XNOCLI", "XWARP", "GET", "PUT", "XANIM",
"XROT", "XSCAL", "XTRAN", "MRESET", "XBUF", "XRMBUF", "DEFSTR", "STRING$",
"GETCOL", "XGETGE", "EXISTS", "LCASE$", "UCASE$", "ENVIRON$", "ENVIRON",
"TXLEN", "SET LCOL", "SET PLCI", "SET LWID", "SET DMOD", "SET PMCI",
"SET PMTS", "SET FASI", "SET FACI", "SET TXCI", "SET TFON", "SET TXAL",
"SET DRMD", "SET BG", "MOD", "SAVEPS", "SAVEPSL", "SAVEEPS", "ERASE", "REDIM",
"SEEK", "INP(", "OUT", "XLEN",
"x" };
static char lcmds_S[CMAX][16];
/* Popular replacements: */
static char *rplc_S[] = {"?",         "PRINT",
                         "GO TO",     "GOTO",
                         "ENDIF",     "END IF",
                         "SQRT(",     "SQR(",
"x"};
/* Literal replacements (no afterspace required): */
static char *litr_S[] = {"LINE(",     "LINE (",
                         "CIRCLE(",   "CIRCLE (",
                         "PSET(",     "PSET (",
                         "PRINT#",    "PRINT #",
                         "def fn",    "DEF FN",
"x"};
static int    imax = 0, jmax = 0, mmax = 0;

/* Open files pointers */
FILE *fp_1, *fp_2;

main(int n__arg, char *argv[])
{
 static int  leng_int, nctok_int, i_int, k_int, j_int, intflg_int, caseflg_int;
 static int  lleng_int, i,j,k,n,m,m1, togfl_int, brcnt_int, tmpfl_int, nl_int;
 static int  log1, log2, Cflg, c64flg, quietflg, rflag, flag1, flag2;
 static char c_S[LMAX], d_S[LMAX], b_S[LMAX], a_S[LMAX];
 static char inpf_S[LMAX], outf_S[LMAX], tmp_S[LMAX];
 static char c, pc, nc, fc, ct; /* current char, previouus, next, first */
 static char line_S[LMAX], nline_S[LMAX], w_S[LMAX];
 static char key; /* for specific '-' and '+' handling as unary */

 static char ctok_S[11][LMAX];

 strcpy(c_S,COMMAND_S(n__arg, argv));
 leng_int = LEN(c_S);
 lcmds__def();
 nctok_int = 0;
 i_int = 1;
 for(k_int = 1; k_int <= 5; k_int++)
 {
  while(strcmp(MID_S(c_S, i_int, 1), " ") == 0 && i_int <= leng_int)
  {
   i_int = i_int + 1;
  }
  j_int = i_int + 1;
  while(strcmp(MID_S(c_S, j_int, 1), " ") != 0 && j_int <= leng_int)
  {
   j_int = j_int + 1;
  }
  strcpy(ctok_S[k_int],MID_S(c_S, i_int, j_int - i_int));
  if(ctok_S[k_int][0]=='\0')
  {
   goto Lab_50005;
  }
  i_int = j_int + 1;
 }
Lab_50005:
 nctok_int = k_int - 1;
 j_int = 0;
 Cflg=0;
 c64flg=0;
 quietflg=0;
 rflag=0; /* flag for PP6 */
 for(i_int = 1; i_int <= nctok_int; i_int++)
 {
  strcpy(d_S,ctok_S[i_int]);
  if(strcmp(d_S, "-q") == 0 || strcmp(d_S, "-Q") == 0)
  {
   quietflg = 1;
  }
  else
  {
   if(strcmp(d_S, "-C64") == 0 || strcmp(d_S, "-c64") == 0)
   {
    c64flg = 1;
   }
   else
   {
    if(strcmp(d_S, "-c") == 0 || strcmp(d_S, "-C") == 0)
    {
     Cflg = 1;
    }
    else
    {
     if(strcmp(d_S, "-r") == 0)
     {
      rflag = 1;
     }
     else
     {
      if(strcmp(MID_S(d_S, 1, 1), "-") == 0)
      {
       printf("%s%s\n","Unrecognized option  ",d_S);
       exit(0);
      }
      else
      {
       j_int = j_int + 1;
       strcpy(ctok_S[j_int],d_S);
      }
     }
    }
   }
  }
 }
 nctok_int = j_int;
 if(nctok_int == 0 || nctok_int > 2)
 {
  printf("%s\n","Usage: bcpp [-option [...]] input_file[.bas] [output_file]");
  exit(0);
 }
 else
 {
  strcpy(inpf_S,ctok_S[1]);
  if(nctok_int >= 1)
  {
   i_int = 1;
   strcpy(b_S,inpf_S);
   while(i_int < LEN(inpf_S))
   {
    if(strcmp(MID_S(inpf_S, i_int, 1), ".") == 0)
    {
     strcpy(b_S,MID_S(inpf_S, 1, i_int - 1));
    }
    i_int = i_int + 1;
   }
   sprintf(inpf_S,"%s%s",b_S,".bas");
   sprintf(outf_S,"%s%s",b_S,".bcp");
  }
  if(nctok_int == 2)
  {
   strcpy(outf_S,ctok_S[2]);
  }
 }
 sprintf(tmp_S,"%s%s",b_S,".bcp");
 if(quietflg==0) printf("%s%s%s%s\n","bcpp: preprocessing ",inpf_S,"  -->  ",outf_S);
 /* SHELL "mv " + outf$ + " " + inpf$ */

 if((fp_1 = fopen(inpf_S, "r")) == NULL)
 {
  fprintf(stderr,"qb2c: cant open file %s\n",inpf_S); exit(0);
 }
 if((fp_2 = fopen(tmp_S, "w")) == NULL)
 {
  fprintf(stderr,"qb2c: cant open file %s\n",tmp_S); exit(0);
 }
 nl_int=0;
 while(! eof(fp_1))
 {
  fgets(line_S, LMAX, fp_1);
  line_S[strlen(line_S) - 1] = 0;
  nl_int++;

  /* PP0: linije komentirane sa 'rem' ili 'REM' ili 'C' */
  c=' '; j=0; strcpy(nline_S,line_S);
  while(line_S[j] == c) j++;
  if(memcmp(&line_S[j],"rem",3) == 0) {memcpy(&nline_S[j],"REM",3); goto Lab_99;}
  if(memcmp(&line_S[j],"REM",3) == 0) goto Lab_99;
  if(Cflg==1 && line_S[0]=='C')
  {
   if (line_S[1]==' ' || line_S[1]=='\0' || line_S[1]=='\t' || (memcmp(line_S,"CG",2)==0 || memcmp(line_S,"CH",2)==0 || memcmp(line_S,"CM",2)==0) && (line_S[2]==' ' || line_S[2]=='\0' || line_S[2]=='\t')) goto Lab_99;
  }
  /* PP1: intersticije u binarnim operacijama: + - * / ~ & | @ <> <= >= << >> */
  /* Ne rastavlja -,+ ako je ispred njih slovo e ili E */
  /* Ne rastavlja & ako ispred njega nije space */
  lleng_int = strlen(line_S);
  togfl_int=0;
  brcnt_int=0;
  key=1;
  j=0;
  pc='\0';
  nc=line_S[0];
  fc='\0'; /* first character in a word */
  for (i=0; i< lleng_int; i++)
  {
   c=nc;
   nc=line_S[i+1];
   if(c != ' ')
   {
    if(c == 34) togfl_int = 1 - togfl_int;
    if(togfl_int==0) 
    {
     if(c=='=' && pc!='>' && pc!='<' || c=='+' || c=='-' || c=='*' || c=='/' || c=='^' || c=='>' && pc!='<' && nc!='=' && nc!='>' || c=='<' && nc!= '>' && nc!='=' && nc!='<' || c=='~' || c=='&' && pc==' ' || c=='|' || c=='@')
     {
      if(c=='-' || c=='+')
      {
       fc='\0'; k=i;
       if (i >= 2) fc=line_S[i-2];
       if((pc=='e' || pc=='E') && fc>='0' && fc<='9')
       {
        nline_S[j]= c ; j++;
       }
       else
       {
        if (key) 
        {
         nline_S[j]=' '; j++;
         nline_S[j]= c ; j++;
        }
        else
        {
         nline_S[j]=' '; j++;
         nline_S[j]= c ; j++;
         nline_S[j]=' '; j++;
        }
       }
      }
      else
      {
       nline_S[j]=' '; j++;
       nline_S[j]= c ; j++;
       nline_S[j]=' '; j++;
      }
      key=1;
     }
     else
     {
      if(c==',' || c==';' || c==':')
      {
       nline_S[j]= c ; j++;
       nline_S[j]=' '; j++;
       if(c==',') key=1;
      }
      else 
      {
       if(c=='<' && (nc=='>' || nc=='<' || nc=='='))
       {
        nline_S[j]=' '; j++;
        nline_S[j]='<'; j++;
        nline_S[j]= nc; j++;
        nline_S[j]=' '; j++;
        i++; c=nc; nc=line_S[i+1];
        key=1;
       }
       else
       {
        if(c=='>' && (nc=='>' || nc=='='))
        {
         nline_S[j]=' '; j++;
         nline_S[j]='>'; j++;
         nline_S[j]= nc; j++;
         nline_S[j]=' '; j++;
         i++; c=nc; nc=line_S[i+1];
         key=1;
        }
        else { nline_S[j]= c ; j++; key=0;}
       }
      }
     }
     if(i>=3){ if(memcmp(&line_S[i-3],"STEP",4)==0 || memcmp(&line_S[i-3],"step",4)==0) key=1; }
    }
    else { nline_S[j]= c ; j++; }
   }
   else { nline_S[j]= c ; j++; }
   pc=line_S[i];
  } 
  /* PP2: Ukidanje visestrukih spaceova ... (osim lijevih) */
  nline_S[j]='\0';
  lleng_int=j;
  j=0;
  c=' ';
  while(nline_S[j] == c) j++;
  tmpfl_int=0;
  togfl_int=0;
  for(i=j; i<lleng_int; i++)       
  {
   c=nline_S[i]; 
   if(c == 34) togfl_int = 1 - togfl_int;
   if(c == ' ' && togfl_int == 0)
   {
    if (tmpfl_int==0) { nline_S[j]= c ; j++; tmpfl_int=1; }
   }
   else { nline_S[j]= c ; j++; tmpfl_int=0; }
  }
  nline_S[j]='\0';
  /* Ako je bilo vise lijevih spaceova ostao je jedan, obrisi i njega: */
  if(nline_S[j-1]==' ') nline_S[--j]='\0';
  strcpy(w_S,nline_S);

  /* ... i ukidanje nezeljenih spc kod (_ _) _, _: */
  lleng_int=j;
  j=0;
  pc='\0';
  tmpfl_int=0;
  togfl_int=0;
  for(i=0; i<lleng_int; i++)       
  {
   c=w_S[i]; 
   ct=c;
   if(c == 34) togfl_int = 1 - togfl_int;
   if(togfl_int == 0)
   {
    if((c==')' || c==',' || c==':') && pc==' ') j--; 
    if(c==' ' && pc=='(') { j--; c='('; }
   }
   nline_S[j]=c; j++;
   pc=ct;
  }
  nline_S[j]='\0';
  lleng_int = j;

  /* PP3: Pretvorba lower case kljucnih rijeci u upper case */
  if (c64flg == 0) { /* no C64 */
  for(i=0; i<imax; i++)
  {
   n = strlen(strcpy(b_S, lcmds_S[i]));
   togfl_int = 0;
   for(j=0; j <= lleng_int - n; j++)
   {
    if (nline_S[j] == 34) togfl_int = 1 - togfl_int;
    if (memcmp(b_S,&nline_S[j],n) == 0 && togfl_int == 0) 
    {
     c=0; if(j > 0) c=nline_S[j-1];
     log1=(c==' ' || c==0 || c=='(');
     c=0; if(j+n < lleng_int) c=nline_S[j+n];
     log2=(c==' ' || c==0 || c=='(' || c==')' || c==':' || c==',' || nline_S[j+n-1]=='(');
     if(log1 && log2)
     {
      strncpy(&nline_S[j],cmds_S[i],n);
      j=j+n-1;
     }
    }
   }
  }  }
  else /* C64 specific */
  {
   for(i=0; i<imax; i++)
   {
    n = strlen(strcpy(b_S, lcmds_S[i]));
    for(j=0; j <= lleng_int - n; j++)
    {
     if (nline_S[j] == 34) togfl_int = 1 - togfl_int;
     if (memcmp(b_S,&nline_S[j],n) == 0 && togfl_int == 0) 
     {
      strncpy(&nline_S[j],cmds_S[i],n);
      j=j+n-1;
     }
    } 
   }
  }
  /* PP4: Brojanje zagrada (brackets ballance checking) */
  lleng_int = strlen(nline_S);
  togfl_int=0;
  brcnt_int=0;
  for(i=0; i<lleng_int; i++)       
  {
   if (memcmp(&nline_S[i],"REM",3) == 0) goto PP5;
   c=nline_S[i];
   if(c == 34) togfl_int = 1 - togfl_int;
   if(togfl_int==0)
   {
    if(c == '(') brcnt_int=brcnt_int + 1;
    if(c == ')') brcnt_int=brcnt_int - 1;
   }
  }
  if(brcnt_int != 0) {fprintf(stderr,"bcpp: Error: brackets not ballanced in line No %d: %s\n",nl_int,w_S); exit(1);}
PP5:
  /* PP5: Popular syntax extensions and literal replacements, caution "?" */
  lleng_int = strlen(nline_S);
  for(i=0; i<jmax || i<mmax; i=i+2)
  {
   flag1=0; if(i < jmax) {flag1=1; n = strlen(strcpy(b_S, rplc_S[i]));}
   flag2=0; if(i < mmax) {flag2=1; m = strlen(strcpy(a_S, litr_S[i]));}
   togfl_int = 0;
   for(j=0; j <= lleng_int - n; j++)
   {
    if (nline_S[j] == 34) togfl_int = 1 - togfl_int;

    if (flag2 && memcmp(&nline_S[j],a_S,m) == 0 && !togfl_int)
    {
     m1=strlen(litr_S[i+1]);
     memmove(&nline_S[j+m1],&nline_S[j+m],lleng_int-(j+m)+1);
     memmove(&nline_S[j],litr_S[i+1],m1);
     j=j+m1-1;
     lleng_int=strlen(nline_S);
     goto pp59;
    }

    if (flag1 && memcmp(&nline_S[j],b_S,n) == 0 && !togfl_int)
    {
     c=0; if(j > 0) c=nline_S[j-1];
     log1=!(c>='a' && c<='z' || c>='A' && c<='Z' || c=='_' || c>='0' && c<='9');
     c=0; if(j+n < lleng_int-1) c=nline_S[j+n];
     log2=!(c>='a' && c<='z' || c>='A' && c<='Z' || c=='_');
     if(log1 && log2)
     {
      strcpy(d_S,nline_S);
      nline_S[j]='\0';
      strcat(nline_S,rplc_S[i+1]);
      strcat(nline_S,&d_S[j+n]);
      j=j+strlen(rplc_S[i+1])-1;
      lleng_int=strlen(nline_S);
     }
    }

pp59:;
   }
  }
  /* PP6: Integer fractals -> real fractals ie. 2 / 3 -> 2 / (double)3 */
  if(rflag) {
   i=0; togfl_int=0;
   while((c=nline_S[i]) != '\0') {
    if(c == 34) togfl_int = 1 - togfl_int;
    if(!togfl_int && c=='/') {
     memmove(&nline_S[i+10],&nline_S[i+2],strlen(nline_S)-i);
     memcpy(&nline_S[i+2],"(double)",8); i+=8;
    }
    i++;
   }
  }
  Lab_99:
  /* printf("%s\n",nline_S); */
  fprintf(fp_2,"%s\n",nline_S);
 } 
 fclose(fp_2);
 fclose(fp_1);
 
 if(nctok_int == 2)
 {
  sprintf(c_S,"mv -f %s %s",tmp_S,outf_S);
  system(c_S);
 }
} /* End of MAIN */


/* Translates of used QB's intrinsic functions: */

extern char *MID_S(char *a_S, int start, int length)
{

 if (i__w++ == 16) i__w=0;
 if(length < 0) { 
  printf("bcpp: Error: in MID_S: length < 0\n");
  exit(1); }
 if(start  < 0) {
  printf("bcpp: Error: in MID_S: start < 1\n");
  exit(1); }
 strncpy(w__S[i__w], &a_S[start-1], length);
 w__S[i__w][length]=0;

 return w__S[i__w];
}

extern int LEN(char *a_S)
{
 if (i__x++ == 16) i__x = 0;
 x__d[i__x] = strlen(a_S);
 return x__d[i__x];
}

extern int eof(FILE *stream)
{
 static int c, istat;

 istat=((c=fgetc(stream))==EOF);
 ungetc(c,stream);
 return istat; 
}

extern char *COMMAND_S(int n__arg, char *argv[])
{
 int i;

 if (i__w++ == 16) i__w=0;
 for(i = 1; i < n__arg; i++)
 {
  strcat(w__S[i__w],argv[i]);
  strcat(w__S[i__w]," ");
 }
 w__S[i__w][strlen(w__S[i__w])-1]=0;
 return w__S[i__w];
}

extern void lcmds__def(void)
{
 static int i=0, j;
 static char w_S[16];

 while(strcmp(strcpy(w_S,cmds_S[i]),"x") != 0)
 {
  if (i >= CMAX)
  { fprintf(stderr, "bcpp: Too many commands. Enlarge CMAX.\n"); exit(-1); }
  for(j = 0; j < strlen(w_S); j++)
  {
   lcmds_S[i][j]=tolower(w_S[j]); 
  }
  i++;
 }
 imax=i;
 while(strcmp(rplc_S[jmax],"x") != 0)
 jmax++;
 i=0; while(strcmp(litr_S[i++],"x") != 0) mmax++;
}
