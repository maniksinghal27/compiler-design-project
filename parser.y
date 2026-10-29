%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex(void);

int current_line = 1;
int current_col = 1;
int json_output_mode = 0;
int compilation_failed = 0;

/* =========================================================
   DIAGNOSTICS & TOKENS TRACING
   ========================================================= */

typedef struct {
    char type[32];
    char lexeme[64];
    int line;
    int col;
} TokenRecord;

#define MAX_TOKENS 2000
TokenRecord token_records[MAX_TOKENS];
int token_count = 0;

void record_token(const char *type, const char *lexeme, int line, int col) {
    if (token_count < MAX_TOKENS) {
        strncpy(token_records[token_count].type, type, 31);
        strncpy(token_records[token_count].lexeme, lexeme, 63);
        token_records[token_count].line = line;
        token_records[token_count].col = col;
        token_count++;
    }
}

typedef struct {
    char severity[16];
    char message[256];
    int line;
    int col;
} Diagnostic;

#define MAX_DIAGNOSTICS 100
Diagnostic diagnostics[MAX_DIAGNOSTICS];
int diag_count = 0;

void add_diagnostic(const char *severity, const char *message, int line, int col) {
    if (diag_count < MAX_DIAGNOSTICS) {
        strncpy(diagnostics[diag_count].severity, severity, 15);
        strncpy(diagnostics[diag_count].message, message, 255);
        diagnostics[diag_count].line = line;
        diagnostics[diag_count].col = col;
        diag_count++;
    }
    if (strcmp(severity, "error") == 0) {
        compilation_failed = 1;
    }
}

void record_lexical_error(const char *msg, int line, int col) {
    char buf[256];
    snprintf(buf, sizeof(buf), "Lexical Error: Unrecognized character '%s'", msg);
    add_diagnostic("error", buf, line, col);
    if (!json_output_mode) {
        fprintf(stderr, "%s at line %d, col %d\n", buf, line, col);
    }
}

/* =========================================================
   NODE STRUCTURE FOR BACKPATCH LISTS
   ========================================================= */

typedef struct Node Node;

struct Node {
    int quad_index;
    Node *next;
};

/* =========================================================
   QUADRUPLE / THREE-ADDRESS CODE STRUCTURE
   ========================================================= */

typedef struct {
    int id;
    char op[16];
    char arg1[32];
    char arg2[32];
    char result[32];
    int line;
    int col;
} Quad;

#define MAX_QUADS 1000
Quad quad_table[MAX_QUADS];
int quad_count = 0;
int temp_var_count = 0;

typedef struct {
    char event_type[16]; // "makelist", "merge", "backpatch"
    int from_quad;
    int target_quad;
    char desc[128];
} BackpatchLog;

#define MAX_BP_LOGS 500
BackpatchLog bp_logs[MAX_BP_LOGS];
int bp_log_count = 0;

/* =========================================================
   HELPER FUNCTION DECLARATIONS
   ========================================================= */

int nextquad();
void emit_loc(const char *op, const char *arg1, const char *arg2, const char *result, int line, int col);
void emit(const char *op, const char *arg1, const char *arg2, const char *result);
char* newtemp();
Node* makelist(int index);
Node* merge(Node *l1, Node *l2);
void backpatch(Node *list, int target_quad);
void print_tac();
void print_quads();
void execute_tac();
void output_json();

%}

%locations

%union {
    char *str;
    int quad_num;
    struct {
        struct Node *truelist;
        struct Node *falselist;
    } expr;
    struct {
        struct Node *nextlist;
    } stmt;
}

%token <str> ID NUM STRING_LITERAL
%token WHILE FOR IF ELSE PRINT
%token LE GE EQ NE LT GT ASSIGN
%token PLUS MINUS MULT DIV
%token SEMI LPAREN RPAREN LBRACE RBRACE

%type <str> E
%type <expr> C
%type <stmt> S S_LIST
%type <quad_num> M
%type <stmt> N

%left PLUS MINUS
%left MULT DIV

%%

PROG
    : S_LIST
      {
          backpatch($1.nextlist, quad_count);
          if (!json_output_mode) {
              printf("\n--- GENERATING INTERMEDIATE CODE ---\n");
          }
      }
    ;

S_LIST
    : S_LIST M S
      {
          backpatch($1.nextlist, $2);
          $$.nextlist = $3.nextlist;
      }
    | S
      {
          $$.nextlist = $1.nextlist;
      }
    ;

M
    : /* empty */
      {
          $$ = nextquad();
      }
    ;

N
    : /* empty */
      {
          $$.nextlist = makelist(nextquad());
          emit_loc("goto", "", "", "_", @$.first_line, @$.first_column);
      }
    ;

FOR_INIT
    : ID ASSIGN E
      {
          emit_loc("=", $3, "", $1, @1.first_line, @1.first_column);
      }
    ;

FOR_STEP
    : ID ASSIGN E
      {
          emit_loc("=", $3, "", $1, @1.first_line, @1.first_column);
      }
    ;

S
    : ID ASSIGN E SEMI
      {
          emit_loc("=", $3, "", $1, @1.first_line, @1.first_column);
          $$.nextlist = NULL;
      }

    | PRINT LPAREN E RPAREN SEMI
      {
          emit_loc("print", $3, "", "", @1.first_line, @1.first_column);
          $$.nextlist = NULL;
      }

    | PRINT LPAREN STRING_LITERAL RPAREN SEMI
      {
          emit_loc("print_str", $3, "", "", @1.first_line, @1.first_column);
          $$.nextlist = NULL;
      }

    | IF LPAREN C RPAREN M LBRACE S_LIST RBRACE
      {
          backpatch($3.truelist, $5);
          $$.nextlist = merge($3.falselist, $7.nextlist);
      }

    | IF LPAREN C RPAREN M LBRACE S_LIST RBRACE ELSE N M LBRACE S_LIST RBRACE
      {
          backpatch($3.truelist, $5);
          backpatch($3.falselist, $11);
          Node *temp = merge($7.nextlist, $10.nextlist);
          $$.nextlist = merge(temp, $13.nextlist);
      }

    | WHILE LPAREN M C RPAREN M LBRACE S_LIST RBRACE
      {          
          backpatch($4.truelist, $6);
          char loop_target[16];
          sprintf(loop_target, "%d", $3);
          emit_loc("goto", "", "", loop_target, @1.first_line, @1.first_column);
          backpatch($8.nextlist, $3);
          $$.nextlist = $4.falselist;
      }

    | FOR LPAREN FOR_INIT SEMI M C SEMI M FOR_STEP
      {
          char cond_target[16];
          sprintf(cond_target, "%d", $5);
          emit_loc("goto", "", "", cond_target, @1.first_line, @1.first_column);
      }
      RPAREN M LBRACE S_LIST RBRACE
      {
          backpatch($6.truelist, $12);
          backpatch($14.nextlist, $8);
          char step_target[16];
          sprintf(step_target, "%d", $8);
          emit_loc("goto", "", "", step_target, @1.first_line, @1.first_column);
          $$.nextlist = $6.falselist;
      }

    | LBRACE S_LIST RBRACE
      {
          $$.nextlist = $2.nextlist;
      }
    ;

C
    : LPAREN C RPAREN
      {
          $$.truelist = $2.truelist;
          $$.falselist = $2.falselist;
      }

    | E LT E
      {
          $$.truelist = makelist(nextquad());
          $$.falselist = makelist(nextquad() + 1);
          emit_loc("<", $1, $3, "_", @2.first_line, @2.first_column);
          emit_loc("goto", "", "", "_", @2.first_line, @2.first_column);
      }

    | E GT E
      {
          $$.truelist = makelist(nextquad());
          $$.falselist = makelist(nextquad() + 1);
          emit_loc(">", $1, $3, "_", @2.first_line, @2.first_column);
          emit_loc("goto", "", "", "_", @2.first_line, @2.first_column);
      }

    | E LE E
      {
          $$.truelist = makelist(nextquad());
          $$.falselist = makelist(nextquad() + 1);
          emit_loc("<=", $1, $3, "_", @2.first_line, @2.first_column);
          emit_loc("goto", "", "", "_", @2.first_line, @2.first_column);
      }

    | E GE E
      {
          $$.truelist = makelist(nextquad());
          $$.falselist = makelist(nextquad() + 1);
          emit_loc(">=", $1, $3, "_", @2.first_line, @2.first_column);
          emit_loc("goto", "", "", "_", @2.first_line, @2.first_column);
      }

    | E EQ E
      {
          $$.truelist = makelist(nextquad());
          $$.falselist = makelist(nextquad() + 1);
          emit_loc("==", $1, $3, "_", @2.first_line, @2.first_column);
          emit_loc("goto", "", "", "_", @2.first_line, @2.first_column);
      }

    | E NE E
      {
          $$.truelist = makelist(nextquad());
          $$.falselist = makelist(nextquad() + 1);
          emit_loc("!=", $1, $3, "_", @2.first_line, @2.first_column);
          emit_loc("goto", "", "", "_", @2.first_line, @2.first_column);
      }
    ;

E
    : LPAREN E RPAREN
      {
          $$ = $2;
      }

    | E PLUS E
      {
          char *temp = newtemp();
          emit_loc("+", $1, $3, temp, @2.first_line, @2.first_column);
          $$ = temp;
      }

    | E MINUS E
      {
          char *temp = newtemp();
          emit_loc("-", $1, $3, temp, @2.first_line, @2.first_column);
          $$ = temp;
      }

    | E MULT E
      {
          char *temp = newtemp();
          emit_loc("*", $1, $3, temp, @2.first_line, @2.first_column);
          $$ = temp;
      }

    | E DIV E
      {
          char *temp = newtemp();
          emit_loc("/", $1, $3, temp, @2.first_line, @2.first_column);
          $$ = temp;
      }

    | ID
      {
          $$ = $1;
      }

    | NUM
      {
          $$ = $1;
      }
    ;

%%

int nextquad() {
    return quad_count;
}

void emit_loc(const char *op, const char *arg1, const char *arg2, const char *result, int line, int col) {
    if (quad_count >= MAX_QUADS) {
        fprintf(stderr, "ERROR: Quadruple table overflow.\n");
        exit(EXIT_FAILURE);
    }
    quad_table[quad_count].id = quad_count;
    strncpy(quad_table[quad_count].op, op, 15);
    strncpy(quad_table[quad_count].arg1, arg1, 31);
    strncpy(quad_table[quad_count].arg2, arg2, 31);
    strncpy(quad_table[quad_count].result, result, 31);
    quad_table[quad_count].line = line > 0 ? line : current_line;
    quad_table[quad_count].col = col > 0 ? col : 1;
    quad_count++;
}

void emit(const char *op, const char *arg1, const char *arg2, const char *result) {
    emit_loc(op, arg1, arg2, result, current_line, current_col);
}

char* newtemp() {
    char *temp = (char*)malloc(16);
    if (temp == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(temp, "t%d", temp_var_count++);
    return temp;
}

Node* makelist(int index) {
    Node *node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    node->quad_index = index;
    node->next = NULL;

    if (bp_log_count < MAX_BP_LOGS) {
        strncpy(bp_logs[bp_log_count].event_type, "makelist", 15);
        bp_logs[bp_log_count].from_quad = index;
        bp_logs[bp_log_count].target_quad = -1;
        snprintf(bp_logs[bp_log_count].desc, sizeof(bp_logs[bp_log_count].desc), "makelist(%d)", index);
        bp_log_count++;
    }
    return node;
}

Node* merge(Node *l1, Node *l2) {
    if (!l1) return l2;
    if (!l2) return l1;

    Node *curr = l1;
    while (curr->next)
        curr = curr->next;
    curr->next = l2;

    if (bp_log_count < MAX_BP_LOGS) {
        strncpy(bp_logs[bp_log_count].event_type, "merge", 15);
        bp_logs[bp_log_count].from_quad = l1->quad_index;
        bp_logs[bp_log_count].target_quad = l2->quad_index;
        snprintf(bp_logs[bp_log_count].desc, sizeof(bp_logs[bp_log_count].desc), "merge lists starting at %d and %d", l1->quad_index, l2->quad_index);
        bp_log_count++;
    }
    return l1;
}

void backpatch(Node *list, int target_quad) {
    Node *curr = list;
    while (curr) {
        char target_str[16];
        sprintf(target_str, "%d", target_quad);

        if (bp_log_count < MAX_BP_LOGS) {
            strncpy(bp_logs[bp_log_count].event_type, "backpatch", 15);
            bp_logs[bp_log_count].from_quad = curr->quad_index;
            bp_logs[bp_log_count].target_quad = target_quad;
            snprintf(bp_logs[bp_log_count].desc, sizeof(bp_logs[bp_log_count].desc), "backpatch quad %d -> target L%d", curr->quad_index, target_quad);
            bp_log_count++;
        }

        if (curr->quad_index >= 0 && curr->quad_index < quad_count) {
            strncpy(quad_table[curr->quad_index].result, target_str, 31);
        }
        curr = curr->next;
    }
}

void print_backpatch_logs() {
    printf("\n--- BACKPATCHING OPERATIONS ---\n");
    if (bp_log_count == 0) {
        printf("No backpatching operations recorded.\n");
        return;
    }
    for (int i = 0; i < bp_log_count; i++) {
        if (strcmp(bp_logs[i].event_type, "backpatch") == 0) {
            printf("BACKPATCH: Quad %d -> Quad %d\n", bp_logs[i].from_quad, bp_logs[i].target_quad);
        }
    }
}

void print_tac() {
    printf("\n--- THREE-ADDRESS CODE (TAC) ---\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < quad_count; i++) {
        Quad *q = &quad_table[i];
        printf("L%d:\n", i);
        if (strcmp(q->op, "=") == 0) {
            printf("    %s = %s\n", q->result, q->arg1);
        } else if (strcmp(q->op, "print") == 0) {
            printf("    print %s\n", q->arg1);
        } else if (strcmp(q->op, "print_str") == 0) {
            printf("    print %s\n", q->arg1);
        } else if (strcmp(q->op, "goto") == 0) {
            printf("    goto L%s\n", q->result);
        } else if (strcmp(q->op, "<") == 0 || strcmp(q->op, ">") == 0 ||
                   strcmp(q->op, "<=") == 0 || strcmp(q->op, ">=") == 0 ||
                   strcmp(q->op, "==") == 0 || strcmp(q->op, "!=") == 0) {
            printf("    if %s %s %s goto L%s\n", q->arg1, q->op, q->arg2, q->result);
        } else {
            printf("    %s = %s %s %s\n", q->result, q->arg1, q->op, q->arg2);
        }
    }
    printf("L%d:\n", quad_count);
}

void print_quads() {
    printf("\n--- QUADRUPLE TABLE ---\n");
    printf("%-8s %-8s %-8s %-8s %-12s\n", "Quad #", "Op", "Arg1", "Arg2", "Result");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < quad_count; i++) {
        printf("%-8d %-8s %-8s %-8s %-12s\n",
            i, quad_table[i].op, quad_table[i].arg1, quad_table[i].arg2, quad_table[i].result);
    }
}

typedef struct {
    char name[32];
    int val;
} Variable;

Variable symbol_table[500];
int sym_count = 0;

int get_var(const char *name) {
    if ((name[0] >= '0' && name[0] <= '9') ||
        (name[0] == '-' && name[1] >= '0' && name[1] <= '9')) {
        return atoi(name);
    }
    for (int i = 0; i < sym_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            return symbol_table[i].val;
        }
    }
    return 0;
}

void set_var(const char *name, int val) {
    for (int i = 0; i < sym_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            symbol_table[i].val = val;
            return;
        }
    }
    if (sym_count < 500) {
        strncpy(symbol_table[sym_count].name, name, 31);
        symbol_table[sym_count].val = val;
        sym_count++;
    }
}

void execute_tac() {
    printf("\n--- RUNTIME EXECUTION OUTPUT ---\n");
    int pc = 0;
    int steps = 0;
    while (pc < quad_count && steps < 1000) {
        steps++;
        Quad q = quad_table[pc];
        int next_pc = pc + 1;

        if (strcmp(q.op, "=") == 0) {
            set_var(q.result, get_var(q.arg1));
        } else if (strcmp(q.op, "+") == 0) {
            set_var(q.result, get_var(q.arg1) + get_var(q.arg2));
        } else if (strcmp(q.op, "-") == 0) {
            set_var(q.result, get_var(q.arg1) - get_var(q.arg2));
        } else if (strcmp(q.op, "*") == 0) {
            set_var(q.result, get_var(q.arg1) * get_var(q.arg2));
        } else if (strcmp(q.op, "/") == 0) {
            int denom = get_var(q.arg2);
            if (denom != 0) {
                set_var(q.result, get_var(q.arg1) / denom);
            }
        } else if (strcmp(q.op, "<") == 0) {
            if (get_var(q.arg1) < get_var(q.arg2)) next_pc = atoi(q.result);
        } else if (strcmp(q.op, ">") == 0) {
            if (get_var(q.arg1) > get_var(q.arg2)) next_pc = atoi(q.result);
        } else if (strcmp(q.op, "<=") == 0) {
            if (get_var(q.arg1) <= get_var(q.arg2)) next_pc = atoi(q.result);
        } else if (strcmp(q.op, ">=") == 0) {
            if (get_var(q.arg1) >= get_var(q.arg2)) next_pc = atoi(q.result);
        } else if (strcmp(q.op, "==") == 0) {
            if (get_var(q.arg1) == get_var(q.arg2)) next_pc = atoi(q.result);
        } else if (strcmp(q.op, "!=") == 0) {
            if (get_var(q.arg1) != get_var(q.arg2)) next_pc = atoi(q.result);
        } else if (strcmp(q.op, "goto") == 0) {
            next_pc = atoi(q.result);
        } else if (strcmp(q.op, "print") == 0) {
            printf("OUTPUT: %d\n", get_var(q.arg1));
        } else if (strcmp(q.op, "print_str") == 0) {
            int len = strlen(q.arg1);
            if (len >= 2 && q.arg1[0] == '"' && q.arg1[len - 1] == '"') {
                printf("OUTPUT: %.*s\n", len - 2, q.arg1 + 1);
            } else {
                printf("OUTPUT: %s\n", q.arg1);
            }
        }
        pc = next_pc;
    }

    if (steps >= 1000) {
        printf("\nWARNING: Execution stopped after 1000 TAC steps.\nPossible infinite loop detected.\n");
    }

    printf("\nFinal Memory State (Variables):\n");
    for (int i = 0; i < sym_count; i++) {
        if (symbol_table[i].name[0] != 't') {
            printf("  %s = %d\n", symbol_table[i].name, symbol_table[i].val);
        }
    }
}

/* =========================================================
   ESCAPE HELPER FOR JSON
   ========================================================= */

void print_json_escaped(const char *s) {
    if (!s) return;
    putchar('"');
    for (; *s; s++) {
        switch (*s) {
            case '\\': fputs("\\\\", stdout); break;
            case '"':  fputs("\\\"", stdout); break;
            case '\b': fputs("\\b", stdout); break;
            case '\f': fputs("\\f", stdout); break;
            case '\n': fputs("\\n", stdout); break;
            case '\r': fputs("\\r", stdout); break;
            case '\t': fputs("\\t", stdout); break;
            default:
                if ((unsigned char)*s < ' ') {
                    printf("\\u%04x", (unsigned char)*s);
                } else {
                    putchar(*s);
                }
                break;
        }
    }
    putchar('"');
}

/* =========================================================
   STRUCTURED JSON OUTPUT
   ========================================================= */

void output_json() {
    printf("{\n");
    printf("  \"success\": %s,\n", compilation_failed ? "false" : "true");
    printf("  \"quad_count\": %d,\n", quad_count);

    /* Diagnostics */
    printf("  \"diagnostics\": [\n");
    for (int i = 0; i < diag_count; i++) {
        printf("    { \"severity\": ");
        print_json_escaped(diagnostics[i].severity);
        printf(", \"message\": ");
        print_json_escaped(diagnostics[i].message);
        printf(", \"line\": %d, \"col\": %d }", diagnostics[i].line, diagnostics[i].col);
        if (i < diag_count - 1) printf(",");
        printf("\n");
    }
    printf("  ],\n");

    /* Tokens */
    printf("  \"tokens\": [\n");
    for (int i = 0; i < token_count; i++) {
        printf("    { \"index\": %d, \"type\": ", i);
        print_json_escaped(token_records[i].type);
        printf(", \"lexeme\": ");
        print_json_escaped(token_records[i].lexeme);
        printf(", \"line\": %d, \"col\": %d }", token_records[i].line, token_records[i].col);
        if (i < token_count - 1) printf(",");
        printf("\n");
    }
    printf("  ],\n");

    /* Quadruples */
    printf("  \"quads\": [\n");
    for (int i = 0; i < quad_count; i++) {
        Quad *q = &quad_table[i];
        printf("    { \"id\": %d, \"op\": ", q->id);
        print_json_escaped(q->op);
        printf(", \"arg1\": ");
        print_json_escaped(q->arg1);
        printf(", \"arg2\": ");
        print_json_escaped(q->arg2);
        printf(", \"result\": ");
        print_json_escaped(q->result);
        printf(", \"line\": %d, \"col\": %d }", q->line, q->col);
        if (i < quad_count - 1) printf(",");
        printf("\n");
    }
    printf("  ],\n");

    /* Backpatch Events */
    printf("  \"backpatch_logs\": [\n");
    for (int i = 0; i < bp_log_count; i++) {
        printf("    { \"event\": ");
        print_json_escaped(bp_logs[i].event_type);
        printf(", \"from_quad\": %d, \"target_quad\": %d, \"desc\": ", bp_logs[i].from_quad, bp_logs[i].target_quad);
        print_json_escaped(bp_logs[i].desc);
        printf(" }");
        if (i < bp_log_count - 1) printf(",");
        printf("\n");
    }
    printf("  ]\n");

    printf("}\n");
}

void yyerror(const char *s) {
    add_diagnostic("error", s, current_line, current_col);
    if (!json_output_mode) {
        fprintf(stderr, "COMPILATION ERROR: %s at line %d, col %d\n", s, current_line, current_col);
    }
}

int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--json") == 0) {
            json_output_mode = 1;
        }
    }

    int parse_res = yyparse();
    if (parse_res != 0) {
        compilation_failed = 1;
    }

    if (json_output_mode) {
        output_json();
    } else {
        if (!compilation_failed) {
            print_backpatch_logs();
            print_tac();
            print_quads();
            execute_tac();
        }
    }

    return compilation_failed ? 1 : 0;
}