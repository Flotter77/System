/*--------------------------------------------------------------------*/
/* lexAnalyzer.h                                                      */
/* Author: Jinyuan (Chi) Qi                                           */
/*--------------------------------------------------------------------*/

#ifndef LEXANALYZER_INCLUDED
#define LEXANALYZER_INCLUDED

#include  "dynarray.h"
#include <stdio.h>

/*--------------------------------------------------------------------*/
/* The name of the executable binary file. */
const char *pcPgmName;


/*--------------------------------------------------------------------*/
/* A Token object can be either a special  or an ordinary. */
enum TokenType {TOKEN_SPECIAL, TOKEN_ORDINARY};


/*--------------------------------------------------------------------*/
/* A Token is expressed as a string. */

struct Token
{
   /* The type of the token. */
   enum TokenType eType;

   /* The string which is the token's value. */
   char *pcValue;
};        


/* A Token_T object is a Token made of strings with two types
  - special ('<' or '>') or ordinary. */
typedef struct Token *Token_T;

/*--------------------------------------------------------------------*/
/* Free all of the tokens in oTokens. */
void LexAnalyzer_freeTokens(DynArray_T oTokens);


/*--------------------------------------------------------------------*/
/* Lexically analyze string pcLine.  If pcLine contains a lexical      
   error, then return NULL.  Otherwise return a DynArray object             
   containing the tokens in pcLine.  The caller owns the DynArray
   object and the tokens that it contains. */

DynArray_T LexAnalyzer_lexLine(const char *pcLine);

#endif
