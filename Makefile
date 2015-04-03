YFLAGS        = -d

PROGRAM       = calc

OBJS          = yacc.tab.o lex.yy.o turtle.o

SRCS          = yacc.tab.c lex.yy.c turtle.c

CC            = gcc 

all:            $(PROGRAM)

.c.o:           $(SRCS)
                $(CC) -c $*.c -o $@ -O

yacc.tab.c:        yacc.y
                bison $(YFLAGS) yacc.y

lex.yy.c:       lex.l 
                flex lex.l

shell:           $(OBJS)
                $(CC) $(OBJS)  -o $@ -lfl -lm

clean:;         rm -f $(OBJS) core *~ \#* *.o $(PROGRAM) \
                y.* lex.yy.* yac.tab.*