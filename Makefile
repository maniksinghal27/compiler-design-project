all: compiler

compiler: lex.yy.c parser.tab.c
	gcc lex.yy.c parser.tab.c -o compiler

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h compiler