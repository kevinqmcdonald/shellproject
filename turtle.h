/*******************************************************************************
 *   turtle.h contains all data structures and Constants for the turtle shell  *
 *******************************************************************************
*/
 
#ifndef TURTLE_H
#define TURTLE_H


#define MAXCMDS 50
#define MAXARGS 300
#define MAXALIAS 100
#define MAXCACHE 100
#define MAXPATH 50

#define OK 0
#define SYSERR 1
#define OLD_ERR 3 
#define SYSCALLERR -1

#define BADFD -2
#define EOFILE 2

// For Aliasing
#define THE_ONLY_ONE 1
#define FIRST 2
#define LAST 3

/* Command line arguments structure */
 typedef struct comargs {
     char   *args[MAXARGS];
 }  ARGTAB;

/* command line structure */
 typedef struct com {
     char   *comname;
     int     remote;
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

/* cache table structure */
 typedef struct cache {
     char   *cmd;
     char   *hostport;
 }  CACHE;

/* externals */
 extern struct alias aliastab[];
 extern COMMAND      comtab[];
 extern CACHE        cachetab[];
 extern char        *pathtab[];

#endif