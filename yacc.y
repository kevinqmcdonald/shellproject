%{
#include "turtle.h"

/* parser local working data section */
 extern char *yytext;

 void yyerror(const char *s) {
    fprintf(stderr, "ERR: %s", s);
 }
%}

%union {
    int i;
    char *string;
}

%token <i> LT GT AMP PIPE PENV SET UNSET
%token <i> CD BYE ALIAS UALIAS PwD 
%token <string> WORD VARIABLE

%%

    commands : /* empty */
        |   commands command;

    command :
        byebye
        |
        whereami
        |
        changedir
        |
        argument
        |
        printenv
        |
        setenv
        |
        unsetenv
        |
        printalias
        |
        setalias
        |
        removealias
        |
        redir
        ;

    byebye :
        BYE
        {
            printf("Seeya\n");
            exit(0);
        };

    whereami :
        PwD
        {
            comtab[currcmd].comname = "pwd";
            comtab[currcmd].code = PWD;
        };

    changedir :
        CD
        {
            comtab[currcmd].comname = "cd home";
            comtab[currcmd].code = CHD;
        }
        |
        CD WORD
        {
            comtab[currcmd].comname = "cd dir";
            comtab[currcmd].code = CDX;
            comtab[currcmd].atptr[0] = $2;
            comtab[currcmd].nargs++;
        };

    argument :
        WORD
        {
            // WORD is either external command,
            if(comtab[currcmd].external == 0) {
                comtab[currcmd].external = 1;
                comtab[currcmd].comname = $1;
            } // or an argument
            else {
                int tmp = comtab[currcmd].nargs;

                if(tmp < MAXARGS) {
                    comtab[currcmd].atptr[tmp] = $1;
                    comtab[currcmd].nargs++;
                }
            }
        }
        |
        argument WORD
        {
            int tmp = comtab[currcmd].nargs;

            if(tmp < MAXARGS) {
                comtab[currcmd].atptr[tmp] = $2;
                comtab[currcmd].nargs++;
            }
        }
        |
        command WORD
        {
            int tmp = comtab[currcmd].nargs;

            if(tmp < MAXARGS) {
                comtab[currcmd].atptr[tmp] = $2;
                comtab[currcmd].nargs++;
            }
        }
        |
        VARIABLE
        {
            char *varname = getenv($1);
            if(varname == NULL)
                printf("Variable %s does not exist\n", $1);
            else
                printf("%s\n", getenv($1));
        }
        |
        argument VARIABLE
        {
            int tmp = comtab[currcmd].nargs;

            if(tmp < MAXARGS) {
                char *varname = getenv($2);
                comtab[currcmd].atptr[tmp] = varname;
                comtab[currcmd].nargs++;
            }
        }
        |
        command VARIABLE
        {
            int tmp = comtab[currcmd].nargs;

            if(tmp < MAXARGS) {
                char *varname = getenv($2);
                if(varname == NULL)
                    printf("Variable %s does not exist\n", $2);
                else {
                    comtab[currcmd].atptr[tmp] = varname;
                    comtab[currcmd].nargs++;
                }
            }
        };

    printenv :
        PENV
        {
            comtab[currcmd].comname = "printenv";
            comtab[currcmd].code = PRINTENV;
        };

    setenv :
        SET WORD WORD
        {
            comtab[currcmd].atptr[0] = $2;
            comtab[currcmd].atptr[1] = $3;
            comtab[currcmd].comname = "setenv";
            comtab[currcmd].code = SETENV;
        };

    unsetenv :
        UNSET WORD
        {
            comtab[currcmd].comname = "unsetenv";
            comtab[currcmd].code = UNSETENV;
            comtab[currcmd].atptr[0] = $2;
        };

    printalias :
        ALIAS
        {
            comtab[currcmd].comname = "printalias";
            comtab[currcmd].code = PRINTALIAS;
        };

    setalias :
        ALIAS WORD WORD
        {
            comtab[currcmd].atptr[0] = $2;
            comtab[currcmd].atptr[1] = $3;
            comtab[currcmd].comname = "setalias";
            comtab[currcmd].code = SETALIAS;
        };

    removealias :
        UALIAS WORD
        {
            comtab[currcmd].atptr[0] = $2;
            comtab[currcmd].comname = "unalias";
            comtab[currcmd].code = UNALIAS;
        };

    redir :
        GT WORD
        {
            comtab[currcmd].outfd = $2;
            ofileredir = 1;
            printf("Output has been redirected to %s\n", $2);
        }
        |
        LT WORD
        {
            comtab[currcmd].infd = $2;
            ifileredir = 1;
            printf("Input has been redirected to %s\n", $2);
        }
        |
        GT GT WORD
        {
            comtab[currcmd].comname = "unalias";
            comtab[currcmd].code = UNALIAS;
            comtab[currcmd].outfd = $3;
            comtab[currcmd].append = 1;
            printf("Output has been appended to %s\n", $3);
        };

