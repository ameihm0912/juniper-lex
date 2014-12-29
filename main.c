#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "juniper-lex.h"

extern FILE	*yyin;
extern char	*yytext;
extern int	yyparse(void);

void		parse_config(char *);
void		yyerror(const char *);
int		yywrap(void);
void		usage(void);

int		lineno = 1;

struct interface	*iflist = NULL;
int			ninterfaces = 0;

struct interface *
get_interface(char *name)
{
	struct interface *p0;

	if (iflist != NULL) {
		for (p0 = iflist; p0->next != NULL; p0 = p0->next) {
			if (strcmp(p0->name, name) == 0)
				return (p0);
		}
	}
	return (NULL);
}

struct interface *
add_interface(char *name)
{
	struct interface *new;
	struct interface *p0;

	p0 = get_interface(name);
	if (p0 != NULL)
		return (p0);
	new = malloc(sizeof(struct interface));
	if (new == NULL) {
		perror("malloc");
		exit(1);
	}
	new->name = strdup(name);
	if (new->name == NULL) {
		perror("strdup");
		exit(1);
	}

	if (iflist != NULL) {
		for (p0 = iflist; p0->next != NULL; p0 = p0->next);
		p0->next = new;
	}
	else
		iflist = new;
	ninterfaces++;

	return (new);
}

int
yywrap()
{
	return (1);
}

void
yyerror(const char *s)
{
	fprintf(stderr, "yyparse: %s: line %d\n", s, lineno);
	exit (1);
}

void
parse_config(char *confpath)
{
	FILE *f;

	f = fopen(confpath, "r");
	if (f == NULL) {
		perror("fopen");
		exit(1);
	}
	yyin = f;
	yyparse();
	fclose(f);
}

void
usage()
{
	printf("usage: juniper-lex [-h] config_path\n");
	exit(0);
}

int
main(int argc, char *argv[])
{
	char *confpath;
	char ch;

	while ((ch = getopt(argc, argv, "h")) != -1) {
		switch (ch) {
		case 'h':
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;
	if (argc != 1)
		usage();
	confpath = argv[0];

	parse_config(confpath);
	printf("%d\n", ninterfaces);

	return (0);
}
