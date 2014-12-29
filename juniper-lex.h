#define MAX_ADDR	32

enum {
	ATYPE_INET4,
	ATYPE_INET6
};

struct address {
	int			type;
	char			str[512];
};

struct interface {
	char			*name;
	struct interface	*next;

	struct address		alist[MAX_ADDR];
};

struct interface	*add_interface(char *);
void			add_address(char *, struct interface *);
