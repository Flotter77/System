/*--------------------------------------------------------------------*/
/* ishsyn.c                                                           */
/* Author: Jinyuan (Chi)  Qi                                          */
/*--------------------------------------------------------------------*/
#define _GNU_SOURCE

#include "readline.h"
#include "command.h"
#include "ish.h"
#include "dynarray.h"
#include "lexAnalyzer.h"
#include "synAnalyzer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

/*--------------------------------------------------------------------*/
/* The name of the executable binary file. */
const char *pcPgmName;

/*--------------------------------------------------------------------*/
/* This is a handler for Signals of type SIGALRM. 
   Change to default ignore signals when time-out */
static void myAlarmHandler(int iSignal)
{
   void (*pfRet) (int);
   pfRet =  signal(SIGINT, SIG_IGN);
   if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}
   
}


/*--------------------------------------------------------------------*/
/* This is a handler for Signals of type SIGINT. 
   Will quit if another SIGINT is entered within 5 seconds.
   otherwise remain ignoring SIGINT signals */

static void myHandler(int iSignal)
{
    enum {ALARM_DURATION_SECONDS = 3};
    void (*pfRet) (int);
    pfRet = signal(SIGALRM, myAlarmHandler);
    if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}
    alarm(ALARM_DURATION_SECONDS);
    pfRet = signal(SIGINT, SIG_DFL);
    if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}
    
}



/*--------------------------------------------------------------------*/
/* This function handles three shell built-in commands - 
   setenv var, unsetenv, and cd.
   It reads a string of pcName, and a pointer to strings- argv, 
   and a size_t iArg, which are processed from oCommand,
   and execute the built-in command.*/

static void handleBuiltInCommands(char* pcName, char** argv,
                                  size_t iArg)
{
   void (*pfRet) (int);
   /* Handle setenv - set the value of var to value, 
     or to the empty string if value is omitted.*/
   if (strcmp(pcName, "setenv") == 0)
   {
   /* Parent process ignores SIGINT signals when handling*/
      pfRet = signal(SIGINT, SIG_IGN);
      if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}
     
      if (iArg == 0)
         fprintf(stderr, "%s: missing variable\n", pcPgmName);
      else if (iArg == 1)
      { setenv(argv[1], "", 1);}
      else if (iArg == 2)
      { setenv(argv[1], argv[2],1);}
      else {fprintf(stderr, "%s: too many arguments\n", pcPgmName);}

   /* Restore the default behavior for SIGINT */    
      pfRet = signal(SIGINT, myHandler);      
      if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}
      
   }
   /* Handle unsetenv - destroy the environment var */
   else if (strcmp(pcName, "unsetenv") == 0)
   {
      pfRet = signal(SIGINT, SIG_IGN);
      if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}
     
      if (iArg == 1)
         unsetenv(argv[1]);
      else if (iArg < 1)
         fprintf(stderr,
                "%s: missing variable\n", pcPgmName);
      else fprintf(stderr, "%s: too many arguments\n", pcPgmName);

      pfRet = signal(SIGINT, myHandler);
   }
   /* Handle cd [dir] - change its working directory to dir, 
      or to the HOME directory if dir is omitted.*/
   else if (strcmp(pcName, "cd") == 0)
   {
      pfRet = signal(SIGINT, SIG_IGN);
      if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}
     
      if (iArg == 0)
      {
         if (getenv("HOME") == NULL)
            fprintf(stderr, "%s: HOME is not set\n", pcPgmName);
         else chdir(getenv("HOME"));
      }
      else if (iArg == 1)
         chdir(argv[1]);
      else fprintf(stderr, "%s: too many arguments\n", pcPgmName);

      pfRet = signal(SIGINT, myHandler);
   }  
}

/*--------------------------------------------------------------------*/
/* This function handles redirection. It reads a command object with 
   potential names of stdin and stdout file, and redirects to a file.
   If stdout is redirected to a file that does not exit, a new file
   will be created; if the stdout is redirected to a file that already
   eixts, then this file's contents will be destroyed and rewrited.*/

static void handleRedirection(Command_T oCommand)
{
   /* The Unix permissions that a newly created file should have. */
   enum {PERMISSIONS = 0600};
   int iRet;
   int iFd;

   assert(oCommand != NULL);
   /* Handle stdin-redirection */
   if (oCommand->pcStdin != NULL)
   {
      iFd = open(oCommand->pcStdin, O_RDONLY);
      if (iFd == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

      iRet = close(0);
      if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

      iRet = dup(iFd);
      if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

      iRet = close(iFd);
      if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}
   }
   
   /* Handle stdout-redirection */
   if (oCommand->pcStdout != NULL)
   {
      iFd = creat(oCommand->pcStdout, PERMISSIONS);
      if (iFd == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

      iRet = close(1);
      if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

      iRet = dup(iFd);
      if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

      iRet = close(iFd);
      if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}
   }
}


/*--------------------------------------------------------------------*/
/* This function reads a command object and dynamic array object that
   creates the command. If first gets the command name pcName, and
   handles "exit" command, and then gets argv to execute and the number
   of args in the command. Then handles other built-in commnds, create
   a child process that handles external commands, and redirection, 
   and then execute the external commands in child process. */

static void ishExec(Command_T oCommand, DynArray_T oTokens)
{
   int iRet;
   char* pcName = NULL;  
   char** argv = NULL;
   size_t iArg = 0;
   void (*pfRet) (int);
   pid_t pid;
   
   assert(oCommand != NULL);
  
   if(oCommand->pcName != NULL)
      pcName = oCommand->pcName;

   /*Handle exit - ish will exit with status 0 */
   if (strcmp(pcName, "exit") == 0)
   {
      /* Parent process ignores SIGINT signals */
     pfRet = signal(SIGINT, SIG_IGN);
     if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}
     if(oTokens != NULL)
      {
         LexAnalyzer_freeTokens(oTokens);
         DynArray_free(oTokens);
      }
     Command_free(oCommand);
     exit(0);
   }
  
   /* Get arguments from oCommand, and get the number of args */
   argv = Command_getArgs(oCommand);
   if(oCommand->oArgs != NULL)
        iArg = DynArray_getLength(oCommand->oArgs);
   /* Handle three other shell built-in commands 
      - setenv var [value], unsetenv var, cd [dir] */
   if (strcmp(pcName, "setenv") == 0 ||
       strcmp(pcName, "unsetenv") == 0 ||
       strcmp(pcName,"cd") == 0)
        handleBuiltInCommands(pcName, argv, iArg);
   else
   {
     /*Handle External Commands*/
      iRet = fflush(NULL);
      if (iRet == EOF) {perror(pcPgmName); exit(EXIT_FAILURE);}
   
      /* Parent proccess ignore SIGINT signals */
      pfRet = signal(SIGINT, SIG_IGN);
      if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}

      pid = fork();
     if (pid == -1) {perror(pcPgmName); exit(EXIT_FAILURE);}

     if (pid == 0)
     {
        /***********IN Child Process ********/
        /* Restore the default behaviors of SIGINT signals */
        pfRet = signal(SIGINT, SIG_DFL);
        if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}

        /* Handle redirection */
        handleRedirection(oCommand);
     
        /* Excecute the command */
        execvp(pcName, argv);

        fprintf(stderr, "%s: No such file or directory\n", pcPgmName);
        exit(EXIT_FAILURE);
     }
     pid = wait(NULL);
     if(pid == -1)  {perror(pcPgmName); exit(EXIT_FAILURE);}
   }

   if(argv!=NULL)
      free(argv);
   Command_free(oCommand);
  
}   


/*--------------------------------------------------------------------*/
/* Read a Line fro stdin. Write the line to stdout. If the Line 
   doesn't have lexical error after lexical analyis, then a dynamic 
   array of tokens will be returned and passed into synthecial analysis.
   If there's no error, a command object will be returned from 
   synthetic analysis. Then if the object is non-null, the command 
   will be executed using ishExec function. Repeat this process 
   until EOF. Return 0 iff sucessful. As always, argc is the 
   command-line argument coutn and argv is an array of 
   command-line arguments. */

int main(int argc, char *argv[])
{
   char *pcLine;
   DynArray_T oTokens = NULL;
   int iRet;
   Command_T oCommand = NULL;

   void (*pfRet) (int);

   pcPgmName = argv[0];

   printf("%% ");

   pfRet = signal(SIGINT, myHandler);   
   if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);}
   
   
   while ((pcLine = ishReadLine(stdin)) != NULL)
   {
      printf("%s\n", pcLine);
      iRet = fflush(stdout);
      if(iRet == EOF)
      {perror(pcPgmName); exit(EXIT_FAILURE);}

      /* Pass through lexAnalyzer and synAnalyzer*/
      oTokens = LexAnalyzer_lexLine(pcLine);
      if (oTokens != NULL)
      {oCommand = SynAnalyzer_syn(oTokens);}
      else {oCommand = NULL;}

      if(oCommand != NULL)
      {
         ishExec(oCommand, oTokens);
      }
      if(oTokens != NULL)
      {
         LexAnalyzer_freeTokens(oTokens);
         DynArray_free(oTokens);
      }
      
      printf("%% ");

      pfRet = signal(SIGINT, myHandler);   
      if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE);} 
      free(pcLine);
   }
   printf("\n");
   return 0;
}

