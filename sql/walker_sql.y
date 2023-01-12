%{
#include "../sql/tree/SqlTree.h"
#include <iostream>
using namespace std;

    extern "C"
        {
            int yyparse(void);
            extern int yylex(void);
            void yyerror(const char *s);
        };


%}

%union {
	TreeNode node;
}

%type <node>  EXPRESSION
%type <node>  COLUMN_NAME
%type <node>  SELECT_EXPRESSION TABLE TABLE_JOIN_EXPRESSION
%type <node>  PROJECTION PROJECTION_ELEMENT

%token <node> IDENTIFIER
%token <node> SELECT FROM WHERE
%token <node> NUMBER
%token <node> TRUE FALSE

%%

EXPRESSION: SELECT_EXPRESSION

SELECT_EXPRESSION: SELECT PROJECTION FROM TABLE

PROJECTION: PROJECTION_ELEMENT ',' PROJECTION
	  | PROJECTION_ELEMENT

PROJECTION_ELEMENT: COLUMN_NAME | IDENTIFIER '(' COLUMN_NAME ')'

TABLE: SELECT_EXPRESSION | TABLE_JOIN_EXPRESSION

TABLE_JOIN_EXPRESSION: IDENTIFIER

WHERE_EXPRESSION: TRUE | FALSE

COLUMN_NAME: '*' | IDENTIFIER | IDENTIFIER '.' IDENTIFIER

%%

