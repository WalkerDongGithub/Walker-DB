%option yylineno

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static LPSTSqlNode mallocSqlNode();

%}

number 		[-]?[0-9]+(\.[0-9]+)?
name		[_a-zA-Z][_0-9a-zA-Z]*
variable	($\{[_a-zA-Z][_0-9a-zA-Z()\.]*\}|[_a-zA-Z][_0-9a-zA-Z()\.\*]*|[-]?[0-9]+(\.[0-9]+)?)
whitespace	[ \n\t]
st		(select|SELECT|Select)
fm		(from|FROM|From)
wh		(where|WHERE|Where)
gy		(group[ ]+by|GROUP[ ]+BY|Group[ ]+By)
oy		(order[ ]+by|ORDER[ ]+BY|Order[ ]+By)
terminator	[;]
nsplit		[,]
whsplit		(and|AND|or|OR|Or|And)
define		(AS|as|As|aS)
operator	[<>=|(<=)|(>=)]
lborder		([ ]+\([ ]+)
rborder		([ ]+\)[ ]+)
indId		(\[[a-zA-Z0-9_]{1,24}\])

%%


{indId} {
		yylval.lpStNode = mallocSqlNode();
                yylval.lpStNode->type=INDID;
                snprintf(yylval.lpStNode->text,sizeof(yylval.lpStNode->text),"%s",yytext);
		return INDID;
	}


{st}  {
		yylval.lpStNode = mallocSqlNode();
		yylval.lpStNode->type=ST;
		snprintf(yylval.lpStNode->text,sizeof(yylval.lpStNode->text),"SELECT");
		return ST;
	}

{fm}  	{
		yylval.lpStNode = mallocSqlNode();
		yylval.lpStNode->type=FM;
                snprintf(yylval.lpStNode->text,sizeof(yylval.lpStNode->text),"FROM");
		return FM;
	}

{wh} 	{
		yylval.lpStNode = mallocSqlNode();
		yylval.lpStNode->type=WH;
                snprintf(yylval.lpStNode->text,sizeof(yylval.lpStNode->text),"WHERE");
		return WH;
	}

{gy}  	{
		yylval.lpStNode = mallocSqlNode();
		yylval.lpStNode->type=GY;
                snprintf(yylval.lpStNode->text,sizeof(yylval.lpStNode->text),"GROUP BY");
		return GY;
	}

{oy}	{
		yylval.lpStNode = mallocSqlNode();
		yylval.lpStNode->type=OY;
                snprintf(yylval.lpStNode->text,sizeof(yylval.lpStNode->text),"ORDER BY");
		return OY;
	}

{define}  {
		yylval.lpStNode = mallocSqlNode();
                yylval.lpStNode->type=DEFINE;
                snprintf(yylval.lpStNode->text,sizeof(yylval.lpStNode->text),"AS");
		return DEFINE;
	   }

{whsplit} {
		yylval.lpStNode = mallocSqlNode();
                yylval.lpStNode->type=WHSPLIT;
                snprintf(yylval.lpStNode->text,sizeof(yylval.lpStNode->text),"%s",yytext);
                return WHSPLIT;
	  }

{operator} {
		yylval.lpStNode = mallocSqlNode();
                yylval.lpStNode->type= OPERATOR;
                snprintf(yylval.lpStNode->text,sizeof(yylval.lpStNode->text),"%s",yytext);
		return OPERATOR;
	   }

{nsplit}	{
			yylval.lpStNode = mallocSqlNode();
                        yylval.lpStNode->type= NSPLIT;
                        snprintf(yylval.lpStNode->text,sizeof(yylval.lpStNode->text),"%s",yytext);
			return NSPLIT;
		}
{lborder}	{
			yylval.lpStNode = mallocSqlNode();
                	yylval.lpStNode->type= LBORDER;
                	snprintf(yylval.lpStNode->text,sizeof(yylval.lpStNode->text),"%s",yytext);
			return LBORDER;
		}

{rborder}	{
			yylval.lpStNode = mallocSqlNode();
                        yylval.lpStNode->type= RBORDER;
                        snprintf(yylval.lpStNode->text,sizeof(yylval.lpStNode->text),"%s",yytext);
			return RBORDER;
		}

{variable} {
		yylval.lpStNode = mallocSqlNode();
                yylval.lpStNode->type=VARIABLE;
                snprintf(yylval.lpStNode->text,sizeof(yylval.lpStNode->text),"%s",yytext);
		return VARIABLE;
	   }

{name}	{
		yylval.lpStNode = mallocSqlNode();
		yylval.lpStNode->type=NAME;
		snprintf(yylval.lpStNode->text,sizeof(yylval.lpStNode->text),"%s",yytext);
		return NAME;
	}

{number} {
		yylval.lpStNode = mallocSqlNode();
		yylval.lpStNode->type=NUMBER;
		yylval.lpStNode->dVal = strtod(yytext,NULL);
		return NUMBER;
	 }

{terminator}	{
			return TERMINATOR;
		}

{whitespace}	;
%%

int yywrap()
{
        return 1;
}



static LPSTSqlNode mallocSqlNode()
{
	LPSTSqlNode lpTemp = NULL;
	lpTemp = (LPSTSqlNode)malloc(sizeof(STSqlNode));
	lpTemp->lpNext = NULL;
        lpTemp->lpSub = NULL;
	return lpTemp;
}

int initBufferSql(char lpStrSql[],char indId[])
{
	char sql[2048];
	memset(sql,0x00,sizeof(sql));

	char *lpTemp = NULL;
	lpTemp = strtok(lpStrSql,";");
	int ret = 0;
	if(lpTemp != NULL){
		snprintf(sql,sizeof(sql),"%s [%s];",lpTemp,indId);
		YY_BUFFER_STATE yyBuf = yy_scan_string(sql);
        	yy_switch_to_buffer( yyBuf );
		ret = yyparse();
        	yy_delete_buffer(yyBuf);
	}else{
		return -1;
	}

	return ret;
}