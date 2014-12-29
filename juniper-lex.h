struct interface {
	char			*name;
	struct interface	*next;
};

struct interface	*add_interface(char *);
