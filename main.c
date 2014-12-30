#include <sys/param.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include "juniper-lex.h"

extern FILE	*yyin;
extern char	*yytext;
extern int	yyparse(void);

void		load_regex(char *);
int		regex_match(char *);
void		dump_addresses(void);
void		parse_config(char *);
void		yyerror(const char *);
int		yywrap(void);
void		usage(void);

#define	FLAGS_INETONLY	0x00000001

int		optflags = 0;

int		lineno = 1;

struct interface	*iflist = NULL;
int			ninterfaces = 0;

/* regex match list */
#define MAX_REGEX	256
int			nexpressions = 0;
regex_t			expressions[MAX_REGEX];

void
add_address(char *addr, int family, struct interface *intptr)
{
	struct address *aptr;
	int i;

	if ((optflags & FLAGS_INETONLY) && (family == ATYPE_INET6))
		return;

	for (i = 0; i < MAX_ADDR; i++) {
		if (intptr->alist[i].str[0] == '\0') {
			aptr = &intptr->alist[i];
			break;
		}
	}
	if (i == MAX_ADDR) {
		fprintf(stderr, "maximum addresses reached on %s\n",
		    intptr->name);
		exit(1);
	}

	aptr->type = family;
	strncpy(aptr->str, addr, sizeof(aptr->str) - 1);
}

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
	memset(new, 0, sizeof(struct interface));
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
dump_addresses()
{
	struct interface *iptr;
	struct address *aptr;
	int i;

	for (iptr = iflist; iptr->next != NULL; iptr = iptr->next) {
		printf("# interface %s\n", iptr->name);

		for (i = 0; i < MAX_ADDR; i++) {
			aptr = &iptr->alist[i];
			if (aptr->str[0] == '\0')
				break;
			if (regex_match(aptr->str))
				printf("%s\n", aptr->str);
			else
				printf("# regex_no_match %s\n", aptr->str);
		}
	}
}

int
regex_match(char *buf)
{
	regmatch_t *pm;
	size_t nm;
	int i;

	if (nexpressions == 0)
		return (1);

	for (i = 0; i < nexpressions; i++) {
		nm = 0;
		pm = NULL;
		if (regexec(&expressions[i], buf, nm, pm, 0) == 0)
			return (1);
	}
	return (0);
}

void
load_regex(char *path)
{
	int i;
	char *p0;
	char buf[1024];
	FILE *f;

	f = fopen(path, "r");
	if (f == NULL) {
		perror("fopen");
		exit(1);
	}
	for (i = 0; i < MAX_REGEX; i++) {
		if (fgets(buf, sizeof(buf), f) == NULL)
			break;
		for (p0 = buf; *p0 != '\0'; p0++) {
			if ((*p0 == '\n') || (*p0 == '\r')) {
				*p0 = '\0';
				break;
			}
		}

		if (regcomp(&expressions[i], buf, REG_EXTENDED) != 0) {
			fprintf(stderr, "error compiling %s\n", buf);
			exit(1);
		}
		nexpressions++;
	}
	fclose(f);
}

void
usage()
{
	printf("usage: juniper-lex [-4h] [-r path] config_path\n");
	exit(0);
}

int
main(int argc, char *argv[])
{
	char *regexpath = NULL;
	char *p0, fname[MAXPATHLEN];
	char *confpath;
	char ch;

	while ((ch = getopt(argc, argv, "4hr:")) != -1) {
		switch (ch) {
		case '4':
			optflags |= FLAGS_INETONLY;
			break;
		case 'r':
			regexpath = optarg;
			break;
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

	p0 = strrchr(confpath, '/');
	if (p0 != NULL) {
		if (strlen(p0) <= 1) {
			fprintf(stderr, "invalid configuration file path\n");
			exit(1);
		}
		memset(fname, 0, sizeof(fname));
		strncpy(fname, ++p0, sizeof(fname) - 1);
	}
	else {
		memset(fname, 0, sizeof(fname));
		strncpy(fname, confpath, sizeof(fname) - 1);
	}

	if (regexpath != NULL)
		load_regex(regexpath);

	parse_config(confpath);

	printf("# %s\n", fname);
	dump_addresses();
	printf("# interfaces processed: %d\n", ninterfaces);

	return (0);
}
