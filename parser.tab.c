/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

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


#line 201 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ID = 3,                         /* ID  */
  YYSYMBOL_NUM = 4,                        /* NUM  */
  YYSYMBOL_STRING_LITERAL = 5,             /* STRING_LITERAL  */
  YYSYMBOL_WHILE = 6,                      /* WHILE  */
  YYSYMBOL_FOR = 7,                        /* FOR  */
  YYSYMBOL_IF = 8,                         /* IF  */
  YYSYMBOL_ELSE = 9,                       /* ELSE  */
  YYSYMBOL_PRINT = 10,                     /* PRINT  */
  YYSYMBOL_LE = 11,                        /* LE  */
  YYSYMBOL_GE = 12,                        /* GE  */
  YYSYMBOL_EQ = 13,                        /* EQ  */
  YYSYMBOL_NE = 14,                        /* NE  */
  YYSYMBOL_LT = 15,                        /* LT  */
  YYSYMBOL_GT = 16,                        /* GT  */
  YYSYMBOL_ASSIGN = 17,                    /* ASSIGN  */
  YYSYMBOL_PLUS = 18,                      /* PLUS  */
  YYSYMBOL_MINUS = 19,                     /* MINUS  */
  YYSYMBOL_MULT = 20,                      /* MULT  */
  YYSYMBOL_DIV = 21,                       /* DIV  */
  YYSYMBOL_SEMI = 22,                      /* SEMI  */
  YYSYMBOL_LPAREN = 23,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 24,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 25,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 26,                    /* RBRACE  */
  YYSYMBOL_YYACCEPT = 27,                  /* $accept  */
  YYSYMBOL_PROG = 28,                      /* PROG  */
  YYSYMBOL_S_LIST = 29,                    /* S_LIST  */
  YYSYMBOL_M = 30,                         /* M  */
  YYSYMBOL_N = 31,                         /* N  */
  YYSYMBOL_FOR_INIT = 32,                  /* FOR_INIT  */
  YYSYMBOL_FOR_STEP = 33,                  /* FOR_STEP  */
  YYSYMBOL_S = 34,                         /* S  */
  YYSYMBOL_35_1 = 35,                      /* $@1  */
  YYSYMBOL_C = 36,                         /* C  */
  YYSYMBOL_E = 37                          /* E  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  16
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   120

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  27
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  11
/* YYNRULES -- Number of rules.  */
#define YYNRULES  31
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  96

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   281


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   163,   163,   173,   178,   186,   193,   200,   207,   214,
     220,   226,   232,   238,   246,   257,   256,   272,   279,   285,
     293,   301,   309,   317,   325,   335,   340,   347,   354,   361,
     368,   373
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ID", "NUM",
  "STRING_LITERAL", "WHILE", "FOR", "IF", "ELSE", "PRINT", "LE", "GE",
  "EQ", "NE", "LT", "GT", "ASSIGN", "PLUS", "MINUS", "MULT", "DIV", "SEMI",
  "LPAREN", "RPAREN", "LBRACE", "RBRACE", "$accept", "PROG", "S_LIST", "M",
  "N", "FOR_INIT", "FOR_STEP", "S", "$@1", "C", "E", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-17)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-3)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       9,   -15,   -13,   -12,    -9,     8,     9,    25,    46,   -17,
       1,   -17,     5,    32,    45,    30,   -17,     9,   -17,   -17,
       1,    51,    32,    57,    36,    32,    39,    89,    54,    33,
     -17,   -17,    41,     1,     1,     1,     1,   -17,    55,     1,
     -17,    56,    75,   -17,     1,     1,     1,     1,     1,     1,
      59,    60,   -17,    24,    24,   -17,   -17,   -17,     2,    32,
     -17,    58,     2,     2,     2,     2,     2,     2,   -17,   -17,
      67,    76,     9,     9,   -17,    71,    80,   108,   103,   -17,
      96,   -17,   -17,     1,    90,   -17,     2,   -17,    91,    92,
       9,     9,    93,    94,   -17,   -17
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     5,     4,
       0,     5,     0,     0,     0,     5,     1,     0,    30,    31,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      17,     3,     0,     0,     0,     0,     0,     9,     0,     0,
       5,     0,     0,     5,     0,     0,     0,     0,     0,     0,
       0,     0,    25,    26,    27,    28,    29,     5,     7,     0,
      18,     0,    21,    22,    23,    24,    19,    20,    11,    10,
       0,     0,     0,     0,     5,     5,     5,     0,    12,    14,
       0,    15,     6,     0,     0,     5,     8,     5,     0,     0,
       0,     0,     5,     5,    13,    16
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -17,   -17,    -6,   -10,   -17,   -17,   -17,    98,   -17,   -16,
      -7
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     7,     8,    17,    85,    24,    81,     9,    84,    26,
      27
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      15,    22,    10,    21,    18,    19,    38,    29,    23,    41,
      11,    12,     1,    32,    13,     2,     3,     4,    42,     5,
      33,    34,    35,    36,    20,    16,    53,    54,    55,    56,
      59,    14,    58,    61,     6,    18,    19,    62,    63,    64,
      65,    66,    67,    71,    35,    36,    -2,    70,    18,    19,
      28,    33,    34,    35,    36,    25,    30,    51,    40,    33,
      34,    35,    36,    43,    77,    52,    75,    76,    20,    33,
      34,    35,    36,    37,    39,    88,    86,    89,    50,    57,
      60,    68,    69,    72,    92,    93,    44,    45,    46,    47,
      48,    49,    73,    33,    34,    35,    36,    78,    74,    52,
      44,    45,    46,    47,    48,    49,    79,    33,    34,    35,
      36,    80,    82,    83,    87,    31,    90,    91,     0,    94,
      95
};

static const yytype_int8 yycheck[] =
{
       6,    11,    17,    10,     3,     4,    22,    14,     3,    25,
      23,    23,     3,    20,    23,     6,     7,     8,    25,    10,
      18,    19,    20,    21,    23,     0,    33,    34,    35,    36,
      40,    23,    39,    43,    25,     3,     4,    44,    45,    46,
      47,    48,    49,    59,    20,    21,     0,    57,     3,     4,
       5,    18,    19,    20,    21,    23,    26,    24,    22,    18,
      19,    20,    21,    24,    74,    24,    72,    73,    23,    18,
      19,    20,    21,    22,    17,    85,    83,    87,    24,    24,
      24,    22,    22,    25,    90,    91,    11,    12,    13,    14,
      15,    16,    25,    18,    19,    20,    21,    26,    22,    24,
      11,    12,    13,    14,    15,    16,    26,    18,    19,    20,
      21,     3,     9,    17,    24,    17,    25,    25,    -1,    26,
      26
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     6,     7,     8,    10,    25,    28,    29,    34,
      17,    23,    23,    23,    23,    29,     0,    30,     3,     4,
      23,    37,    30,     3,    32,    23,    36,    37,     5,    37,
      26,    34,    37,    18,    19,    20,    21,    22,    36,    17,
      22,    36,    37,    24,    11,    12,    13,    14,    15,    16,
      24,    24,    24,    37,    37,    37,    37,    24,    37,    30,
      24,    30,    37,    37,    37,    37,    37,    37,    22,    22,
      30,    36,    25,    25,    22,    29,    29,    30,    26,    26,
       3,    33,     9,    17,    35,    31,    37,    24,    30,    30,
      25,    25,    29,    29,    26,    26
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    27,    28,    29,    29,    30,    31,    32,    33,    34,
      34,    34,    34,    34,    34,    35,    34,    34,    36,    36,
      36,    36,    36,    36,    36,    37,    37,    37,    37,    37,
      37,    37
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     3,     1,     0,     0,     3,     3,     4,
       5,     5,     8,    14,     9,     0,    15,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* PROG: S_LIST  */
#line 164 "parser.y"
      {
          backpatch((yyvsp[0].stmt).nextlist, quad_count);
          if (!json_output_mode) {
              printf("\n--- GENERATING INTERMEDIATE CODE ---\n");
          }
      }
#line 1400 "parser.tab.c"
    break;

  case 3: /* S_LIST: S_LIST M S  */
#line 174 "parser.y"
      {
          backpatch((yyvsp[-2].stmt).nextlist, (yyvsp[-1].quad_num));
          (yyval.stmt).nextlist = (yyvsp[0].stmt).nextlist;
      }
#line 1409 "parser.tab.c"
    break;

  case 4: /* S_LIST: S  */
#line 179 "parser.y"
      {
          (yyval.stmt).nextlist = (yyvsp[0].stmt).nextlist;
      }
#line 1417 "parser.tab.c"
    break;

  case 5: /* M: %empty  */
#line 186 "parser.y"
      {
          (yyval.quad_num) = nextquad();
      }
#line 1425 "parser.tab.c"
    break;

  case 6: /* N: %empty  */
#line 193 "parser.y"
      {
          (yyval.stmt).nextlist = makelist(nextquad());
          emit_loc("goto", "", "", "_", (yyloc).first_line, (yyloc).first_column);
      }
#line 1434 "parser.tab.c"
    break;

  case 7: /* FOR_INIT: ID ASSIGN E  */
#line 201 "parser.y"
      {
          emit_loc("=", (yyvsp[0].str), "", (yyvsp[-2].str), (yylsp[-2]).first_line, (yylsp[-2]).first_column);
      }
#line 1442 "parser.tab.c"
    break;

  case 8: /* FOR_STEP: ID ASSIGN E  */
#line 208 "parser.y"
      {
          emit_loc("=", (yyvsp[0].str), "", (yyvsp[-2].str), (yylsp[-2]).first_line, (yylsp[-2]).first_column);
      }
#line 1450 "parser.tab.c"
    break;

  case 9: /* S: ID ASSIGN E SEMI  */
#line 215 "parser.y"
      {
          emit_loc("=", (yyvsp[-1].str), "", (yyvsp[-3].str), (yylsp[-3]).first_line, (yylsp[-3]).first_column);
          (yyval.stmt).nextlist = NULL;
      }
#line 1459 "parser.tab.c"
    break;

  case 10: /* S: PRINT LPAREN E RPAREN SEMI  */
#line 221 "parser.y"
      {
          emit_loc("print", (yyvsp[-2].str), "", "", (yylsp[-4]).first_line, (yylsp[-4]).first_column);
          (yyval.stmt).nextlist = NULL;
      }
#line 1468 "parser.tab.c"
    break;

  case 11: /* S: PRINT LPAREN STRING_LITERAL RPAREN SEMI  */
#line 227 "parser.y"
      {
          emit_loc("print_str", (yyvsp[-2].str), "", "", (yylsp[-4]).first_line, (yylsp[-4]).first_column);
          (yyval.stmt).nextlist = NULL;
      }
#line 1477 "parser.tab.c"
    break;

  case 12: /* S: IF LPAREN C RPAREN M LBRACE S_LIST RBRACE  */
#line 233 "parser.y"
      {
          backpatch((yyvsp[-5].expr).truelist, (yyvsp[-3].quad_num));
          (yyval.stmt).nextlist = merge((yyvsp[-5].expr).falselist, (yyvsp[-1].stmt).nextlist);
      }
#line 1486 "parser.tab.c"
    break;

  case 13: /* S: IF LPAREN C RPAREN M LBRACE S_LIST RBRACE ELSE N M LBRACE S_LIST RBRACE  */
#line 239 "parser.y"
      {
          backpatch((yyvsp[-11].expr).truelist, (yyvsp[-9].quad_num));
          backpatch((yyvsp[-11].expr).falselist, (yyvsp[-3].quad_num));
          Node *temp = merge((yyvsp[-7].stmt).nextlist, (yyvsp[-4].stmt).nextlist);
          (yyval.stmt).nextlist = merge(temp, (yyvsp[-1].stmt).nextlist);
      }
#line 1497 "parser.tab.c"
    break;

  case 14: /* S: WHILE LPAREN M C RPAREN M LBRACE S_LIST RBRACE  */
#line 247 "parser.y"
      {          
          backpatch((yyvsp[-5].expr).truelist, (yyvsp[-3].quad_num));
          char loop_target[16];
          sprintf(loop_target, "%d", (yyvsp[-6].quad_num));
          emit_loc("goto", "", "", loop_target, (yylsp[-8]).first_line, (yylsp[-8]).first_column);
          backpatch((yyvsp[-1].stmt).nextlist, (yyvsp[-6].quad_num));
          (yyval.stmt).nextlist = (yyvsp[-5].expr).falselist;
      }
#line 1510 "parser.tab.c"
    break;

  case 15: /* $@1: %empty  */
#line 257 "parser.y"
      {
          char cond_target[16];
          sprintf(cond_target, "%d", (yyvsp[-4].quad_num));
          emit_loc("goto", "", "", cond_target, (yylsp[-8]).first_line, (yylsp[-8]).first_column);
      }
#line 1520 "parser.tab.c"
    break;

  case 16: /* S: FOR LPAREN FOR_INIT SEMI M C SEMI M FOR_STEP $@1 RPAREN M LBRACE S_LIST RBRACE  */
#line 263 "parser.y"
      {
          backpatch((yyvsp[-9].expr).truelist, (yyvsp[-3].quad_num));
          backpatch((yyvsp[-1].stmt).nextlist, (yyvsp[-7].quad_num));
          char step_target[16];
          sprintf(step_target, "%d", (yyvsp[-7].quad_num));
          emit_loc("goto", "", "", step_target, (yylsp[-14]).first_line, (yylsp[-14]).first_column);
          (yyval.stmt).nextlist = (yyvsp[-9].expr).falselist;
      }
#line 1533 "parser.tab.c"
    break;

  case 17: /* S: LBRACE S_LIST RBRACE  */
#line 273 "parser.y"
      {
          (yyval.stmt).nextlist = (yyvsp[-1].stmt).nextlist;
      }
#line 1541 "parser.tab.c"
    break;

  case 18: /* C: LPAREN C RPAREN  */
#line 280 "parser.y"
      {
          (yyval.expr).truelist = (yyvsp[-1].expr).truelist;
          (yyval.expr).falselist = (yyvsp[-1].expr).falselist;
      }
#line 1550 "parser.tab.c"
    break;

  case 19: /* C: E LT E  */
#line 286 "parser.y"
      {
          (yyval.expr).truelist = makelist(nextquad());
          (yyval.expr).falselist = makelist(nextquad() + 1);
          emit_loc("<", (yyvsp[-2].str), (yyvsp[0].str), "_", (yylsp[-1]).first_line, (yylsp[-1]).first_column);
          emit_loc("goto", "", "", "_", (yylsp[-1]).first_line, (yylsp[-1]).first_column);
      }
#line 1561 "parser.tab.c"
    break;

  case 20: /* C: E GT E  */
#line 294 "parser.y"
      {
          (yyval.expr).truelist = makelist(nextquad());
          (yyval.expr).falselist = makelist(nextquad() + 1);
          emit_loc(">", (yyvsp[-2].str), (yyvsp[0].str), "_", (yylsp[-1]).first_line, (yylsp[-1]).first_column);
          emit_loc("goto", "", "", "_", (yylsp[-1]).first_line, (yylsp[-1]).first_column);
      }
#line 1572 "parser.tab.c"
    break;

  case 21: /* C: E LE E  */
#line 302 "parser.y"
      {
          (yyval.expr).truelist = makelist(nextquad());
          (yyval.expr).falselist = makelist(nextquad() + 1);
          emit_loc("<=", (yyvsp[-2].str), (yyvsp[0].str), "_", (yylsp[-1]).first_line, (yylsp[-1]).first_column);
          emit_loc("goto", "", "", "_", (yylsp[-1]).first_line, (yylsp[-1]).first_column);
      }
#line 1583 "parser.tab.c"
    break;

  case 22: /* C: E GE E  */
#line 310 "parser.y"
      {
          (yyval.expr).truelist = makelist(nextquad());
          (yyval.expr).falselist = makelist(nextquad() + 1);
          emit_loc(">=", (yyvsp[-2].str), (yyvsp[0].str), "_", (yylsp[-1]).first_line, (yylsp[-1]).first_column);
          emit_loc("goto", "", "", "_", (yylsp[-1]).first_line, (yylsp[-1]).first_column);
      }
#line 1594 "parser.tab.c"
    break;

  case 23: /* C: E EQ E  */
#line 318 "parser.y"
      {
          (yyval.expr).truelist = makelist(nextquad());
          (yyval.expr).falselist = makelist(nextquad() + 1);
          emit_loc("==", (yyvsp[-2].str), (yyvsp[0].str), "_", (yylsp[-1]).first_line, (yylsp[-1]).first_column);
          emit_loc("goto", "", "", "_", (yylsp[-1]).first_line, (yylsp[-1]).first_column);
      }
#line 1605 "parser.tab.c"
    break;

  case 24: /* C: E NE E  */
#line 326 "parser.y"
      {
          (yyval.expr).truelist = makelist(nextquad());
          (yyval.expr).falselist = makelist(nextquad() + 1);
          emit_loc("!=", (yyvsp[-2].str), (yyvsp[0].str), "_", (yylsp[-1]).first_line, (yylsp[-1]).first_column);
          emit_loc("goto", "", "", "_", (yylsp[-1]).first_line, (yylsp[-1]).first_column);
      }
#line 1616 "parser.tab.c"
    break;

  case 25: /* E: LPAREN E RPAREN  */
#line 336 "parser.y"
      {
          (yyval.str) = (yyvsp[-1].str);
      }
#line 1624 "parser.tab.c"
    break;

  case 26: /* E: E PLUS E  */
#line 341 "parser.y"
      {
          char *temp = newtemp();
          emit_loc("+", (yyvsp[-2].str), (yyvsp[0].str), temp, (yylsp[-1]).first_line, (yylsp[-1]).first_column);
          (yyval.str) = temp;
      }
#line 1634 "parser.tab.c"
    break;

  case 27: /* E: E MINUS E  */
#line 348 "parser.y"
      {
          char *temp = newtemp();
          emit_loc("-", (yyvsp[-2].str), (yyvsp[0].str), temp, (yylsp[-1]).first_line, (yylsp[-1]).first_column);
          (yyval.str) = temp;
      }
#line 1644 "parser.tab.c"
    break;

  case 28: /* E: E MULT E  */
#line 355 "parser.y"
      {
          char *temp = newtemp();
          emit_loc("*", (yyvsp[-2].str), (yyvsp[0].str), temp, (yylsp[-1]).first_line, (yylsp[-1]).first_column);
          (yyval.str) = temp;
      }
#line 1654 "parser.tab.c"
    break;

  case 29: /* E: E DIV E  */
#line 362 "parser.y"
      {
          char *temp = newtemp();
          emit_loc("/", (yyvsp[-2].str), (yyvsp[0].str), temp, (yylsp[-1]).first_line, (yylsp[-1]).first_column);
          (yyval.str) = temp;
      }
#line 1664 "parser.tab.c"
    break;

  case 30: /* E: ID  */
#line 369 "parser.y"
      {
          (yyval.str) = (yyvsp[0].str);
      }
#line 1672 "parser.tab.c"
    break;

  case 31: /* E: NUM  */
#line 374 "parser.y"
      {
          (yyval.str) = (yyvsp[0].str);
      }
#line 1680 "parser.tab.c"
    break;


#line 1684 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 379 "parser.y"


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
