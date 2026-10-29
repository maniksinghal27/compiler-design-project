

Requirements:
- GCC
- Flex
- Bison
- WSL/Linux 

Build:
1. Open terminal in this folder.
2. Run:

bison -d parser.y
flex lexer.l
gcc parser.tab.c lex.yy.c -o compiler

Run:
./compiler < input.txt

Interactive mode:
./compiler

Press Ctrl+D after entering the complete source program.

Important:
The compiler currently supports:
- Assignment statements
- Arithmetic expressions
- print()
- if / else
- while loops
- for loops
- Nested while loops
- Nested control flow
- Three-address code generation
- Backpatching
- Quadruple generation
- TAC execution

For-loop syntax:

for (i = 0; i <= 5; i = i + 1) {
    print(i);
}

The language does NOT currently support i++.