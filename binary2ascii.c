/* Time-stamp: <2008-10-06 17:21:26 poser>
 *
 * This program reads binary input and converts it to ASCII textual representation.
 *
 * Copyright (C) 2005-2008 William J. Poser (billposer@alum.mit.edu)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 3 the GNU General Public License
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 * or go to the web page:  http://www.gnu.org/licenses/gpl.txt.
 */

#include "config.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_GETOPT_LONG
#define _GNU_SOURCE
#include <getopt.h>
#endif
#include <errno.h>
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#ifdef HAVE_LIBINTL_H
#include <libintl.h>
#else
#define gettext(x) (x)
#endif
#include "exitcodes.h"

#define TMPSIZE 12

#ifndef HAVE_GETOPT_LONG

struct option
{
# if (defined __STDC__ && __STDC__) || defined __cplusplus
  const char *name;
# else
  char *name;
# endif
  /* has_arg can't be an enum because some compilers complain about
     type mismatches in all the code that assumes it is an int.  */
  int has_arg;
  int *flag;
  int val;
};

int
getopt_long(int ac,
	    char *const *av,
	    const char * sopts,
	    const struct option *lopts, 
	    int *lind){

  return(getopt(ac,av,sopts));
}
#endif

char pgname[]="binary2ascii";
char version[]=PACKAGE_VERSION;
char compdate[]=__DATE__ " " __TIME__ ;

void
Usage(void){
  fprintf(stderr,gettext("Filter to convert binary numbers to textual representation.\n"));
  fprintf(stderr,gettext("Usage: %s [flags]\n"),pgname);
  fprintf(stderr,gettext("Options:\n"));
  fprintf(stderr,gettext("\t-b <base>       set base for integer conversions:\n"));
  fprintf(stderr,gettext("\t                b(inary)        or  2\n")); 
  fprintf(stderr,gettext("\t                d(ecimal)       or 10\n"));
  fprintf(stderr,gettext("\t                h(exadecimal)   or 16\n"));
  fprintf(stderr,gettext("\t                o(ctal)         or  8\n"));
  fprintf(stderr,gettext("\t-d              delimit as per locale\n"));
  fprintf(stderr,gettext("\t-D              do not delimit as per locale\n"));
  fprintf(stderr,gettext("\t-e              use exponential (scientific) notation\n"));
  fprintf(stderr,gettext("\t-h              print this help message\n"));
  fprintf(stderr,gettext("\t-L <locale>     set the locale\n"));
  fprintf(stderr,gettext("\t-l              add a linefeed after each 0x0A\n"));
  fprintf(stderr,gettext("\t-n <number>     number of items per line [default 1]\n"));
  fprintf(stderr,gettext("\t-o <offset>     byte offset at which to start\n"));
  fprintf(stderr,gettext("\t-p <precision>\n"));
  fprintf(stderr,gettext("\t-s              print sizes of types on this machine\n"));
  fprintf(stderr,gettext("\t-t <type>       specify type of input. The default is uc.\n"));
  fprintf(stderr,"\t                d [double]\n");
  fprintf(stderr,"\t                f  [float]\n");
#ifdef HAVE_LONG_DOUBLE
  fprintf(stderr,"\t                q  [long double]\n");
#endif
  fprintf(stderr,"\t                sc [signed char]\n");
  fprintf(stderr,"\t                ss [signed short]\n");
  fprintf(stderr,"\t                si [signed int]\n");
  fprintf(stderr,"\t                sl [signed long]\n");
  fprintf(stderr,"\t                sq [signed long long]\n");
  fprintf(stderr,"\t                uc [unsigned char]\n");
  fprintf(stderr,"\t                us [unsigned short]\n");
  fprintf(stderr,"\t                ui [unsigned int]\n");
  fprintf(stderr,"\t                ul [unsigned long]\n");
  fprintf(stderr,"\t                uq [unsigned long long]\n");
  fprintf(stderr,gettext("\t-X              print explanation of exit codes\n"));
  fprintf(stderr,gettext("\t-x              do not mark hex output with prefix 0x\n"));
  fprintf(stderr,gettext("\t-V              be verbose\n"));
  fprintf(stderr,gettext("\t-v              identify program version\n"));
  fprintf(stderr,gettext("\t-w <width>      minimum field width\n"));
  fprintf(stderr,gettext("\t-z              zero pad integers\n"));
  fprintf(stderr,gettext("\t-Z              do not zero pad integers\n"));
}

void
Version(void){
  fprintf(stderr,"%s %s \n",pgname,version);
  fprintf(stderr,gettext("Compiled %s.\n"),compdate);
  fprintf(stderr,"Copyright 2005-2008 William J. Poser\n");
  fprintf(stderr,"This program is free software; you can redistribute\n\
it and/or modify it under the terms of version 3 of\n\
the GNU General Public License as published by the\n\
Free Software Foundation.\n");
  fprintf(stderr,gettext("Report bugs to: billposer@alum.mit.edu.\n"));
}


/* Compute default field width for integer types */
int
Compute_Field_Width(int size, int base)
{
  switch(base)
    {
    case 8:
      return (size * 3);
      break;
    case 10:
      return (size * 3);
      break;
    default:			/* hex */
      return (size * 2);
      break;
    }

}

struct option opts[]={
  "base",1,NULL,'b',
  "delimit",0,NULL,'d',
  "do-not-delimit",0,NULL,'D',
  "exponential",0,NULL,'e',
  "help",0,NULL,'h',
  "locale",1,NULL,'L',
  "linefeed",0,NULL,'l',
  "number",1,NULL,'n',
  "offset",1,NULL,'o',
  "precision",1,NULL,'p',
  "sizes",0,NULL,'s',
  "type",1,NULL,'t',
  "verbose",0,NULL,'V',
    "version",0,NULL,'v',
  "width",1,NULL,'w',
  "explain-exit-codes",0,NULL,'X',
  "no-hex-mark",0,NULL,'x',
  "zero-pad-integers",0,NULL,'z',
  "do-not-zero-pad-integers",0,NULL,'Z',
    0,0,0,0
};

int
main(int ac, char **av)
{
   int Flag;
   int infd;
   int outfd;
   size_t Item_Size;		/* Number of bytes in one input number */
   char *inbufptr;
   char tmpstr[TMPSIZE];
   char format[32];		/* Print format which we will construct */
   int Bytes_Read;
   int Field_Width_Specified_P = 0;
   int Precision_Specified_P = 0;
   int Mark_Hex_P = 1;
#ifdef HAVE_PRINTF_THSEP
   int Delimit_P = 1;
#else
   int Delimit_P = 0;
#endif
   int Verbose_P = 0;
   struct stat stbuf;
   unsigned long Tokens_Processed;
   char cb;			/* Used to store each byte as read */
   char *endptr;		/* Used by strtol */
   int lgoindex;		/* Unused but needed by getopt_long */

   int Base = 10;		/* Base for integer conversions */
   int Exponential_P = 0;	/* Use exponential (scientific) notation for floats? */
   int Items_Per_Line = 1;	/* Number of items to print per line */
   long Current_Byte_Offset  = 0L;
   long Start_Byte_Offset  = 0L;	
   int Precision    = 3;	/* Precision for floats */
   int Field_Width   = 6;	/* Minimum field width for output */
   int Zero_Pad_P     = 0;	/* Zero pad output on left? */
   short AddNewlineP = 0;	/* Add newline after 0x0A? */
   short EmittedLFP = 0;
   int Seek_Successful_P = 0;

   enum bt  {BSIGNED, BUNSIGNED, BFLOAT} Binary_Type;
#ifdef HAVE_LONG_DOUBLE
   enum bs  {BCHAR, BSHORT, BINT, BLONG, BLLONG, BSINGLE, BDOUBLE,BLDOUBLE} Binary_Size;
#else
   enum bs  {BCHAR, BSHORT, BINT, BLONG, BLLONG, BSINGLE, BDOUBLE} Binary_Size;
#endif

   extern char *binfmtc (unsigned char);
   extern char *binfmts (unsigned short);
   extern char *binfmti (unsigned int);
   extern char *binfmtl (unsigned long);
   extern char *binfmtq (unsigned long long);

   extern void Print_Byte_Order(void);
   extern void Print_ExitCodes(void);
   extern void Print_Sizes(void);

   Binary_Type = BUNSIGNED;
   Binary_Size = BCHAR;
   Tokens_Processed = 0L;



#ifdef HAVE_SETLOCALE
   setlocale(LC_ALL,"");	/* This has the effect of introducing locale-specific notions
				   of whitespace into the parse done by Get_Word and adding
				   locale-specific formats to those recognized by strto* */
#endif
#ifdef HAVE_LIBINTL_H
   bindtextdomain (PACKAGE, LOCALEDIR);
   textdomain (PACKAGE);
#endif

   /* Handle command line arguments */

   opterr = 0;			/* We'll handle errors ourselves */
  while ((Flag = getopt_long(ac,av,":b:DdehlL:n:o:p:st:Vvw:XxZz",&opts[0],&lgoindex)) != EOF){
    switch(Flag) {
      case 'L':
#ifdef HAVE_LOCALE_H
	 if(setlocale(LC_NUMERIC,optarg) == NULL) {
	   fprintf(stderr,"The locale %s is not available.\n",optarg);
	   exit(LOCALENOTAVAILABLE);
	 }
#else
	 fprintf(stderr,"This system does not support locales.\n");
	 exit(COMMANDLINEERROR);
#endif
	 break;
      case 'b':
       switch(tolower(optarg[0]))
	 {
	 case 'b':
	   Base = 2;
	   break;
	 case 'd':
	   Base = 10;
	   break;
	 case 'h':
	   Base = 16;
	   break;
	 case 'o':
	   Base = 8;
	   break;
	 default:
	   Base = strtoul(optarg,&endptr,10);
	   if(errno == ERANGE){
	     fprintf(stderr,gettext("%s: base of %s falls outside of the representable range\n"),
		     pgname,optarg);
	     exit(COMMANDLINEERROR);
	   }
	   if(errno == EINVAL){
	     fprintf(stderr,gettext("%s: base of %s is ill-formed\n"), pgname,optarg);
	     exit(COMMANDLINEERROR);
	  }
	   if(*endptr != '\0'){
	     fprintf(stderr,gettext("%s: base of %s is ill-formed\n"), pgname,optarg);
	     exit(COMMANDLINEERROR);
	   }
	 }
       break;
    case 'D':
       Delimit_P = 0;
       break;
    case 'd':
#ifdef HAVE_PRINTF_THSEP
       Delimit_P = 1;
#else
       fprintf(stderr,"This system does not support thousands separation.\n");
       exit(COMMANDLINEERROR);
#endif
       break;
     case 'e':
       Exponential_P = 1;
       break;
     case 'h':
       Usage();
       exit(INFO);
     case 'l':
       AddNewlineP = 1;
       break;
     case 'n':
       Items_Per_Line = atoi(optarg);
       break;
     case 'o':
       Start_Byte_Offset = atol(optarg);
       break;
     case 'p':
       Precision = atoi(optarg);
       Precision_Specified_P = 1;
       break;
     case 's':
       Print_Sizes();
       putc('\n',stderr);
       Print_Byte_Order();
       exit(INFO);
       break;			/* NOTREACHED */
     case 't':			/* Set type of output */
       switch(tolower(optarg[0])){
          case 'u':
	    Binary_Type = BUNSIGNED;
	    break;
          case 's':
	    Binary_Type = BSIGNED;
	    break;
          case 'f':
	    Binary_Type = BFLOAT;
	    Binary_Size = BSINGLE;
	    break;
          case 'd':
	    Binary_Type = BFLOAT;
	    Binary_Size = BDOUBLE;
	    break;
#ifdef HAVE_LONG_DOUBLE
          case 'q':
	    Binary_Type = BFLOAT;
	    Binary_Size = BLDOUBLE;
	    break;
#endif
          default:
	    fprintf(stderr,gettext("%s: unrecognized output type %c\n"),pgname,optarg[0]);
	    exit(COMMANDLINEERROR);
       }
       if(Binary_Type == BFLOAT) break;
       if(strlen(optarg) < 2){
	 fprintf(stderr,gettext("%s: output type %s does not specify size\n"),pgname,optarg);
	 exit(COMMANDLINEERROR);
       }
       switch(tolower(optarg[1])){
          case 'c':
	    Binary_Size = BCHAR;
	    break;
          case 's':
	    Binary_Size = BSHORT;
	    break;
          case 'i':
	    Binary_Size = BINT;
	    break;
          case 'l':
	    Binary_Size = BLONG;
	    break;
          case 'q':
	    Binary_Size = BLLONG;
	    break;
          default:
	    fprintf(stderr,gettext("%s: unrecognized output size %c\n"),pgname,optarg[1]);
	    exit(COMMANDLINEERROR);
       }
       break;
     case 'V':
       Verbose_P =1;
       break;
     case 'v':
       Version();
       exit(INFO);
     case 'X':
       Print_ExitCodes();
       exit(INFO);
     case 'w':
       Field_Width = atoi(optarg);
       Field_Width_Specified_P = 1;
       break;
     case 'x':
       Mark_Hex_P = 0;
       break;
     case 'z':
       Zero_Pad_P = 1;
       break;
    case 'Z':
      Zero_Pad_P = 0;
      break;
     case ':':
       fprintf(stderr,gettext("%s: missing argument to option flag %c.\n"),pgname,optopt);
       exit(COMMANDLINEERROR);
     default:
       fprintf(stderr,gettext("%s: invalid option flag %c\n"),pgname,optopt);
       Usage();
       exit(COMMANDLINEERROR);
     }
   }

   /* Sanity checks */

   if( (Base !=2) && (Base !=8) && (Base != 10) && (Base != 16)) {
     fprintf(stderr,gettext("Base of %d not permitted. Possible bases are: 2, 8, 10, and 16.\n"),Base);
     exit(COMMANDLINEERROR);
   }

   if( (Base != 10) && (Binary_Type == BFLOAT)){
     fprintf(stderr,gettext("Floating point numbers must be printed in decimal.\n"));
     exit(COMMANDLINEERROR);
   }

#ifdef HAVE_LONG_DOUBLE
   if( ((Binary_Size == BSINGLE) || (Binary_Size == BDOUBLE) || (Binary_Size == BLDOUBLE)) && (Binary_Type != BFLOAT)){
#else
   if( ((Binary_Size == BSINGLE) || (Binary_Size == BDOUBLE)) && (Binary_Type != BFLOAT)){
#endif
     fprintf(stderr,gettext("%s: impermissible combination of output type and size \n"),pgname);
     exit(COMMANDLINEERROR);
   }
   if( (Binary_Type == BSIGNED) && ((Base ==8) || (Base == 16)) ){
     fprintf(stderr,gettext("Signed integers may be printed only in decimal or binary.\n"));
     exit(COMMANDLINEERROR);
   }
   if(Field_Width < 1){
     fprintf(stderr,gettext("Field width of %d does not make sense.\n"),Field_Width);
     exit(COMMANDLINEERROR);
   }
   if (Precision < 0){
     fprintf(stderr,gettext("Precision of %d does not make sense.\n"),Precision);
     exit(COMMANDLINEERROR);
   }
   if (Start_Byte_Offset < 0L){
     fprintf(stderr,gettext("Byte offset of %d does not make sense.\n"),Start_Byte_Offset);
     exit(COMMANDLINEERROR);
   }
   if(Items_Per_Line < 1){
     fprintf(stderr,gettext("Items per line of %d does not make sense.\n"),Items_Per_Line);
     exit(COMMANDLINEERROR);
   }

   if(Precision > 50){
     fprintf(stderr,gettext("Precision of %d is excessive\n"),Precision);
     exit(COMMANDLINEERROR);
   }
   if (Field_Width > 50){
     fprintf(stderr,gettext("Field width of %d is excessive\n"),Field_Width);
     exit(COMMANDLINEERROR);
   }   
   if(Exponential_P && (Binary_Type != BFLOAT) ){
     fprintf(stderr,gettext("Warning: -e flag has no effect with integral data types.\n"));
   }

#ifndef HAVE_PRINTF_THSEP_FLOAT
   if( Delimit_P && (BFLOAT == Binary_Type)) {
     fprintf(stderr,
        "This system does not support thousands separation for floating point numbers.\n");
     exit(COMMANDLINEERROR);
   }
#endif

   switch(Binary_Size){
   case BCHAR:
     Item_Size = sizeof(char);
     break;
   case BSHORT:
     Item_Size = sizeof(short);
     break;
   case BINT:
     Item_Size = sizeof(int);
     break;
   case BLONG:
     Item_Size = sizeof(long);
     break;
   case BLLONG:
     Item_Size = sizeof(long long);
     break;
   case BSINGLE:
     Item_Size = sizeof(float);
     break;
   case BDOUBLE:
     Item_Size = sizeof(double);
     break;
#ifdef HAVE_LONG_DOUBLE
   case BLDOUBLE:
     Item_Size = sizeof(long double);
     break;
#endif
   default:			/* NOTREACHED */
     break;
   }

   if(!Field_Width_Specified_P){
     switch(Binary_Size)
       {
       case BFLOAT:
	 Field_Width = 6;
	 break;
       case BDOUBLE:
	 Field_Width = 8;
	 break;
#ifdef HAVE_LONG_DOUBLE
       case BLDOUBLE:
	 Field_Width = 10;
	 break;
#endif
       default:
	 Field_Width = Compute_Field_Width(Item_Size,Base);
	 break;
       }
   }

   if(!Precision_Specified_P){
     if(Binary_Type == BFLOAT) Precision = 3;
     else Precision = 6;
   }

   if(  8 ==Base) Zero_Pad_P = 1;
   if((BFLOAT != Binary_Type) && ((16 == Base) || (8 == Base)) && Mark_Hex_P) Zero_Pad_P = 1;

   inbufptr = (char *) malloc(Item_Size);
   if(inbufptr == NULL){
     fprintf(stderr,gettext("Failed to allocate storage for input buffer.\n"));
     exit(SYSTEMERROR);
   }

   if(Base != 2){   /* Construct the print format */
     format[0] = '\0';
     if(Mark_Hex_P && Base == 16) strcat(format,"0x");
     strcat(format,"%");
     if(Delimit_P) strcat(format,"\'");
     if(Zero_Pad_P) strcat(format,"0");
     snprintf(tmpstr,TMPSIZE,"%d",Field_Width);
     strcat(format,tmpstr);
     if(Binary_Type == BFLOAT){
       snprintf(tmpstr,TMPSIZE,".%d",Precision);
       strcat(format,tmpstr);
     }
     if(Binary_Type == BFLOAT){
#ifdef HAVE_LONG_DOUBLE
       if(Binary_Size == BLDOUBLE) strcat(format,"L");
#endif
       if(Exponential_P) strcat(format,"e");
       else strcat(format,"f");
     }
     else{
       switch(Binary_Size)	/* Insert size modifiers */
	 {
	 case BCHAR:
	   strcat(format,"hh");
	   break;
	 case BSHORT:
	   strcat(format,"h");
	   break;
	 case BLONG:
	   strcat(format,"l");
	   break;
	 case BLLONG:
	   strcat(format,"ll");
	   break;
	 default:
	   break;
	 } /* End of switch on Binary_Size */
       switch(Base)
	 {
	 case 8:
	   strcat(format,"o");
	   break;
	 case 10:
	   if(Binary_Type == BSIGNED)  strcat(format,"d");
	   else strcat(format,"u");
	   break;
	 case 16:
	   strcat(format,"X");
	   break;
	 default:
	   break;	   /* NOTREACHED */
	 } /* End of switch on Base */
     } /* End of else enclosing integer types */

     if (Verbose_P) fprintf(stderr,"format: %s\n",format);

   } /* End of section for bases other than two */

   infd = fileno(stdin);
   outfd = fileno(stdout);

   if(Start_Byte_Offset > 0L){
     while( (Bytes_Read = read(infd,&cb,1))){
       Current_Byte_Offset++;
       if(Current_Byte_Offset == Start_Byte_Offset) break;
     }
     if(Bytes_Read == 0){
       fprintf(stderr,gettext("Byte offset exceeds size of input file.\n"));
       exit(COMMANDLINEERROR);
     }
   }

   while( (Bytes_Read=read(infd,inbufptr,Item_Size)) == Item_Size){
     Tokens_Processed++;
     switch(Binary_Size){
     case BCHAR:
       if( 2 == Base) fputs(binfmtc(*((unsigned char *)inbufptr)),stdout);
       else if(BSIGNED == Binary_Type) printf(format,*((char *)inbufptr));
       else printf(format,*((unsigned char *)inbufptr));
       if(AddNewlineP && (*inbufptr == 0x0A)) {
	 putchar('\n'); 
	 EmittedLFP = 1;
       }
       break;
     case BSHORT:
       if(2 == Base) fputs(binfmts(*((unsigned short *)inbufptr)),stdout);
       else if(BSIGNED == Binary_Type) printf(format,*((short *)inbufptr));
       else printf(format,*((unsigned short *)inbufptr));
       if(AddNewlineP && (*inbufptr == 0x0A)) {
	 putchar('\n'); 
	 EmittedLFP = 1;
       }
       break;
     case BINT:
       if(2 ==Base) fputs(binfmti(*((unsigned int *)inbufptr)),stdout);
       else if(BSIGNED == Binary_Type) printf(format,*((int *)inbufptr));
       else printf(format,*((unsigned int *)inbufptr));
       if(AddNewlineP && (*inbufptr == 0x0A)) {
	 putchar('\n'); 
	 EmittedLFP = 1;
       }
       break;
     case BLONG:
       if(2 == Base) fputs(binfmtl(*((unsigned long *)inbufptr)),stdout);
       else if(BSIGNED == Binary_Type) printf(format,*((long *)inbufptr));
       else printf(format,*((unsigned long *)inbufptr));
       if(AddNewlineP && (*inbufptr == 0x0A)) {
	 putchar('\n'); 
	 EmittedLFP = 1;
       }
       break;
     case BLLONG:
       if(2 == Base) fputs(binfmtq(*((unsigned long long *)inbufptr)),stdout);
       else if(BSIGNED == Binary_Type) printf(format,*((long long *)inbufptr));
       else printf(format,*((unsigned long long *)inbufptr));
       break;
     case BSINGLE:
       printf(format,*((float *)inbufptr));
       break;
     case BDOUBLE:
       printf(format,*((double *)inbufptr));
       break;
#ifdef HAVE_LONG_DOUBLE
     case BLDOUBLE:
       printf(format,*((long double *)inbufptr));
       break;
#endif
     }
     if(Tokens_Processed % Items_Per_Line == 0) putchar('\n');
     else putchar(' ');
   } /* End of input loop */

   if(Tokens_Processed % Items_Per_Line != 0) putchar('\n');
   if(Verbose_P){
     if(Tokens_Processed == 1) fprintf(stderr,gettext("%s: converted 1 token\n"),pgname);
     else fprintf(stderr,gettext("%s: converted %d tokens\n"),pgname,Tokens_Processed); 
   }
   if(Bytes_Read > 0){
     fprintf(stderr,gettext("Error reading input.\n"));
     exit(SYSTEMERROR);
   }
   exit(SUCCESS);
}

