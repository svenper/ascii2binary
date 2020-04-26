/* Time-stamp: <2009-12-25 21:20:47 poser> */
/*
 * Copyright (C) 2003, 2009 William J. Poser (billposer@alum.mit.edu)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 3 of the  GNU Lesser General Public License
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * or go to the web page:  http://www.gnu.org/copyleft/lesser.html
 */

/*
 * These functions format unsigned integers into base-2 text.
 * They return a pointer to static storage containing the resulting
 * null terminated string.  There are five otherwise identical functions
 * for char,short,int, long, and long long operands. They are
 * written in such a way as to adapt at compile time to the
 * operand sizes used on a particular machine.
 *
 * They assume that a byte consists of eight bits. In the unlikely
 * event that this is not true of your system, define CHAR_BITS to the
 * appropriate value.
 */

#ifndef CHAR_BITS
#define CHAR_BITS 8
#endif

#define CBITS (sizeof(unsigned char) * CHAR_BITS)
#define SBITS (sizeof(unsigned short) * CHAR_BITS)
#define IBITS (sizeof(unsigned int) * CHAR_BITS)
#define LBITS (sizeof(unsigned long) * CHAR_BITS)
#define QBITS (sizeof(unsigned long long) * CHAR_BITS)

#define MASK 0x01

char *
binfmtc(unsigned char n)
{
  int i;
  static char str [CBITS+1];

  for (i = CBITS-1; i >= 0;i--){
    str[i] = (n & MASK ? '1' : '0');
    n >>= 1;
  }

  str[CBITS] = '\0';
  return(str);
}

char *
binfmts(unsigned short n)
{
  int i;
  static char str [SBITS+1];

  for (i = SBITS-1; i >= 0;i--){
    str[i] = (n & MASK ? '1' : '0');
    n >>= 1;
  }

  str[SBITS] = '\0';
  return(str);
}

char *
binfmti(unsigned int n)
{
  int i;
  static char str [IBITS+1];

  for (i = IBITS-1; i >= 0;i--){
    str[i] = (n & MASK ? '1' : '0');
    n >>= 1;
  }

  str[IBITS] = '\0';
  return(str);
}

char *
binfmtl(unsigned long n)
{
  int i;
  static char str [LBITS+1];

  for (i = LBITS-1; i >= 0;i--){
    str[i] = (n & MASK ? '1' : '0');
    n >>= 1;
  }

  str[LBITS] = '\0';
  return(str);
}

char *
binfmtq(unsigned long long n)
{
  int i;
  static char str [QBITS+1];

  for (i = QBITS-1; i >= 0;i--){
    str[i] = (n & MASK ? '1' : '0');
    n >>= 1;
  }

  str[QBITS] = '\0';
  return(str);
}
