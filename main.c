#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern FILE	*yyin;
extern char	*yytext;
extern int	yyparse(void);

void		parse_config(char *);
void		yyerror(const char *);
int		yywrap(void);
void		usage(void);

int		lineno = 1;

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

	return (0);
}
