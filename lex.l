%{
#include <y.tab.h>
%}

%%
[a-zA-z][a-zA-Z0-9]*	{return WORD;}
[a-zA-Z0-9\/.-]+	{return FILE;}
\|			{return PIPE;}
\\			{return BACKSLASH;}
"			{return QUOTE;}
[ \t]+			/* ignore whitespace */;
<			{return LT;}
>			{return GT;}
&			{return AMPERSAND;}
"[a-zA-Z0-9]*"		{return QWORD;}
%%