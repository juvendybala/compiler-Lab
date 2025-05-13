#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ir.h"
#include "symboltable.h"
#include "typechecking.h"

typedef struct Arg Arg;
typedef struct ArgList ArgList;
typedef struct Info Info;

struct Arg
{
    char *name;
    Arg *next;
};

struct ArgList
{
    Arg *head;
};

struct Info
{
    Code *code;
    Type *type;
};

Code *translate_ExtDefList(Node *node);
Code *translate_ExtDef(Node *node);
Code *translate_FunDec(Node *node);
Code *translate_CompSt(Node *node);
Code *translate_VarList(Node *node);
Code *translate_ParamDec(Node *node);
IR *translate_VarDec_func(Node *node);
Code *translate_DefList(Node *node);
Code *translate_Def(Node *node);
Code *translate_DecList(Node *node);
Code *translate_Dec(Node *node);
Code *translate_StmtList(Node *node);
Code *translate_Stmt(Node *node);
Code *translate_Cond(Node *node, char *label_true, char *label_false);
Code *translate_Exp(Node *node, char *place);
Info *translate_array(Node *node, char *place);
Code *translate_Exp_help(Node *node, char *place);
Code *translate_Func(Node *node, char *place);
Code *translate_Args(Node *node, ArgList *arglist);
#endif