/*--------------------------------------------------------------------*/
/* synAnalyzer.h                                                      */
/* Author: Jinyuan (Chi) Qi                                           */
/*--------------------------------------------------------------------*/

#ifndef SYNANALYZER_INCLUDED
#define SYNANALYZER_INCLUDED

#include "dynarray.h"
#include "command.h"

/*--------------------------------------------------------------------*/
/* The name of the executable binary file. */
const char *pcPgmName;

/*--------------------------------------------------------------------*/
/* Accepts a DynArray object oTokens, and anaylzes whether  
   the tokens have synthetic error. If yes, then quit and return NULL.
   Otherwise, returns a Command object. */
Command_T SynAnalyzer_syn(DynArray_T oTokens);

#endif 
