%{
#include <stdio.h>

extern void	yyerror(const char *);
extern int	yylex(void);
%}

%union {
	char		*str;
}

%token STRING OBRACE EBRACE SEMICOLON
%token INTERFACES GIGETHER_OPTIONS FABRIC_OPTIONS
%token UNIT DESCRIPTION REDUNDANT_ETHER_OPTIONS
%token FAMILY

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
	| STRING OBRACE interface_entries EBRACE
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
	| FAMILY STRING OBRACE family_statements EBRACE

family_statements:
	family_statement
	| family_statements family_statement

family_statement:
	statement

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
