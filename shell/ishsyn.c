/*--------------------------------------------------------------------*/
/* ishsyn.c                                                           */
/* Author: Jinyuan (Chi)  Qi                                          */
/*--------------------------------------------------------------------*/
#include "readline.h"
#include "ishsyn.h"
#include "command.h"
#include "dynarray.h"
#include "lexAnalyzer.h"
#include "synAnalyzer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*--------------------------------------------------------------------*/
/* The name of the executable binary file. */
const char *pcPgmName;


/*--------------------------------------------------------------------*/
/* Read a Line fro stdin. Write the line to stdout. If the Line  
   doesn't have lexical error after lexical analyis, then will be
   passed into synthecial analysis. If there is any  error(e.g., doesn't
   begin with ordinary token, redirect token is not followed by an 
   ordinary token, multiple stdin-redirct tokens, multiple 
   stdout-redirect tokens), it will write an error message and reject
   the line. Otherwise write to stadout each command with name, args,
   stdin, and stdout filename. Repeat until EOF.
   Return 0 iff sucessful. As always, argc is the command-line 
   argument coutn and argv is an array of command-line arguments. */
int main(int argc, char *argv[])
{
   char *pcLine;
   DynArray_T oTokens;
   int iRet;
   Command_T oCommand;

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
         {oCommand = SynAnalyzer_syn(oTokens);}
      else {oCommand = NULL;}
      
      if(oCommand != NULL)
      {
         Command_print(oCommand);
         Command_free(oCommand);
      }
      if(oTokens != NULL)
      {
         LexAnalyzer_freeTokens(oTokens);
         DynArray_free(oTokens);
      }

      free(pcLine);
      printf("%% ");
   }
   printf("\n");
   return 0;
      
}
