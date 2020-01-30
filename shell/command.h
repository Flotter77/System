/*--------------------------------------------------------------------*/
/* command.h                                                          */
/* Author: Jinyuan (Chi) Qi                                           */
/*--------------------------------------------------------------------*/

#ifndef COMMAND_INCLUDED
#define COMMAND_INCLUDED

#include "dynarray.h"

/*--------------------------------------------------------------------*/
/* The command struct has a Command Name, a dynamic array of args, 
   a stdin filename, and a stdout filename */

struct Command
{
   /* Command Name*/
   char* pcName;

   /* Array of args */
   DynArray_T oArgs;

   /* Stdin Filename */
   char* pcStdin;

   /* Stdout Filename */
   char* pcStdout;
};

/* Define the command object Command_T */
typedef struct Command* Command_T;


/*--------------------------------------------------------------------*/
/* The name of the executable binary file. */
const char *pcPgmName;

/*--------------------------------------------------------------------*/
/* Create and return a command that consists of three string values        
   (command name pcName, stdin filename pcStdin, and stdout filename       
   pcStdout), and a DynArray object (oArgs) that consists of args. */
Command_T  Command_new(char* pcName, DynArray_T oArgs,
                       char* pcStdin, char* pcStdout);

/*--------------------------------------------------------------------*/
/* Free oCommand object. */
void Command_free(Command_T oCommand);

/*--------------------------------------------------------------------*/
/* Print oCommand object. */
void Command_print(Command_T oCommand);


/*--------------------------------------------------------------------*/
/* Take a Command object oCommand and return a pointer to an array of 
  strings (char**argv)  for execution function*/
char** Command_getArgs(Command_T oCommand);

#endif
   
