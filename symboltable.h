#ifndef SYMBOLETABLE_H
#define SYMBOLETABLE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Type Type;
typedef struct FieldList FieldList;
typedef struct Var_or_Struct Var_or_Struct;
typedef struct FuncArg FuncArg;
typedef struct FuncType FuncType;

struct Type
{
    int kind; // 0代表基本类型，1代表数组，2代表结构体
    union
    {
        int basic; // 0代表int，1代表浮点数
        struct
        {
            Type *elem;
            int size;
            int elem_size;
        } array;
        FieldList *structure;
    } u;
};

struct FieldList
{
    char *name;
    Type *type;
    FieldList *next;
};

struct Var_or_Struct
{
    int kind;     // 0代表变量，1代表结构体
    int is_param; // 0表示为形参，1表示不是
    int offset;   // 反映该变量在当前函数中的偏移位置，从原sp开始
    char *name;
    Type *type;
    Var_or_Struct *next; // 用于构成hash表中的链表
};

struct FuncArg
{
    Type *type;
    FuncArg *next;
};

struct FuncType
{
    char *name;
    int argc;       // 表示函数参数个数
    FuncArg *argv;  // 表示函数参数链表
    Type *rtype;    // 表示返回值的类型
    FuncType *next; // 用于构成hash表中的链表
};

unsigned int hash_func(char *name);

Var_or_Struct *get_Var(char *name, Type *type);
Var_or_Struct *get_Struct(char *name, Type *type);

Type *get_int();
Type *get_float();

Type *get_array(Type *right, int size, int elem_size);

Type *get_struct();
FieldList *get_FieldList(char *name, Type *type);
int addFieldList2struct(Type *type, FieldList *field);

FuncType *get_func(char *name, Type *rtype);
FuncArg *get_funcarg(Type *type);
int addFuncArg2func(FuncType *func, FuncArg *arg);

int is_same_type(Type *type1, Type *type2);
int is_same_function(FuncType *func1, FuncType *func2);

void insert_Vartable(Var_or_Struct *var);
void insert_Structtable(Var_or_Struct *struc);
void insert_Functable(FuncType *func);

Var_or_Struct *lookup_Vartable(char *name);
Var_or_Struct *lookup_Structtable(char *name);
FuncType *lookup_Functable(char *name);
#endif