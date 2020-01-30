/*--------------------------------------------------------------------*/
/* readline.h                                                         */
/* Author: Jinyuan (Chi) Qi                                           */
/*--------------------------------------------------------------------*/
#ifndef READLINE_INCLUDED
#define READLINE_INCLUDED

#include <stdio.h>

/*--------------------------------------------------------------------*/
/* The name of the executable binary file. */
const char *pcPgmName;



/*--------------------------------------------------------------------*/
/* If no lines remain in psFile, then return NULL. Otherwise read a line
   of psFile and return it as a string. The string does not contain a
    terminating newline character. The caller owns the string. */

char *ishReadLine(FILE *psFile);

#endif
