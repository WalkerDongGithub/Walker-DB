%{
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "alsstruct.h"
#include "dbformula.h"
#include "odbcapi.h"


#define  OPT_BROTHER		1
#define	 OPT_SON		2
#define	 GROUP_BY_PREFIX	"__wjq__"


extern int yylineno;
extern pthread_key_t _theSelfConnKey;


static LPSTSqlNode oper(int optType,int optTotal,...);
static void	 display(LPSTSqlNode lpNode,char lpSql[],char lpGyName[],char lpGyIndex[]);
static void	 freeSqlNode(LPSTSqlNode lpNode);
int yylex(void);
%}

%union {
        int nVal;
        LPSTSqlNode lpStNode;
}

%token TERMINATOR
%token <lpStNode>ST FM WH GY OY NAME NUMBER WHSPLIT VARIABLE OPERATOR LBORDER RBORDER NSPLIT DEFINE INDID
%type <lpStNode>stmt name_list st_list fm_list oy_list gy_list whname whname_list wh_list


%%
program:
	program stmt INDID TERMINATOR {
			char sql[2048];
			char gyColList[256];
			char gyIndexList[128];

			memset(sql,0x00,sizeof(sql));
			memset(gyColList,0x00,sizeof(gyColList));
			display($2,sql,gyColList,gyIndexList);
			LPSTSqlNode lp = $3;

			char *lpSplit ="[]";

			char* lpIndex = strtok(lp->text,lpSplit);
			STIndexDef indexDef;
			snprintf(indexDef.indId,sizeof(indexDef.indId),"%s",lpIndex);
			snprintf(indexDef.indAslDef,sizeof(indexDef.indAslDef),"%s",sql);
			snprintf(indexDef.indColName,sizeof(indexDef.indColName),"%s",gyColList);

			LPSTODBCHandle lpHandle = (LPSTODBCHandle)pthread_getspecific(_theSelfConnKey);
			updateIndexDef(lpHandle,&indexDef);

			//printf("sql : [%s]\n",sql);
			//printf("Group By : [%s]\n",gyColList);
			//printf("index ID : [%s]\n",lp->text);
			freeSqlNode($2);
			freeSqlNode($3);
		     }
	|
	;

stmt:
     	 st_list fm_list             			{$$=oper(OPT_BROTHER,2,$1,$2);}
	|st_list fm_list oy_list			{$$=oper(OPT_BROTHER,3,$1,$2,$3);}
	|st_list fm_list gy_list			{$$=oper(OPT_BROTHER,3,$1,$2,$3);}
	|st_list fm_list wh_list			{$$=oper(OPT_BROTHER,3,$1,$2,$3);}
	|st_list fm_list wh_list gy_list		{$$=oper(OPT_BROTHER,4,$1,$2,$3,$4);}
	|st_list fm_list gy_list oy_list		{$$=oper(OPT_BROTHER,4,$1,$2,$3,$4);}
	|st_list fm_list wh_list oy_list		{$$=oper(OPT_BROTHER,4,$1,$2,$3,$4);}
	|st_list fm_list wh_list gy_list oy_list	{$$=oper(OPT_BROTHER,5,$1,$2,$3,$4,$5);}
	;

st_list:
	ST name_list			{$$=oper(OPT_SON,2,$1,$2);}
	;
fm_list:
	FM NAME				{$$=oper(OPT_SON,2,$1,$2);}
	|FM LBORDER stmt RBORDER	{$$=oper(OPT_SON,4,$1,$2,$3,$4);}
	|fm_list NSPLIT NAME		{$$=oper(OPT_SON,3,$1,$2,$3);}
	|fm_list NSPLIT LBORDER stmt RBORDER {$$=oper(OPT_SON,5,$1,$2,$3,$4,$5);}
	|fm_list DEFINE NAME		{$$=oper(OPT_SON,3,$1,$2,$3);}
	;

oy_list:
	OY name_list			{$$=oper(OPT_SON,2,$1,$2);}
	;
gy_list:
	GY name_list			{$$=oper(OPT_SON,2,$1,$2);}
	;
wh_list:
        WH whname_list                  {$$=oper(OPT_SON,2,$1,$2);}
        ;

name_list:
	 NAME				{$$=oper(OPT_BROTHER,1,$1);}
	|name_list DEFINE NAME		{$$=oper(OPT_BROTHER,3,$1,$2,$3);}
	|name_list NSPLIT NAME		{$$=oper(OPT_BROTHER,3,$1,$2,$3);}
	;

whname_list:
	whname				{$$=oper(OPT_BROTHER,1,$1);}
	|whname_list WHSPLIT whname	{$$=oper(OPT_BROTHER,3,$1,$2,$3);}
	;

whname:
	NAME OPERATOR VARIABLE          {$$=oper(OPT_BROTHER,3,$1,$2,$3);}
	;

%%

void yyerror(char *s)
{
	printf("sqlAls ERROR : [%s], line : [%d]\n",s,yylineno);
}



static void freeSqlNode(LPSTSqlNode lpNode)
{
	if(lpNode == NULL) return ;
	freeSqlNode(lpNode->lpSub);
	freeSqlNode(lpNode->lpNext);
	free(lpNode);
}

void traversalSyntaxTree(LPSTSqlNode lpNode,char lpStr[])
{
	if(lpNode == NULL) return ;

	strcat(lpStr,lpNode->text);
        strcat(lpStr," ");
	//printf("%s ",lpNode->text);
	traversalSyntaxTree(lpNode->lpSub,lpStr);
	traversalSyntaxTree(lpNode->lpNext,lpStr);

}


LPSTSqlNode mallocSplit()
{
	LPSTSqlNode lp = NULL;
	lp = (LPSTSqlNode)malloc(sizeof(STSqlNode));
        lp->type   = NSPLIT;
        lp->lpNext = NULL;
        lp->lpSub  = NULL;
	snprintf(lp->text,sizeof(lp->text),",");
	return lp;
}


static void     display(LPSTSqlNode lpNode,char lpSql[],char lpGyName[],char lpGyIndex[])
{
	if(lpNode == NULL) return ;

	LPSTSqlNode lpSel = NULL;
	LPSTSqlNode lpGy  = NULL;
	LPSTSqlNode lpTemp = NULL;

	lpTemp = lpNode;

	while(lpTemp != NULL){
		if(lpTemp->type == ST ){
			lpSel = lpTemp->lpSub;
			while(lpSel->lpNext != NULL ){
				lpSel = lpSel->lpNext;
			}
		}

		if(lpTemp->type == GY){
			lpGy = lpTemp;
		}
		lpTemp = lpTemp->lpNext;
	}

	int isFirst = 1;
	if( lpGy != NULL){
		lpTemp = lpGy->lpSub;
		LPSTSqlNode lp = NULL;
		while( lpTemp != NULL){
			if(lpTemp->type != NSPLIT && lpTemp->type != DEFINE ){
				lp = (LPSTSqlNode)malloc(sizeof(STSqlNode));
				lp->type   = lpTemp->type;
				lp->lpNext = NULL;
				lp->lpSub  = NULL;
				lpSel->lpNext = mallocSplit();
				lpSel = lpSel->lpNext;

				lpSel->lpNext = lp;
				lpSel = lp;
				snprintf(lp->text,sizeof(lp->text),"%s AS %s%s",lpTemp->text,lpTemp->text,GROUP_BY_PREFIX);
				if(isFirst == 1){
					isFirst = 0;
				}else{
					strcat(lpGyName,",");
				}
				strcat(lpGyName,lpTemp->text);
				strcat(lpGyName,GROUP_BY_PREFIX);
			}

			lpTemp = lpTemp->lpNext;
        	}
	}

	if(lpSql != NULL)
		traversalSyntaxTree(lpNode,lpSql);

}


static LPSTSqlNode oper(int optType,int optTotal,...)
{
	va_list paraList;
	LPSTSqlNode lpTemp = NULL;
	LPSTSqlNode lpFirst = NULL;
	LPSTSqlNode lpPre	= NULL;
	int tag = 1;

	va_start(paraList,optTotal);

	if(OPT_BROTHER == optType){
		while(optTotal > 0){
			lpTemp = va_arg(paraList,LPSTSqlNode);
			--optTotal;

			if(tag == 1){
				lpPre = lpFirst = lpTemp;
				tag = 0;
			}else{
				lpPre->lpNext = lpTemp;
			}

			while(lpPre->lpNext != NULL){
                                        lpPre = lpPre->lpNext;
                        }
		//	printf("oper : [%s] [%d]\n",lpTemp->text,optType);
		}

	}else if(OPT_SON == optType){
		lpFirst = va_arg(paraList,LPSTSqlNode);

		while(optTotal > 1){
			lpTemp = va_arg(paraList,LPSTSqlNode);
                        --optTotal;

                        if(tag == 1){
                                lpPre = lpTemp;
				if(lpFirst->lpSub == NULL){
					lpFirst->lpSub = lpPre;
				}else{
					lpTemp = lpFirst->lpSub;
					while(lpTemp->lpNext != NULL){
                                        	lpTemp = lpTemp->lpNext;
                        		}
					lpTemp->lpNext = lpPre;
				}
                                tag = 0;
                        }else{
                                lpPre->lpNext = lpTemp;
                        }

			while(lpPre->lpNext != NULL){
                                        lpPre = lpPre->lpNext;
                        }
		//	printf("oper : [%s] [%d]\n",lpTemp->text,optType);
		}
	}

	va_end(paraList);

	return lpFirst;
}

/*
int main()
{

	yyparse();
	return 0;

}*/