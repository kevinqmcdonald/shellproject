PROGRAM = shell
LEX     = flex
YACC    = yacc
YFLAGS  = -d
CC      = gcc 
OBJS    = y.tab.o lex.yy.o turtle.o
SRCS    = y.tab.c lex.yy.c turtle.c

all:        $(PROGRAM)
shell:      y.tab.o lex.yy.o turtle.o
	$(CC) -o shell turtle.o lex.yy.o y.tab.o
turtle.o:   turtle.c
	$(CC) -c turtle.c
lex.yy.o:   y.tab.h lex.yy.c
	$(CC) -c lex.yy.c 
y.tab.o:    y.tab.c 
	$(CC) -c y.tab.c 
lex.yy.c:   lex.l y.tab.h
	$(LEX) lex.l 
y.tab.c y.tab.h:    yacc.y
	$(YACC) -d yacc.y 
clean:
	rm -f *.o
	rm -f y.tab.*
	rm -f lex.yy.*
	rm -f shell
