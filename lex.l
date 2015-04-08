%{
#include <y.tab.h>
%}

%%
"[a-zA-z][a-zA-Z0-9]*"		yylval.s=strdup(yytext); return WORD;
[a-zA-Z0-9\/.-]+			yylval.s=strdup(yytext); return PATH;
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
bye							return BYE;
cd							return CD;
alias						return ALIAS;
unalias						return UNALIAS;
setenv						return SET;
unsetenv					return UNSET;
pwd							return PwD;
%%
