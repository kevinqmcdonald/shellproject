%{
#include <stdio.h>
#include "y.tab.h"
#include "turtle.h"
%}

%%

bye						return BYE;
cd						return CD;
alias						return ALIAS;
unalias						return UALIAS;
setenv						return SET;
unsetenv					return UNSET;
printenv					return PENV;
pwd						return PwD;
<<EOF>>						{
								if(ignoreEOF == 1)
									return 0;
								return BYE;
							}
[a-zA-Z0-9\.\/-]+			{ 
								yylval.string=strdup(yytext); 
								return WORD;
							} 
"${"[a-zA-Z0-9]+"}"			{
								char sub[yyleng-2];
								strncpy(sub, yytext+2, (yyleng-3));
								sub[yyleng-3] = "\0";
								yylval.string=strdup(yytext);
								return VARIABLE;
							}
\"(\\.|[^"\n])*\"			{
								char sub[yyleng - 1];
								strncpy(sub, yytext + 1, (yyleng - 2));
								sub[yyleng - 2] = '\0';
								yylval.string=strdup(sub); 
								return WORD;
							}
"|"							return PIPE;
"\\"[<>|&]					/* ignore escape chars */;
[ \t]+						/* ignore whitespace */;
"<"							return LT;
">"							return GT;
"&"							return AMP;
"\n"						return 0;

%% //subroutines

int yywrap(void)
{
	return 1;
}

void parse_string(char* str)
{
	yy_scan_string(str);
	yyparse();
	yylex_destroy();
}

