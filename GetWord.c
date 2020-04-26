/* Time-stamp: <2008-03-14 19:56:20 poser> */
/*
 * Copyright (C) 2008 William J. Poser (billposer@alum.mit.edu)
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

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/* 
 * Read a chunk of arbitrary length from a file, terminated
 * by whitespace.
 *
 * Return a pointer to the null-terminated string allocated, or null on failure
 * to allocate sufficient storage.
 * The length of the word is placed in the variable pointed to by
 * the second argument.
 *
 * It is the responsibility of the caller to free the space allocated.
 */

#define INITLENGTH 16


char * Get_Word(FILE *fp, char *delim, int *WordLength)
{
  int c;
  int Available;
  int Chars_Read;
  char *Word;
  char Delimiter;

  Delimiter = *delim;
  Available = INITLENGTH;
  Chars_Read=0;
  Word = (char *) malloc((size_t)Available);
  if(Word == (char *) 0) return (Word);
  while(1){
    c=getc(fp);
    if(c == EOF){
      Word[Chars_Read]='\0';
      *WordLength=Chars_Read;
      return(Word);
    }
    /* For mysterious reasons isspace is sometimes a problem */
    if((0x20==c)||(0x09==c)||(0x0A==c)||(0x0D==c)){
      Word[Chars_Read]='\0';
      *WordLength=Chars_Read;
      return(Word);
    }
    if(c == Delimiter) continue;
    if(Chars_Read == (Available-1)){ /* -1 because of null */
      Available += INITLENGTH/2;
      Word = (char *) realloc( (void *) Word, (size_t) (Available * sizeof (char)));
      if(Word == (char *) 0) return(Word);
    }
    Word[Chars_Read++]= (char) (c & 0x7F);
  }
}
