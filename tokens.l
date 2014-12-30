%{
#include <stdio.h>
#include "grammar.tab.h"

#define YY_NO_INPUT

extern int	lineno;

int		yycontext = 0;
int		ignore_tokens = 0;
int		parser_mode = 0;

#define INTERFACES_MATCH_CONTEXT 2

enum {
	MODE_NONE,
	MODE_INTERFACES
};

void
set_ignore_tokens(int f)
{
	ignore_tokens = f;
}

int
parser_mode_return(char *t)
{
	yylval.str = strdup(t);

	if (ignore_tokens)
		return (STRING);

	if (parser_mode == MODE_NONE)
		return (STRING);
	else if (parser_mode == MODE_INTERFACES) {
		if (strcmp(t, "family") == 0)
			return (FAMILY);
		else if (strcmp(t, "address") == 0)
			return (ADDRESS);

		if (yycontext != INTERFACES_MATCH_CONTEXT)
			return (STRING);

		if (strcmp(t, "gigether-options") == 0)
			return (GIGETHER_OPTIONS);
		else if (strcmp(t, "fabric-options") == 0)
			return (FABRIC_OPTIONS);
		else if (strcmp(t, "unit") == 0)
			return (UNIT);
		else if (strcmp(t, "description") == 0)
			return (DESCRIPTION);
		else if (strcmp(t, "redundant-ether-options") == 0)
			return (REDUNDANT_ETHER_OPTIONS);
		else if (strcmp(t, "inactive:") == 0)
			return (INACTIVE);
	}
	return (STRING);
}

void
parser_set_mode(int m)
{
	parser_mode = m;
}

%}

%option nounput

tokenchar	[^[:blank:];\n]
tokenstring	{tokenchar}+
poundcomment	\#.*\n
ccomment	\/\*.*\n

%s inner

%%

{poundcomment}		/* ignore comments */
{ccomment}		/* ignore comments */
[ \t]+			/* whitespace ignored */
\n			lineno++;
gigether-options	return parser_mode_return(yytext);
fabric-options		return parser_mode_return(yytext);
unit			return parser_mode_return(yytext);
description		return parser_mode_return(yytext);
redundant-ether-options	return parser_mode_return(yytext);
family			return parser_mode_return(yytext);
address			return parser_mode_return(yytext);
inactive:		return parser_mode_return(yytext);
interface-range		return (INTERFACE_RANGE);
interfaces		{
				if (yycontext == 0) {
					parser_set_mode(MODE_INTERFACES);
					return (INTERFACES);
				}
				else {
					yylval.str = strdup(yytext);
					return (STRING);
				}
			}
\{			{
				yycontext++;
				return (OBRACE);
			}
\}			{
				yycontext--;
				if (yycontext == 0)
					parser_set_mode(MODE_NONE);
				return (EBRACE);
			}
\;			return (SEMICOLON);
{tokenstring}		{
				yylval.str = strdup(yytext);
				return (STRING);
			}
