%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"

extern int current_col;
extern int current_line;

void update_location(int length) {
    yylloc.first_line = current_line;
    yylloc.first_column = current_col;
    yylloc.last_line = current_line;
    yylloc.last_column = current_col + length - 1;
    current_col += length;
}

void record_token(const char *type, const char *lexeme, int line, int col);
void record_lexical_error(const char *msg, int line, int col);
%}

%option noyywrap
%option yylineno

%%

\"[^\"]*\"      { 
    update_location(yyleng);
    yylval.str = strdup(yytext); 
    record_token("STRING_LITERAL", yytext, yylloc.first_line, yylloc.first_column);
    return STRING_LITERAL; 
}

"print"         { update_location(yyleng); record_token("PRINT", yytext, yylloc.first_line, yylloc.first_column); return PRINT; }
"if"            { update_location(yyleng); record_token("IF", yytext, yylloc.first_line, yylloc.first_column); return IF; }
"else"          { update_location(yyleng); record_token("ELSE", yytext, yylloc.first_line, yylloc.first_column); return ELSE; }
"while"         { update_location(yyleng); record_token("WHILE", yytext, yylloc.first_line, yylloc.first_column); return WHILE; }
"for"           { update_location(yyleng); record_token("FOR", yytext, yylloc.first_line, yylloc.first_column); return FOR; }

[a-zA-Z_][a-zA-Z0-9_]* { 
    update_location(yyleng);
    yylval.str = strdup(yytext); 
    record_token("ID", yytext, yylloc.first_line, yylloc.first_column);
    return ID; 
}

[0-9]+          { 
    update_location(yyleng);
    yylval.str = strdup(yytext); 
    record_token("NUM", yytext, yylloc.first_line, yylloc.first_column);
    return NUM; 
}

"<="            { update_location(yyleng); record_token("LE", yytext, yylloc.first_line, yylloc.first_column); return LE; }
">="            { update_location(yyleng); record_token("GE", yytext, yylloc.first_line, yylloc.first_column); return GE; }
"=="            { update_location(yyleng); record_token("EQ", yytext, yylloc.first_line, yylloc.first_column); return EQ; }
"!="            { update_location(yyleng); record_token("NE", yytext, yylloc.first_line, yylloc.first_column); return NE; }
"<"             { update_location(yyleng); record_token("LT", yytext, yylloc.first_line, yylloc.first_column); return LT; }
">"             { update_location(yyleng); record_token("GT", yytext, yylloc.first_line, yylloc.first_column); return GT; }
"="             { update_location(yyleng); record_token("ASSIGN", yytext, yylloc.first_line, yylloc.first_column); return ASSIGN; }
"+"             { update_location(yyleng); record_token("PLUS", yytext, yylloc.first_line, yylloc.first_column); return PLUS; }
"-"             { update_location(yyleng); record_token("MINUS", yytext, yylloc.first_line, yylloc.first_column); return MINUS; }
"*"             { update_location(yyleng); record_token("MULT", yytext, yylloc.first_line, yylloc.first_column); return MULT; }
"/"             { update_location(yyleng); record_token("DIV", yytext, yylloc.first_line, yylloc.first_column); return DIV; }

";"             { update_location(yyleng); record_token("SEMI", yytext, yylloc.first_line, yylloc.first_column); return SEMI; }
"("             { update_location(yyleng); record_token("LPAREN", yytext, yylloc.first_line, yylloc.first_column); return LPAREN; }
")"             { update_location(yyleng); record_token("RPAREN", yytext, yylloc.first_line, yylloc.first_column); return RPAREN; }
"{"             { update_location(yyleng); record_token("LBRACE", yytext, yylloc.first_line, yylloc.first_column); return LBRACE; }
"}"             { update_location(yyleng); record_token("RBRACE", yytext, yylloc.first_line, yylloc.first_column); return RBRACE; }

[ \t]+          { current_col += yyleng; }
\n              { current_line++; current_col = 1; }
\r              { /* ignore carriage return */ }

"//"[^\n]*      { /* skip single line comments */ }

.               { 
    record_lexical_error(yytext, current_line, current_col);
    update_location(yyleng);
}

%%