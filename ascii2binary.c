/* Time-stamp: <2008-10-06 17:21:10 poser>
 *
 * This program reads a file containing ASCII textual representations of numbers
 * separated by whitespace and produces as output the binary equivalents.
 * The type and precision of the binary output is selected using command line flags.
 *
 * Copyright (C) 2004-2008 William J. Poser (billposer@alum.mit.edu)
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
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#ifdef HAVE_GETOPT_LONG
#define _GNU_SOURCE
#include <getopt.h>
#endif
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#include <langinfo.h>
#ifdef HAVE_LIBINTL_H
#include <libintl.h>
#else
#define gettext(x) (x)
#endif
#include "exitcodes.h"

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

char pgname[]="ascii2binary";
char version[]=PACKAGE_VERSION;
char compdate[]=__DATE__ " " __TIME__ ;

void
Usage(void){
  fprintf(stderr,gettext("Filter to convert textual representations of numbers to binary.\n"));
  fprintf(stderr,gettext("Input consists of whitespace-delimited representations of numbers.\n"));
  fprintf(stderr,gettext("The formats supported are those used by strtod(3), strtoll(3) and strtoull(3).\n"));
  fprintf(stderr,gettext("Usage: %s [flags]\n"),pgname);
  fprintf(stderr,gettext("Flags:\n"));
  fprintf(stderr,gettext("\t-h          print this help message\n"));
  fprintf(stderr,gettext("\t-L <locale> set the locale\n"));
  fprintf(stderr,gettext("\t-s          print sizes of types on this machine\n"));
  fprintf(stderr,gettext("\t-V          be verbose\n"));
  fprintf(stderr,gettext("\t-v          identify program version\n"));
  fprintf(stderr,gettext("\t-X          print explanation of exit codes\n"));
  fprintf(stderr,gettext("\t-b <base>   set base in range [2,36] for integer conversions\n"));
  fprintf(stderr,gettext("\t              The base may be given as a number or as:\n"));
  fprintf(stderr,"\t                b(inary)\n");
  fprintf(stderr,"\t                d(ecimal)\n");
  fprintf(stderr,"\t                h(exadecimal)\n");
  fprintf(stderr,"\t                o(ctal)\n");
  fprintf(stderr,gettext("\t-t <type>  specify type of output. The default is uc.\n"));
  fprintf(stderr,"\t           \td  [double]\n");
  fprintf(stderr,"\t           \tf  [float]\n");
#ifdef HAVE_LONG_DOUBLE
  fprintf(stderr,"\t           \tq  [long double]\n");
#endif
  fprintf(stderr,"\t           \tsc [signed char]\n");
  fprintf(stderr,"\t           \tss [signed short]\n");
  fprintf(stderr,"\t           \tsi [signed int]\n");
  fprintf(stderr,"\t           \tsl [signed long]\n");
  fprintf(stderr,"\t           \tsq [signed long long]\n");
  fprintf(stderr,"\t           \tuc [unsigned char]\n");
  fprintf(stderr,"\t           \tus [unsigned short]\n");
  fprintf(stderr,"\t           \tui [unsigned int]\n");
  fprintf(stderr,"\t           \tul [unsigned long]\n");
  fprintf(stderr,"\t           \tuq [unsigned long long]\n");
}

void
Version(void){
  fprintf(stderr,"%s %s \n",pgname,version);
  fprintf(stderr,gettext("Compiled %s.\n"),compdate);
  fprintf(stderr,"Copyright 2004-2008 William J. Poser\n");
  fprintf(stderr,"This program is free software; you can redistribute\n\
it and/or modify it under the terms of version 3 of\n\
the GNU General Public License as published by the\n\
Free Software Foundation.\n");
  fprintf(stderr,gettext("Report bugs to: billposer@alum.mit.edu.\n"));
}

#ifdef HAVE_LONG_DOUBLE
extern long double strtold (const char *, char **);
#endif

struct option opts[]={
  "base",1,NULL,'b',
  "help",0,NULL,'h',
  "locale",1,NULL,'L',
  "sizes",0,NULL,'s',
  "type",1,NULL,'t',
  "verbose",0,NULL,'V',
    "version",0,NULL,'v',
  "explain-exit-codes",0,NULL,'X',
    0,0,0,0
};

int
main(int ac, char **av)
{
   int Flag;
   int Base = 0;		/* Base for integer conversions */
   char *str;
   int Word_Length;
   float sfval;
   double dfval;
#ifdef HAVE_LONG_DOUBLE
   long double ldfval;
#endif
   unsigned char ucval;
   unsigned short usval;
   unsigned int uival;
   unsigned long ulval;
   unsigned long long uqval;
   char scval;
   short ssval;
   int sival;
   long slval;
   long long sqval;
   char *endptr;
   int towrite;
   int nwritten;
   unsigned long Tokens_Processed;
   int Verbose_P = 0;
   int lgoindex;			/* Unused but needed by getopt_long */
   char DefaultDelimiter = ',';
   char *Delimiter = &DefaultDelimiter;

   enum bt  {BSIGNED, BUNSIGNED, BFLOAT} Binary_Type;
#ifdef HAVE_LONG_DOUBLE
   enum bs  {BCHAR, BSHORT, BINT, BLONG, BLLONG, BSINGLE, BDOUBLE,BLDOUBLE} Binary_Size;
#else
   enum bs  {BCHAR, BSHORT, BINT, BLONG, BLLONG, BSINGLE, BDOUBLE} Binary_Size;
#endif

   extern int optopt;
   extern char *optarg;
   extern char *Get_Word(FILE *,char *, int *);
   extern void Print_Byte_Order(void);
   extern void Print_Sizes(void);
   extern void Print_ExitCodes(void);

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

   /* Check command line arguments */
   
   opterr = 0;			/* We'll handle errors ourselves */
  while ((Flag = getopt_long(ac,av,":b:hL:st:VvX",&opts[0],&lgoindex)) != EOF){
    switch(Flag)
       {
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
	   } /* End of switch on base argument*/
	 break;
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
       Verbose_P = 1;
       break;
     case 'v':
       Version();
       exit(INFO);
     case 'X':
       Print_ExitCodes();
       exit(INFO);
     case 'h':
       Usage();
       exit(INFO);
     case ':':
       fprintf(stderr,gettext("%s: missing argument to option flag %c.\n"),pgname,optopt);
       exit(COMMANDLINEERROR);
     default:
       fprintf(stderr,gettext("%s: invalid option flag %c\n"),pgname,optopt);
       exit(COMMANDLINEERROR);
     }
   }

   /* Sanity checks */

   if( (Base < 0) || (Base == 1) || (Base > 36)){
     fprintf(stderr,gettext("%s: a base of %d is not acceptable.\n"),pgname,Base);
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

#ifdef HAVE_LANGINFO_H
   Delimiter = nl_langinfo(THOUSEP);
#endif
   while(1){
     str = Get_Word(stdin,Delimiter,&Word_Length);
     if(Word_Length == 0) break;
     Tokens_Processed++;
     if(str == NULL){
       fprintf(stderr,gettext("%s: failed to allocate storage for input token %ld.\n"),
	       pgname,Tokens_Processed);
       exit(SYSTEMERROR);
     }
     switch(Binary_Type){
       case BFLOAT:
	 errno=0;
#ifdef HAVE_LONG_DOUBLE
	 if (Binary_Size == BLDOUBLE) {
	   ldfval = strtold(str,&endptr);
	 }
	 else dfval = strtod(str,&endptr);
#else
	 dfval = strtod(str,&endptr);
#endif 
	 if(errno == ERANGE){
	   fprintf(stderr,gettext("%s: value %s at token %ld out of representable range\n"),
		   pgname,str,Tokens_Processed);
	   exit(RANGEERROR);
	 }
	 if(*endptr != '\0'){
	   fprintf(stderr,gettext("%s: ill-formed input %s at token %ld\n"),
		   pgname,str,Tokens_Processed);
	   exit(INPUTERROR);
	 }
          switch(Binary_Size){	/* FLOAT */
             case BSINGLE:
	       sfval = (float) dfval;
	       towrite = sizeof(float);
	       nwritten = write(fileno(stdout),&sfval,towrite);
	       if(nwritten != towrite){
		 fprintf(stderr,gettext("%s: error writing output\n"),pgname);
		 exit(SYSTEMERROR);
	       }
	       break;
             case BDOUBLE:
	       towrite = sizeof(double);
	       nwritten = write(fileno(stdout),&dfval,towrite);
	       if(nwritten != towrite){
		 fprintf(stderr,gettext("%s: error writing output\n"),pgname);
		 exit(SYSTEMERROR);
	       }
	       break;
#ifdef HAVE_LONG_DOUBLE
             case BLDOUBLE:	
	       towrite = sizeof(long double);
	       nwritten = write(fileno(stdout),&ldfval,towrite);
	       if(nwritten != towrite){
		 fprintf(stderr,gettext("%s: error writing output\n"),pgname);
		 exit(SYSTEMERROR);
	       }
	       break;
#endif
	  }
	  break;
        case BUNSIGNED:
	  errno=0;
	  uqval = strtoull(str,&endptr,Base);
	  if(errno == ERANGE){
	    fprintf(stderr,gettext("%s: value %s at token %ld out of representable range\n"),
		    pgname,str,Tokens_Processed);
	    exit(RANGEERROR);
	  }
	  if(errno == EINVAL){
	    fprintf(stderr,gettext("%s: value %s at token %ld ill-formed\n"),
		    pgname,str,Tokens_Processed);

	    exit(INPUTERROR);
	  }
	  if(*endptr != '\0'){
	    fprintf(stderr,gettext("%s: ill-formed input %s at token %ld\n"),
		    pgname,str,Tokens_Processed);
	    exit(INPUTERROR);
	  }
          switch(Binary_Size){	/* UNSIGNED INT */
             case BCHAR:
	       if(uqval > UCHAR_MAX){
		 fprintf(stderr,gettext("%s: value of %llu at token %d too large for binary type\n"),
			 pgname,uqval,Tokens_Processed);
		 exit(RANGEERROR);
	       }
	       ucval = (unsigned char) uqval;
	       towrite = sizeof(unsigned char);
	       nwritten = write(fileno(stdout),&ucval,towrite);
	       if(nwritten != towrite){
		 fprintf(stderr,gettext("%s: error writing output\n"),pgname);
		 exit(SYSTEMERROR);
	       }
	       break;
             case BSHORT:
	       if(uqval > USHRT_MAX){
		 fprintf(stderr,gettext("%s: value of %llu at token %d too large for binary type\n"),
			 pgname,uqval,Tokens_Processed);
		 exit(RANGEERROR);
	       }
	       usval = (unsigned short) uqval;
	       towrite = sizeof(unsigned short);
	       nwritten = write(fileno(stdout),&usval,towrite);
	       if(nwritten != towrite){
		 fprintf(stderr,gettext("%s: error writing output\n"),pgname);
		 exit(SYSTEMERROR);
	       }
	       break;
             case BINT:
	       if(uqval > UINT_MAX){
		 fprintf(stderr,gettext("%s: value of %llu at token %d too large for binary type\n"),
			 pgname,uqval,Tokens_Processed);
		 exit(RANGEERROR);
	       }
	       uival = (unsigned int) uqval;
	       towrite = sizeof(unsigned int);
	       nwritten = write(fileno(stdout),&uival,towrite);
	       if(nwritten != towrite){
		 fprintf(stderr,gettext("%s: error writing output\n"),pgname);
		 exit(SYSTEMERROR);
	       }
	       break;
	     case BLONG:
	       if(uqval > ULONG_MAX){
		 fprintf(stderr,gettext("%s: value of %llu at token %d too large for binary type\n"),
			 pgname,uqval,Tokens_Processed);
		 exit(RANGEERROR);
	       }
	       ulval = (signed int) uqval;
	       towrite = sizeof(signed long);
	       nwritten = write(fileno(stdout),&ulval,towrite);
	       if(nwritten != towrite){
		 fprintf(stderr,gettext("%s: error writing output\n"),pgname);
		 exit(SYSTEMERROR);
	       }
	       break;
	     case BLLONG:
	       towrite = sizeof(signed long long);
	       nwritten = write(fileno(stdout),&uqval,towrite);
	       if(nwritten != towrite){
		 fprintf(stderr,gettext("%s: error writing output\n"),pgname);
		 exit(SYSTEMERROR);
	       }
	       break;
	  }
	  break;
        case BSIGNED:
	  errno=0;
	  sqval = strtoll(str,&endptr,Base);
	  if(errno == ERANGE){
	    fprintf(stderr,gettext("%s: value %s at token %ld out of representable range\n"),
		    pgname,str,Tokens_Processed);
	    exit(RANGEERROR);
	  }
	  if(errno == EINVAL){
	    fprintf(stderr,gettext("%s: value %s at token %ld ill-formed\n"),
		    pgname,str,Tokens_Processed);
	    exit(INPUTERROR);
	  }
	  if(*endptr != '\0'){
	    fprintf(stderr,gettext("%s: ill-formed input %s at token %ld\n"),pgname,str,Tokens_Processed);
	    exit(INPUTERROR);
	  }
          switch(Binary_Size){	/* SIGNED INT */
             case BCHAR:
	       if(sqval > SCHAR_MAX){
		 fprintf(stderr,gettext("%s: value of %lld at token %d too large for binary type\n"),
			 pgname,sqval,Tokens_Processed);
		 exit(RANGEERROR);
	       }
	       if(sqval < SCHAR_MIN){
		 fprintf(stderr,gettext("%s: value of %lld at token %d too small for binary type\n"),
			 pgname,sqval,Tokens_Processed);
		 exit(RANGEERROR);
	       }
	       scval = (signed char) sqval;
	       towrite = sizeof(signed char);
	       nwritten = write(fileno(stdout),&scval,towrite);
	       if(nwritten != towrite){
		 fprintf(stderr,gettext("%s: error writing output\n"),pgname);
		 exit(SYSTEMERROR);
	       }
	       break;
             case BSHORT:
	       if(sqval < SHRT_MIN){
		 fprintf(stderr,gettext("%s: value of %lld at token %d too small for binary type\n"),
			 pgname,sqval,Tokens_Processed);
		 exit(RANGEERROR);
	       }
	       if(sqval > SHRT_MAX){
		 fprintf(stderr,gettext("%s: value of %lld at token %d too large for binary type\n"),
			 pgname,sqval,Tokens_Processed);
		 exit(RANGEERROR);
	       }
	       ssval = (signed short) sqval;
	       towrite = sizeof(signed short);
	       nwritten = write(fileno(stdout),&ssval,towrite);
	       if(nwritten != towrite){
		 fprintf(stderr,gettext("%s: error writing output\n"),pgname);
		 exit(SYSTEMERROR);
	       }
	       break;
             case BINT:
	       if(sqval < INT_MIN){
		 fprintf(stderr,gettext("%s: value of %lld at token %d too small for binary type\n"),
			 pgname,sqval,Tokens_Processed);
		 exit(RANGEERROR);
	       }
	       if(sqval > INT_MAX){
		 fprintf(stderr,gettext("%s: value of %lld at token %d too large for binary type\n"),
			 pgname,sqval,Tokens_Processed);
		 exit(RANGEERROR);
	       }
	       sival = (signed int) sqval;
	       towrite = sizeof(signed int);
	       nwritten = write(fileno(stdout),&sival,towrite);
	       if(nwritten != towrite){
		 fprintf(stderr,gettext("%s: error writing output\n"),pgname);
		 exit(SYSTEMERROR);
	       }
	       break;
	     case BLONG:
	       if(sqval < LONG_MIN){
		 fprintf(stderr,gettext("%s: value of %lld at token %d too small for binary type\n"),
			 pgname,sqval,Tokens_Processed);
		 exit(RANGEERROR);
	       }
	       if(sqval > LONG_MAX){
		 fprintf(stderr,gettext("%s: value of %lld at token %d too large for binary type\n"),
			 pgname,sqval,Tokens_Processed);
		 exit(RANGEERROR);
	       }
	       slval = (signed int) sqval;
	       towrite = sizeof(signed long);
	       nwritten = write(fileno(stdout),&slval,towrite);
	       if(nwritten != towrite){
		 fprintf(stderr,gettext("%s: error writing output\n"),pgname);
		 exit(SYSTEMERROR);
	       }
	       break;
	     case BLLONG:
	       towrite = sizeof(signed long long);
	       nwritten = write(fileno(stdout),&sqval,towrite);
	       if(nwritten != towrite){
		 fprintf(stderr,gettext("%s: error writing output\n"),pgname);
		 exit(SYSTEMERROR);
	       }
	       break;
	  }
     }
     free(str);
   }
   if(Verbose_P){
     if(Tokens_Processed == 1) fprintf(stderr,gettext("%s: converted 1 token\n"),pgname);
     else fprintf(stderr,gettext("%s: converted %d tokens\n"),pgname,Tokens_Processed);
   }
   exit(SUCCESS);
}

