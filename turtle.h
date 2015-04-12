/*******************************************************************************
 *   turtle.h contains all data structures and Constants for the turtle shell  *
 *******************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#ifndef TURTLE_H
#define TURTLE_H

#define TURTLE_PROMPT "tsh $$"

#define CDX 1
#define CHD 2
#define SETALIAS 3
#define UNALIAS 4
#define PRINTALIAS 9
#define SETENV 5
#define UNSETENV 6
#define PRINTENV 7
#define PWD 8

#define MAXCMDS 50
#define MAXARGS 300
#define MAXALIAS 100
#define MAXPATH 50

#define OK 0
#define SYSERR 1

#define BADFD -2
#define EOFILE 2

/* Command line arguments structure */
 typedef struct comargs {
     char   *args[MAXARGS];
 }  ARGTAB;

/* command line structure */
 typedef struct com {
     char   *comname;
     int     code;
     int     external;
     int     infd;
     int     outfd;
     int     nargs;
     ARGTAB *atptr;
 }  COMMAND;

/* Alias Structure */
 struct alias {
     int    used;
     char  *alname;
     char  *alstring;
 };

/* externals */
 extern struct alias aliastab[];
 extern COMMAND      comtab[];
 extern int         currcmd;
 extern int         aliasno;
 extern char        *aliasroot;
 extern int         yylex();
 extern int         yylineno;
 extern char        *yytext;
 extern char        **environ;