/*--------------------------------------------------------------------*/
/* synAnalyzer.c                                                      */
/* Author: Jinyuan (Chi) Qi                                           */
/*--------------------------------------------------------------------*/
#include "ishsyn.h"
#include "lexAnalyzer.h"
#include "synAnalyzer.h"
#include "dynarray.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*--------------------------------------------------------------------*/
/* pcPgmName is the  name of the executable binary file. */
const char *pcPgmName;


/*--------------------------------------------------------------------*/
/* Accepts a DynArray object oTokens containing tokens, and anaylzes 
   whether the tokens have synthetic error. If yes, then quit and 
  return NULL. Otherwise, returns a Command object. */
Command_T SynAnalyzer_syn(DynArray_T oTokens)
{
   size_t  u = 0; /*The iterator*/
   size_t  uLength = 0; /*Length of oTokens*/
   size_t  uStdin = 0;  /*The number of stdin-redirection sign '<'*/
   size_t  uStdout = 0; /*The number of stdout-redirection sign '>'*/
   char* pcCommandName = NULL;
   char* pcCommandStdin = NULL; /* stdin filename*/
   char* pcCommandStdout = NULL; /* stdout filename*/
   Token_T psToken = NULL;
   DynArray_T oCommandArgs = NULL;
   Command_T oCommand = NULL;  /*Command to be returned*/
      
   assert(oTokens != NULL);
   
   uLength = DynArray_getLength(oTokens);
   if(uLength == 0) return NULL;

   /*Report error if tokens don't begin with an ordinary token, 
     which is the command's name. */
   psToken = DynArray_get(oTokens, 0);
   assert(psToken != NULL);
   if (psToken->eType != TOKEN_ORDINARY)
   {
      fprintf(stderr, "%s: missing command name\n",
               pcPgmName);
      return NULL;
   }
   pcCommandName = psToken->pcValue;

   /*Handle redirection - quit and report error if 
     the redirection token is not followed by an ordinary token,
     which is the name of file the sign (either '<'or'<') redirected.
     It's also an error for a sequence of tokens to contain multiple
     stdin-redirect tokens, or multiple stdout-redirect tokens.*/

   for(u = 1; u < uLength; u++)
   {
      psToken = DynArray_get(oTokens, u); 
      assert(psToken != NULL);
       /* Handle when the token is special*/
      if(psToken->eType == TOKEN_SPECIAL)
      {
         /*Stdin-redirect*/
         if(strcmp(psToken->pcValue, "<") == 0)
         {
            uStdin++;
            /*Check wether the following token is ordinary*/
            if (u == (uLength-1))
            {
               fprintf(stderr,
                   "%s: standard input redirection without file name\n",
                       pcPgmName);   
               if (oCommandArgs != NULL)
                  DynArray_free(oCommandArgs);
               return NULL;         
            }
            psToken = DynArray_get(oTokens, ++u);
            assert(psToken != NULL);
            /*Save the filename for stdin */
            if (psToken->eType == TOKEN_ORDINARY)
            {pcCommandStdin = psToken->pcValue;}
            else if (psToken->eType == TOKEN_SPECIAL)
            {uStdin++;}
            
            /*Check multiple stdin-redict tokens */
            if(uStdin > 1)
            {
               fprintf(stderr,
                      "%s: multiple redirection of standard input\n",
                       pcPgmName);
               if (oCommandArgs != NULL)
                  DynArray_free(oCommandArgs);
               return NULL;           
            }   
         }
         /*Stdout-redirect*/
         else if(strcmp(psToken->pcValue, ">") == 0)
        {
            uStdout++;
            /*Check wether the following token is ordinary*/
            if (u == (uLength-1))
            {
               fprintf(stderr,
                  "%s: standard output redirection without file name\n",
                       pcPgmName);   
               if (oCommandArgs != NULL)
                  DynArray_free(oCommandArgs);
               return NULL;         
            }
            psToken = DynArray_get(oTokens, ++u);
            assert(psToken != NULL);
            /*Save the filename for stdout */
            if (psToken->eType == TOKEN_ORDINARY)
            {pcCommandStdout = psToken->pcValue;}
            else if (psToken->eType == TOKEN_SPECIAL)
            {uStdout++;}
            
            /*Check multiple stdout-redict tokens */
            if(uStdout > 1)
            {
               fprintf(stderr,
                      "%s: multiple redirection of standard output\n",
                       pcPgmName);
               if (oCommandArgs != NULL)
                  DynArray_free(oCommandArgs);
               return NULL;           
            }  
         }
         /*Handle when the token is ordinary*/
      } else if(psToken->eType == TOKEN_ORDINARY)
       {
            if(oCommandArgs == NULL)
            {
            oCommandArgs = DynArray_new(0);
              if (oCommandArgs == NULL)
              {perror(pcPgmName); exit(EXIT_FAILURE);}
            }
            
         /* Add new arg to the array */
         DynArray_add(oCommandArgs, psToken->pcValue);       
       }
   }
   /*Create a Command object*/
   oCommand = Command_new(pcCommandName, oCommandArgs,
                          pcCommandStdin, pcCommandStdout);
   if(oCommand == NULL)
     {perror(pcPgmName); exit(EXIT_FAILURE);}     
   if (oCommandArgs != NULL)
    {DynArray_free(oCommandArgs);}

   return oCommand;      
}


   
   

   
