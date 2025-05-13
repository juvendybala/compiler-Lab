#include "symboltable.h"

Var_or_Struct *Vartable[16384];
Var_or_Struct *Structtable[16384];
FuncType *Functable[16384];

int struct_num = 0; // 用于给匿名结构体赋唯一值，只用数字命名保证不可能与普通的ID重复

unsigned int hash_func(char *name)
{
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff)
        {
            val = (val ^ (i >> 12)) & 0x3fff;
        }
    }
    return val;
}

Var_or_Struct *get_Var(char *name, Type *type)
{
    Var_or_Struct *var = (Var_or_Struct *)malloc(sizeof(Var_or_Struct));
    var->kind = 0;
    var->name = name;
    var->type = type;
    var->next = NULL;
    var->is_param = 0;
    var->offset = -1;
    return var;
}

Var_or_Struct *get_Struct(char *name, Type *type)
{
    Var_or_Struct *struc = (Var_or_Struct *)malloc(sizeof(Var_or_Struct));
    struc->kind = 1;
    if (name == NULL)
    {
        name = (char *)malloc(sizeof(char) * 8);
        sprintf(name, "%d", struct_num);
        struct_num++;
    }
    struc->name = name;
    struc->type = type;
    struc->next = NULL;
    return struc;
}

Type *get_int()
{
    Type *Int = (Type *)malloc(sizeof(Type));
    Int->kind = 0;
    Int->u.basic = 0;
    return Int;
}

Type *get_float()
{
    Type *Float = (Type *)malloc(sizeof(Type));
    Float->kind = 0;
    Float->u.basic = 1;
    return Float;
}

Type *get_array(Type *right, int size, int elem_size)
{
    Type *array = (Type *)malloc(sizeof(Type));
    array->kind = 1;
    array->u.array.size = size;
    array->u.array.elem = right;
    array->u.array.elem_size = elem_size;
    return array;
}

Type *get_struct()
{
    Type *Struct = (Type *)malloc(sizeof(Type));
    Struct->kind = 2;
    Struct->u.structure = NULL;
    return Struct;
}

FieldList *get_FieldList(char *name, Type *type)
{
    FieldList *field = (FieldList *)malloc(sizeof(FieldList));
    field->name = name;
    field->next = NULL;
    field->type = type;
    return field;
}

int addFieldList2struct(Type *type, FieldList *field)
{
    if (type->u.structure == NULL)
    {
        type->u.structure = field;
    }
    else
    {
        FieldList *cur = type->u.structure;
        while (cur->next != NULL)
        {
            if (strcmp(cur->name, field->name) == 0)
            {
                return 0;
            }
            cur = cur->next;
        }
        if (strcmp(cur->name, field->name) == 0)
        {
            return 0;
        }
        else
        {
            cur->next = field;
        }
    }
    return 1;
}

FuncType *get_func(char *name, Type *rtype)
{
    FuncType *func = (FuncType *)malloc(sizeof(FuncType));
    func->name = name;
    func->argc = 0;
    func->argv = NULL;
    func->next = NULL;
    func->rtype = rtype;
    return func;
}

FuncArg *get_funcarg(Type *type)
{
    FuncArg *arg = (FuncArg *)malloc(sizeof(FuncArg));
    arg->type = type;
    arg->next = NULL;
}

int addFuncArg2func(FuncType *func, FuncArg *arg)
{
    if (func->argv == NULL)
    {
        func->argv = arg;
    }
    else
    {
        FuncArg *cur = func->argv;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        cur->next = arg;
    }
    func->argc++;
    return 1;
}

int is_same_type(Type *type1, Type *type2)
{
    if (type1 == NULL || type2 == NULL)
    {
        return 0;
    }
    if (type1->kind != type2->kind)
    {
        return 0;
    }
    else
    {
        if (type1->kind == 0)
        {
            return type1->u.basic == type2->u.basic;
        }
        else if (type1->kind == 1)
        {
            return is_same_type(type1->u.array.elem, type2->u.array.elem);
        }
        else
        {
            FieldList *field1 = type1->u.structure;
            FieldList *field2 = type2->u.structure;
            while (1)
            {
                if (field1 == NULL && field2 == NULL)
                {
                    break;
                }
                else if (field1 == NULL || field2 == NULL)
                {
                    return 0;
                }
                else
                {
                    if (is_same_type(field1->type, field2->type) == 0)
                    {
                        return 0;
                    }
                }
                field1 = field1->next;
                field2 = field2->next;
            }
            return 1;
        }
    }
}

int is_same_function(FuncType *type1, FuncType *type2)
{
    if (type1->argc != type2->argc)
    {
        return 0;
    }
    else
    {
        FuncArg *arg1 = type1->argv;
        FuncArg *arg2 = type2->argv;
        while (arg1 != NULL)
        {
            if (is_same_type(arg1->type, arg2->type) == 0)
            {
                return 0;
            }
            arg1 = arg1->next;
            arg2 = arg2->next;
        }
        return 1;
    }
}

void insert_Vartable(Var_or_Struct *var)
{
    unsigned int idx = hash_func(var->name);
    if (Vartable[idx] == NULL)
    {
        Vartable[idx] = var;
    }
    else
    {
        Var_or_Struct *cur = Vartable[idx];
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        cur->next = var;
    }
}

void insert_Structtable(Var_or_Struct *struc)
{
    unsigned int idx = hash_func(struc->name);
    if (Structtable[idx] == NULL)
    {
        // printf("%s\n", struc->name);
        // printf("%d\n", idx);
        Structtable[idx] = struc;
    }
    else
    {
        Var_or_Struct *cur = Structtable[idx];
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        cur->next = struc;
    }
}

void insert_Functable(FuncType *func)
{
    unsigned int idx = hash_func(func->name);
    if (Functable[idx] == NULL)
    {
        Functable[idx] = func;
    }
    else
    {
        FuncType *cur = Functable[idx];
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        cur->next = func;
    }
}

Var_or_Struct *lookup_Vartable(char *name)
{
    unsigned int idx = hash_func(name);
    Var_or_Struct *cur = Vartable[idx];
    while (cur != NULL)
    {
        if (strcmp(cur->name, name) == 0)
        {
            break;
        }
        cur = cur->next;
    }
    return cur;
}

Var_or_Struct *lookup_Structtable(char *name)
{
    unsigned int idx = hash_func(name);
    Var_or_Struct *cur = Structtable[idx];
    // if (cur == NULL)
    // {
    //     printf("%s\n", name);
    //     printf("%d\n", idx);
    // }
    while (cur != NULL)
    {
        if (strcmp(cur->name, name) == 0)
        {
            break;
        }
        cur = cur->next;
    }
    // if (cur == NULL)
    // {
    //     printf("1\n");
    // }
    return cur;
}

FuncType *lookup_Functable(char *name)
{
    unsigned int idx = hash_func(name);
    FuncType *cur = Functable[idx];
    while (cur != NULL)
    {
        if (strcmp(cur->name, name) == 0)
        {
            break;
        }
        cur = cur->next;
    }
    return cur;
}