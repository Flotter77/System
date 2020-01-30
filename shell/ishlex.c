/*--------------------------------------------------------------------*/
/* ishlex.c                                                           */
/* Author: Jinyuan (Chi)  Qi                                          */
/*--------------------------------------------------------------------*/

#include "readline.h"
#include "ishlex.h"
#include "dynarray.h"
#include "lexAnalyzer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*--------------------------------------------------------------------*/
/* The name of the executable binary file. */
const char *pcPgmName;

/*--------------------------------------------------------------------*/
/* Write all tokens in oTokens to stdout. */

static void writeTokens(DynArray_T oTokens)
{
   size_t u;
   size_t uLength;
   Token_T psToken;

   assert(oTokens != NULL);

   uLength = DynArray_getLength(oTokens);
 
   for (u = 0; u < uLength; u++)
   {
      psToken = DynArray_get(oTokens, u);
      if (psToken->eType == TOKEN_SPECIAL)
         printf("Token: %s (special)\n", psToken->pcValue);
      else if (psToken->eType == TOKEN_ORDINARY)
         printf("Token: %s (ordinary)\n", psToken->pcValue);
   }
 
}

/*--------------------------------------------------------------------*/

/* A "spcial" word consists of '<' or '>' stdin-redirect token and 
   the stdout-redirect token respecively, and an "ordinary" word
   consists of nonspecial characters. Special characters inside of 
   strings are not separate tokens.Read a line from stdin.  
   Write the line to stdout. If the line contains an "opening" double 
   quote within a line to be unmatched by a "closing" double quote,
   then write an error message to stderr and reject the line.
    Otherwise write to stdout each number that the line contains
   followed by each word that the line contains.  Repeat until EOF.
   Return 0 iff successful. As always, argc is the command-line
   argument count and argv is an array of command-line arguments. */

int main(int argc, char *argv[])
{
   char *pcLine;
   DynArray_T oTokens;
   int iRet;

   pcPgmName = argv[0];

   printf("%% ");

   while ((pcLine = ishReadLine(stdin)) != NULL)
   {
      printf("%s\n", pcLine);
      iRet = fflush(stdout);
      if (iRet == EOF)
         {perror(pcPgmName); exit(EXIT_FAILURE);}
      oTokens = LexAnalyzer_lexLine(pcLine);
      if (oTokens != NULL)
      {
         writeTokens(oTokens);
         LexAnalyzer_freeTokens(oTokens);
         DynArray_free(oTokens);
      }

      free(pcLine);
      printf("%% ");
   }
   printf("\n");
   return 0;
}
