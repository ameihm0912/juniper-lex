CC = gcc
BISON = bison
BISON_FLAGS = --defines -v
FLEX = flex
CFLAGS = -Wall -g
TARGETS = juniper-lex
OBJ = main.o grammar.tab.o lex.yy.o

all: $(TARGETS)

lex.yy.c: tokens.l
	$(FLEX) tokens.l

grammar.tab.c: grammar.y
	$(BISON) $(BISON_FLAGS) grammar.y

juniper-lex: $(OBJ)
	$(CC) -o $@ $(OBJ)

clean:
	rm -f $(TARGETS) *.o *.core core
	rm -f *.tab.c *.tab.h lex.yy.c
	rm -f *.output
