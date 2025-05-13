#ifndef TYPECHECKING_H
#define TYPECHECKING_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "ast.h"
#include "symboltable.h"

extern int has_struct;

void add_func();
void check_name(Node *node, const char *name);
void ExtDefList(Node *node);
void ExtDef(Node *node);
Type *Specifier(Node *node);
Type *StructSpecifier(Node *node);
void DefList_struct(Node *node, Type *struc);
void Def_struct(Node *node, Type *struc);
void DecList_struct(Node *node, Type *struc, Type *type);
void Dec_struct(Node *node, Type *struc, Type *type);
void VarDec_struct(Node *node, Type *struc, Type *type);
void ExtDecList(Node *node, Type *type);
void VarDec_var(Node *node, Type *type);
FuncType *FunDec(Node *node, Type *rtype);
void VarList(Node *node, FuncType *func);
void ParamDec(Node *node, FuncType *func);
void VarDec_func(Node *node, Type *type, FuncType *func);
void VarDec(Node *node, Type *type, Type *struc, FuncType *func, int kind);
void CompSt(Node *node, FuncType *func);
void DefList(Node *node);
void Def(Node *node);
void DecList(Node *node, Type *type);
void Dec(Node *node, Type *type);
void StmtList(Node *node, FuncType *func);
void Stmt(Node *node, FuncType *func);
Type *Exp(Node *node);
void Args(Node *node, FuncType *func);
#endif