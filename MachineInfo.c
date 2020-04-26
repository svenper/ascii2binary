/* Time-stamp: <2010-08-29 22:11:25 poser> */
/*
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
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <wchar.h>
#ifdef INTLIZE
#include <libintl.h>
#else
#define gettext(x) (x)
#endif
#include "a2b_endian.h"
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

void
Print_Byte_Order(void)
{
  int EndianNess;

  extern int Get_Endianness(void);

  EndianNess = Get_Endianness();
  switch(EndianNess){
    case E_LITTLE_ENDIAN:
      fprintf(stderr,gettext("This machine is little-endian. (1234 byte order).\n"));
      fprintf(stderr,gettext("That is, the most significant byte is stored in the highest memory address.\n"));
      break;
    case E_BIG_ENDIAN:
      fprintf(stderr,gettext("This machine is big-endian. (4321 byte order).\n"));
      fprintf(stderr,gettext("That is, the most significant byte is stored in the lowest memory address.\n"));
      break;
    case E_PDP_ENDIAN:
      fprintf(stderr,gettext("This machine is PDP-endian and so is probably a PDP-11. (3412 byte order).\n"));
      fprintf(stderr,gettext("In 4-byte integers, the first two bytes are more significant than the second two.\n"));
      fprintf(stderr,gettext("but within each pair, the second byte is the more significant.\n"));
      break;
    default:
      fprintf(stderr,gettext("Unknown byte-ordering\n"));
  }
}

void
Print_Sizes(void)
{
  int width = 24;
  signed long wchartMax = 0L;
  signed long wchartMin = 0L;

  if(sizeof(wchar_t) == sizeof(short)) {
    wchartMax = SHRT_MAX;
    wchartMin = SHRT_MIN;
  }
  else if(sizeof(wchar_t) == sizeof(int)) {
    wchartMax = INT_MAX;
    wchartMin = INT_MIN;
  }
  else if(sizeof(wchar_t) == sizeof(long)) {
    wchartMax = LONG_MAX;
    wchartMin = LONG_MIN;
  }

  fprintf(stderr,gettext("                   Bytes               Minimum                  Maximum\n"));
  fprintf(stderr,"char      signed      %2d %*d %*d\n",sizeof(signed char),width,SCHAR_MIN,width,SCHAR_MAX);  
  fprintf(stderr,"          unsigned    %2d %*d %*d\n",sizeof(unsigned char),width,0,width,UCHAR_MAX);
  fprintf(stderr,"short     signed      %2d %*d %*d\n",sizeof(signed short),width,SHRT_MIN,width,SHRT_MAX);  
  fprintf(stderr,"          unsigned    %2d %*d %*d\n",sizeof(unsigned short),width,0,width,USHRT_MAX); 
  fprintf(stderr,"int       signed      %2d %*d %*d\n",sizeof(signed int),width,INT_MIN,width,INT_MAX); 
  fprintf(stderr,"          unsigned    %2d %*d %*u\n",sizeof(unsigned int),width,0,width,UINT_MAX); 
  fprintf(stderr,"long      signed      %2d %*ld %*ld\n",sizeof(signed long),width,LONG_MIN,width,LONG_MAX);  
  fprintf(stderr,"          unsigned    %2d %*ld %*lu\n",sizeof(unsigned long),width,0,width,ULONG_MAX);
#ifdef HAVE_LONG_LONG
  fprintf(stderr,"long long signed      %2d %*lld %*lld\n",sizeof(signed long long),width,LLONG_MIN,width,LLONG_MAX);
  fprintf(stderr,"          unsigned    %2d %*llu %*llu\n",sizeof(unsigned long long),width,0ULL,width,ULLONG_MAX);
#endif
  fprintf(stderr,"wchar_t               %2d %*ld %*ld\n",sizeof(wchar_t),width,wchartMin,width,wchartMax);  
  fprintf(stderr,"float                 %2d %*g %*g\n",sizeof(float),width,FLT_MIN,width,FLT_MAX); 
  fprintf(stderr,"double                %2d %*g %*g\n",sizeof(double),width,DBL_MIN,width,DBL_MAX);
#ifdef HAVE_LONG_DOUBLE
  fprintf(stderr,"long double           %2d %*Lg  %*Lg\n",sizeof(long double),width,LDBL_MIN,width,LDBL_MAX);
#endif
  fprintf(stderr,"size_t                %2d\n",sizeof(size_t));
#ifdef HAVE_SYS_TYPES_H
  fprintf(stderr,"u_int8_t              %2d\n",sizeof(u_int8_t));
  fprintf(stderr,"u_int16_t             %2d\n",sizeof(u_int16_t));
  fprintf(stderr,"u_int32_t             %2d\n",sizeof(u_int32_t));
  fprintf(stderr,"u_int64_t             %2d\n",sizeof(u_int64_t));
#endif
}
