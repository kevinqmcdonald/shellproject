%{
#include "turtle.h"
#include "bicmd.h"
/*******************************************************************************
 *             yacc.y contains all specs for the parser generator              *       
 *******************************************************************************
*/

/* parser local working data section */
 COMMAND *q, *p;
 int    pfd[2];
%}

%union {
    int i;
    char *s;
    char *w;
}

%token <i> LT GT AMP LPAREN RPAREN PIPE DOT DDOT DEBUG
%token <i> SET PROMPT CD BYE ALIAS UNALIAS PwD EXTEND
%token <i> ALIASLOOP
%token <w> WORD
%token <s> STRING PATH

%token <w> cmd.file

%start cmd

%%

cmd:            builtin.cmd { 
                    eventcount++; 
                }
            |   builtin.cmd LT WORD {
                    turtErr("illegal input redirection");
                    undoit(); 
                    bicmd = 0;
                }
            |   builtin.cmd LT STRING {
                    turtErr("illegal input redirection");
                    undoit(); 
                    bicmd = 0;
                }
            |   alias.cmd {
                    eventcount++;
                }
            |   alias.cmd LT WORD {
                    turtErr("illegal input redirection");
                    bicmd = 0;
                }
            |   alias.cmd LT STRING {
                    turtErr("illegal input redirection");
                    bicmd = 0;
                }
            |   other.cmd {
                    eventcount++;
                    builtin = 0;
                }

builtin.cmd:    SET PATH dir.list useless.redir {
                    bicmd = SETPATH;
                }
            |   SET PATH { 
                    pathleng = 0;
                    bicmd = SETPATH;
                }
            |   SET PROMPT STRING { 
                    bicmd = SETPROMPT;
                    bistr = mkstr($3);
                } useless.redir
            |   SET PROMPT {
                    bicmd = SETPROMPT;
                    bistr = mkstr("");
                } useless.redir
            |   SET {
                    bicmd = SETT;
                }
            |   SET GT WORD {
                    bicmd = SETT;
                    bioutf = 1;
                    bistr = mkstr($3);
                }
            |   SET GT STRING {
                    bicmd = SETT;
                    bioutf = 1;
                    bistr = mkstr($3);
                }
            |   SET GT GT WORD {
                    bicmd = SETT;
                    bioutf = 1;
                    bistr = mkstr($3);
                    append = 1;
                }
            |   SET GT GT STRING {
                    bicmd = SETT;
                    bioutf = 1;
                    bistr = mkstr($3);
                    append = 1;
                }
            |   CD WORD useless.redir {
                    bicmd = CDX;
                    bistr = mkstr($2);
                }
            |   CD STRING useless.redir {
                    bicmd = CDX;
                    bistr = mkstr($2);
                }
simple.cmd:     cmd.file {
                    commtab[currcmd].comname = mkstr($1);
                    commtab[currcmd].atptr = NIL(ARGTAB);
                    commtab[currcmd].nargs = 0;
                    /* First arg is reserved for the command */
                }
            |   cmd.file  arguments {
                    commtab[currcmd].comname = mkstr($1);
                    commtab[currcmd].nargs = currarg;
                }
            ;
cmd.file:       WORD {
                    $$ = $1;
                }
            |   STRING {
                    $$ = $1;
                }
            ;
arguments:      WORD {
                    (p = &comtab[currcmd])->atptr = Allocate(ARGTAB);
                    currarg = 1;
                    p->atptr->args[currarg++] = mkstr($1);
                }
            |   STRING {
                    (p = &comtab[currcmd])->atptr = Allocate(ARGTAB);
                    currarg = 1;
                    p->atptr->args[currarg++] = mkstr($1);
                }
            |   arguments WORD {
                    p->atptr->args[currarg++] = mkstr($2);
                }
            |   arguments STRING {
                    p->atptr->args[currarg++] = mkstr($2);
                }
            ;
input:          LT WORD {
                    strcpy( srcf, $2 );
                    comtab[0].infd = BADFD;
                }
            ;
output:         GT WORD {
                    strcpy( distf, $2 );
                    comtab[currcmd-1].outfd = BADFD;
                }
            |   GT GT WORD {
                    strcpy( distf, $3 );
                    comtab[currcmd-1].outfd = BADFD;
                    append = 1;
                }
            ;
words:          WORD WORD {

                }
            |   words WORD {

                }
            ;