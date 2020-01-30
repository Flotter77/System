/*--------------------------------------------------------------------*/
/* lexAnalyzer.c                                                      */
/* Author: Jinyuan (Chi) Qi                                           */
/*--------------------------------------------------------------------*/

#include "ishlex.h"
#include "lexAnalyzer.h"
#include "dynarray.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*--------------------------------------------------------------------*/
/* The name of the executable binary file. */
const char *pcPgmName; 
 

/*--------------------------------------------------------------------*/
/* Free all of the tokens in oTokens. */
void LexAnalyzer_freeTokens(DynArray_T oTokens)
{
   size_t u;
   size_t uLength;
   Token_T psToken;

   assert(oTokens != NULL);
   uLength = DynArray_getLength(oTokens);

   for (u = 0; u < uLength; u++)
   {
      psToken = DynArray_get(oTokens, u);
      free(psToken->pcValue);
      free(psToken);
   }
}

/*--------------------------------------------------------------------*/
/* Create and return a token whose type is eTokenType and whose
   value consists of string pcValue.  The caller owns the token. */

static Token_T LexAnalyzer_newToken(enum TokenType eTokenType,
                              char *pcValue)
{
   Token_T psToken;
   
   assert(pcValue != NULL);
   
   psToken = (Token_T)malloc(sizeof(struct Token));
   if (psToken == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}

   psToken->eType = eTokenType;
   psToken->pcValue =
      (char*)malloc(strlen(pcValue) + 1);
   if (psToken->pcValue == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}

   strcpy(psToken->pcValue, pcValue);
   return psToken;
}

/*--------------------------------------------------------------------*/

/* Lexically analyze string pcLine.  If pcLine contains a lexical
   error, then return NULL.  Otherwise return a DynArray object
   containing the tokens in pcLine.  The caller owns the DynArray
   object and the tokens that it contains. */

DynArray_T LexAnalyzer_lexLine(const char *pcLine)
{
   /* lexLine() uses a DFA approach.  It "reads" its characters from
      pcLine. The DFA has these five states: */
   enum LexState {STATE_START, STATE_IN_SPECIAL, STATE_IN_ORDINARY,
                  STATE_IN_QUOTE, STATE_OUT_QUOTE};

   /* The current state of the DFA. */
   enum LexState eState = STATE_START;

   /* An index into pcLine. */
   size_t uLineIndex = 0;

   /* Pointer to a buffer in which the characters comprising each
      token are accumulated. */
   char *pcBuffer;

   /* An index into the buffer. */
   int uBufferIndex = 0;

   char c;
   Token_T psToken;
   DynArray_T oTokens;
   int iSuccessful;

   assert(pcLine != NULL);

   /* Create an empty token DynArray object. */
   oTokens = DynArray_new(0);
   if (oTokens == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}

   /* Allocate memory for a buffer that is large enough to store the
      largest token that might appear within pcLine. */
   pcBuffer = (char*)malloc(strlen(pcLine) + 1);
   if (pcBuffer == NULL)
      {perror(pcPgmName); exit(EXIT_FAILURE);}

   for (;;)
   {
      /* "Read" the next character from pcLine. */
      c = pcLine[uLineIndex++];

      switch (eState)
      {
         /* Handle the START state. */
         case STATE_START:
            if (c == '\0')
            {
               free(pcBuffer);
               return oTokens;
            }
            else if (c == '<' || c == '>' )
            {
               pcBuffer[uBufferIndex++] = c;

               eState = STATE_IN_SPECIAL;
            }
            else if (c == '\"')
            {
               eState = STATE_IN_QUOTE;
            }
            else if (isspace(c))
               eState = STATE_START;
            else
            {
               pcBuffer[uBufferIndex++] = c;

               eState = STATE_IN_ORDINARY;
            }
            break;

            /* Handle the IN_SPECIAL state. */
         case STATE_IN_SPECIAL:
            if (c == '\0')
            {
               /* Create a SPECIAL token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = LexAnalyzer_newToken(TOKEN_SPECIAL, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
                  {perror(pcPgmName); exit(EXIT_FAILURE);}
               uBufferIndex = 0;
               /* Exit */
               free(pcBuffer);
               return oTokens;
            }
            else if (c == '<' || c == '>')
            {
               pcBuffer[uBufferIndex++] = c;

               eState = STATE_IN_SPECIAL;
            }
            else if (isspace(c))
            {
               /* Create a SPECIAL token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = LexAnalyzer_newToken(TOKEN_SPECIAL, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
                  {perror(pcPgmName); exit(EXIT_FAILURE);}
               uBufferIndex = 0;

               eState = STATE_START;
            }
            else if (c == '\"')
            {
               /* Create a SPECIAL token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = LexAnalyzer_newToken(TOKEN_SPECIAL, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
                  {perror(pcPgmName); exit(EXIT_FAILURE);}
               uBufferIndex = 0;

               eState = STATE_IN_QUOTE;
            }
            else
            {
               /* Create a SPECIAL token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = LexAnalyzer_newToken(TOKEN_SPECIAL, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
                  {perror(pcPgmName); exit(EXIT_FAILURE);}
               uBufferIndex = 0;
               pcBuffer[uBufferIndex++] = c;

               eState = STATE_IN_ORDINARY;
            }
            break;

            /* Handle the IN_ORDINARY state. */
         case STATE_IN_ORDINARY:
            if (c == '\0')
            {
               /* Create an ORDINARY token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = LexAnalyzer_newToken(TOKEN_ORDINARY, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
                  {perror(pcPgmName); exit(EXIT_FAILURE);}
               uBufferIndex = 0;
               /* Exit */
               free(pcBuffer);
               return oTokens;
            }
            else if (c == '<' || c == '>' )
            {
               /* Create an ORDINARY token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = LexAnalyzer_newToken(TOKEN_ORDINARY, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
                  {perror(pcPgmName); exit(EXIT_FAILURE);}
               uBufferIndex = 0;
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_IN_SPECIAL;
            }
            else if (isspace(c))
            {
               /* Create an ORDINARY token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = LexAnalyzer_newToken(TOKEN_ORDINARY, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
                  {perror(pcPgmName); exit(EXIT_FAILURE);}
               uBufferIndex = 0;

               eState = STATE_START;
            }
            else if (c == '\"')
               eState = STATE_IN_QUOTE;            
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_IN_ORDINARY;
            }
            break;

            /* Handle the IN_ORDINARY state. */
         case STATE_IN_QUOTE:
            if (c == '\0')
            {
               fprintf(stderr, "%s: unmatched quote\n", pcPgmName);
               /* Exit */
               free(pcBuffer);
               LexAnalyzer_freeTokens(oTokens);
               DynArray_free(oTokens);
               return NULL;
            }
            else if (c == '\"')
               {eState = STATE_OUT_QUOTE;}
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_IN_QUOTE;
            }
            break;
            
            /* Handle the IN_ORDINARY state. */
         case STATE_OUT_QUOTE:
            if (c == '\0')
            {
               /* Create an ORDINARY token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = LexAnalyzer_newToken(TOKEN_ORDINARY, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
                 {perror(pcPgmName); exit(EXIT_FAILURE);}
               uBufferIndex = 0;
               /* Exit */
               free(pcBuffer);
               return oTokens;
            }
            else if (c == '<' || c == '>')
            {
               /* Create a SPECIAL token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = LexAnalyzer_newToken(TOKEN_SPECIAL, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
                  {perror(pcPgmName); exit(EXIT_FAILURE);}
               uBufferIndex = 0;
               eState = STATE_IN_SPECIAL;
            }
            else if (isspace(c))
            {
               /* Create an ORDINARY token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = LexAnalyzer_newToken(TOKEN_ORDINARY, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
                  {perror(pcPgmName); exit(EXIT_FAILURE);}
               uBufferIndex = 0;
               eState = STATE_START;
            }
            else if (c == '\"')
            {
               /* Create an ORDINARY token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = LexAnalyzer_newToken(TOKEN_ORDINARY, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
                  {perror(pcPgmName); exit(EXIT_FAILURE);}
               uBufferIndex = 0;
               eState = STATE_IN_QUOTE;
            }
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_IN_ORDINARY;
            }
            break;
            
         default:
            assert(0);
      }
   }
}
