/*--------------------------------------------------------------------*/
/* command.c                                                          */
/* Author: Jinyuan (Chi) Qi                                           */
/*--------------------------------------------------------------------*/
#include "ish.h"
#include "command.h"
#include "ishsyn.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*--------------------------------------------------------------------*/
/* The name of the executable binary file. */
const char *pcPgmName;

/*--------------------------------------------------------------------*/
Command_T  Command_new(char* pcName, DynArray_T oArgs,
                       char* pcStdin, char* pcStdout)
{
   Command_T oCommand;
   assert(pcName != NULL);

   /* Store pcName in oCommand */
   oCommand = (struct Command*)malloc(sizeof(struct Command));
   if (oCommand == NULL)
   {perror(pcPgmName); exit(EXIT_FAILURE);}
   oCommand->pcName = (char*)malloc(strlen(pcName)+1);
   if (oCommand->pcName == NULL)
   {perror(pcPgmName); exit(EXIT_FAILURE);}
 
   strcpy(oCommand->pcName, pcName);

   /* Store oArgs if there is any argument */
   if (oArgs != NULL)
   {
      size_t u;
      size_t uLength = DynArray_getLength(oArgs);
      oCommand->oArgs = DynArray_new(uLength);
      for (u = 0; u < uLength; u++)
      {
         char* oArg = DynArray_get(oArgs, u);
         assert(oArg != NULL);
         (void) DynArray_set(oCommand->oArgs, u, oArg);
      }
   }else oCommand->oArgs = NULL;

   /* Store stdin filename if there's stdin*/
   if (pcStdin != NULL)
   {
      oCommand->pcStdin =
         (char*)malloc(strlen(pcStdin)+1);
      if(oCommand->pcStdin == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
      
      strcpy(oCommand->pcStdin, pcStdin); 
   } else oCommand->pcStdin = NULL;

   /* Store stdout filename if there's stdout*/
   if (pcStdout != NULL)
   {
      oCommand->pcStdout =
         (char*)malloc(strlen(pcStdout)+1);
      if(oCommand->pcStdout == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
      
      strcpy(oCommand->pcStdout, pcStdout); 
   } else oCommand->pcStdout = NULL;

   return oCommand;
}

/*--------------------------------------------------------------------*/
void Command_free(Command_T oCommand)
{
   assert(oCommand != NULL);
   assert(oCommand->pcName != NULL);
   free(oCommand->pcName);
   if(oCommand->oArgs != NULL)
      DynArray_free(oCommand->oArgs);
   if(oCommand->pcStdin != NULL)
      free(oCommand->pcStdin);
   if(oCommand->pcStdout != NULL)
      free(oCommand->pcStdout);
   free(oCommand);   
}

/*--------------------------------------------------------------------*/
void Command_print(Command_T oCommand)
{
   assert(oCommand != NULL);
   assert(oCommand->pcName != NULL);

   /*Print the command name*/
   printf("Command name: %s\n", oCommand->pcName); 
   /*Print all args*/
   if (oCommand->oArgs != NULL)
   {
      size_t u;
      size_t uLength = DynArray_getLength(oCommand->oArgs);
      for (u = 0; u < uLength; u++)
      {
         char* oArg = DynArray_get(oCommand->oArgs, u);
         assert(oArg != NULL);
         printf("Command arg: %s\n", oArg);
      }   
   }
   if (oCommand->pcStdin != NULL)
      printf("Command stdin: %s\n", oCommand->pcStdin);
   if (oCommand->pcStdout != NULL)
      printf("Command stdout: %s\n", oCommand->pcStdout);
}


/*--------------------------------------------------------------------*/
char** Command_getArgs(Command_T oCommand)
{

   char** argv = NULL;
   size_t u = 0;
   size_t uLength = 0;

   assert(oCommand != NULL);

   if(oCommand->oArgs == NULL)
      uLength = 0;
   else uLength = DynArray_getLength(oCommand->oArgs);
   argv = (char**)calloc(uLength+2, sizeof(char*));
   if (argv == NULL)
       {perror(pcPgmName); exit(EXIT_FAILURE);}
   argv[0] = oCommand->pcName;
   for (u = 1; u < (uLength + 1); u++)
   {
      argv[u] = DynArray_get(oCommand->oArgs, u-1);
   }
   argv[u] = NULL;
   return argv;
}

   





