%{
#include "Lexer.h"
#include "tree/SqlTree.h"
#include "walker_sql.tab.hpp"
#include <iostream>
using namespace std;

    extern "C"				//为了能够在C++程序里面调用C函数，必须把每一个需要使用的C函数，其声明都包括在extern "C"{}块里面，这样C++链接时才能成功链接它们。extern "C"用来在C++环境下设置C链接类型。
    {						//yacc.y中也有类似的这段extern "C"，可以把它们合并成一段，放到共同的头文件main.h中
        int yywrap(void);
        int yylex(void);	//这个是lex生成的词法分析函数，yacc的yyparse()里会调用它，如果这里不声明，生成的yacc.tab.c在编译时会找不到该函数
    };

%}


%%
select                 { putchar(yytext[0]); return SELECT; }
from                   { putchar(yytext[0]); return FROM; }
where                  { putchar(yytext[0]); return WHERE; }
[-]?[0-9]+(\.[0-9]+)?  { putchar(yytext[0]); return NUMBER; }
[_a-zA-Z][_0-9a-zA-Z]* { putchar(yytext[0]); return IDENTIFIER; }
.                      { return yytext[0]; }

%%

void run() {
    yylex();
}
int i;
int yywrap()
{
    if (i < 10)
        return 0;
    else
        return 1;
}


