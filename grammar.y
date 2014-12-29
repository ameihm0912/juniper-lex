%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "juniper-lex.h"

extern void	yyerror(const char *);
extern int	yylex(void);

struct interface	*current_interface = NULL;
int			current_family = 0;
%}

%union {
	char		*str;
}

%token STRING OBRACE EBRACE SEMICOLON
%token INTERFACES GIGETHER_OPTIONS FABRIC_OPTIONS
%token UNIT DESCRIPTION REDUNDANT_ETHER_OPTIONS
%token FAMILY ADDRESS

%type <str> STRING

%%

root:
	statements
	;

statements:
	statement
	| statements statement
	;

statement:
	command_spec
	| stanza_spec
	;

interfaces_inner:
	interface_statement
	| interfaces_inner interface_statement

interface_statement:
	STRING words SEMICOLON
	| STRING 
	{
		current_interface = add_interface($1);
	}
	OBRACE interface_entries EBRACE
	;

interface_entries:
	interface_entry
	| interface_entries interface_entry
	;

interface_entry:
	STRING SEMICOLON
	| STRING words SEMICOLON
	| DESCRIPTION words SEMICOLON
	| GIGETHER_OPTIONS OBRACE statements EBRACE
	| FABRIC_OPTIONS OBRACE statements EBRACE
	| UNIT STRING OBRACE unit_statements EBRACE
	| REDUNDANT_ETHER_OPTIONS OBRACE statements EBRACE

unit_statements:
	unit_statement
	| unit_statements unit_statement

unit_statement:
	STRING words SEMICOLON
	| STRING SEMICOLON
	| FAMILY STRING
	{
		if (strcmp("inet", $2) == 0)
			current_family = ATYPE_INET4;
		else if (strcmp("inet6", $2) == 0)
			current_family = ATYPE_INET6;
		else {
			fprintf(stderr, "bad family %s\n", $2);
			exit(1);
		}
	}
	OBRACE family_statements EBRACE

family_statements:
	family_statement
	| family_statements family_statement

family_statement:
	ADDRESS STRING SEMICOLON
	{
		add_address($2, current_interface);
	}
	| ADDRESS STRING OBRACE statements EBRACE
	| STRING OBRACE statements EBRACE
	| STRING STRING SEMICOLON

stanza_spec:
	INTERFACES OBRACE interfaces_inner EBRACE
	| STRING OBRACE stanza_inner EBRACE
	| STRING words OBRACE stanza_inner EBRACE

stanza_inner:
	statements

command_spec:
	STRING SEMICOLON
	| STRING words SEMICOLON
	;

words:
	STRING
	| words STRING
