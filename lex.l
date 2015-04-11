%{
#include <y.tab.h>
%}

%%
bye							return BYE;
cd							return CD;
alias						return ALIAS;
unalias						return UNALIAS;
setenv						return SET;
unsetenv					return UNSET;
pwd							return PwD;
prompt						return PROMPT;
PATH						return PATH;
"[a-zA-Z][a-zA-Z0-9]*"		{ yylval.s=strdup(yytext); 
								return STRING;
							}
([a-zA-Z][a-zA-Z0-9]* | 
[a-zA-Z0-9\/.-]+)			{ yylval.s=strdup(yytext);
								return WORD;
							}
\|							return PIPE;
\\							return BACKSLASH;
"							return QUOTE;
[ \t]+						/* ignore whitespace */;
<							return LT;
>							return GT;
&							return AMP;
( 							return LPAREN;
)							return RPAREN;
\.							return DOT;
\.\.						return DDOT;
.							return yyerror("Invalid input entered");
%%
