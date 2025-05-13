#include "typechecking.h"

void add_func()
{
    FuncType *func1 = get_func("read", get_int());
    FuncType *func2 = get_func("write", NULL);
    FuncArg *arg2 = get_funcarg(get_int());
    addFuncArg2func(func2, arg2);
    insert_Functable(func1);
    insert_Functable(func2);
}

int get_type_size(Type *type)
{
    if (type->kind == 0)
    {
        return 4;
    }
    else if (type->kind == 1)
    {
        return type->u.array.size * type->u.array.elem_size;
    }
    else
    {
        return 0;
    }
}
void check_name(Node *node, const char *name)
{
    // printf("current function: %s\n", name);
    assert(node != NULL);
    // printf("current tree node: %s\n", node->name);
    assert(strcmp(node->name, name) == 0);
}

void typechecking_error(int num, int line, const char *msg)
{
    printf("Error type %d at Line %d: %s\n", num, line, msg);
}

int check_leftvalue(Node *node)
{
    // printf("1\n");
    if (strcmp(node->childs[0]->name, "ID") == 0 && node->num == 1) // ID
    {
        return 1;
    }
    else if (node->num >= 2 && (strcmp(node->childs[1]->name, "LB") == 0 || strcmp(node->childs[1]->name, "DOT") == 0)) // Exp LB Exp RB, Exp DOT ID
    {
        return check_leftvalue(node->childs[0]);
    }
    else
    {
        return 0;
    }
}

void ExtDefList(Node *node)
{
    if (node == NULL)
    {
        return;
    }
    check_name(node, "ExtDefList");
    Node *extdef = node->childs[0];
    Node *extdeflist = node->childs[1];
    ExtDef(extdef);
    ExtDefList(extdeflist);
}

void ExtDef(Node *node)
{
    check_name(node, "ExtDef");
    Node *specifier = node->childs[0];
    Type *type = Specifier(specifier);
    if (node->num == 2)
    {
        return;
    }
    else if (!strcmp(node->childs[1]->name, "ExtDecList"))
    {
        ExtDecList(node->childs[1], type);
    }
    else
    {
        FuncType *func = FunDec(node->childs[1], type);
        FuncType *result = lookup_Functable(func->name);
        if (result != NULL)
        {
            typechecking_error(4, node->childs[1]->line, "Redifined function");
        }
        else
        {
            insert_Functable(func);
        }
        CompSt(node->childs[2], func);
    }
}

Type *Specifier(Node *node)
{
    check_name(node, "Specifier");
    Node *type = node->childs[0];
    if (!strcmp(type->name, "TYPE"))
    {
        if (!strcmp(type->data.string_value, "int"))
        {
            return get_int();
        }
        else
        {
            return get_float();
        }
    }
    else
    {
        return StructSpecifier(type); // 返回值可能为空
    }
}

Type *StructSpecifier(Node *node)
{
    check_name(node, "StructSpecifier");
    has_struct = 1;
    if (node->num == 2)
    {
        char *name = node->childs[1]->childs[0]->data.string_value;
        Var_or_Struct *res = lookup_Structtable(name);
        if (res == NULL)
        {
            typechecking_error(17, node->childs[1]->childs[0]->line, "Undefined Struct");
            return NULL;
        }
        return res->type;
    }
    else
    {
        int error = 0;
        Node *opttag = node->childs[1];
        char *name = NULL;
        if (opttag != NULL)
        {
            name = opttag->childs[0]->data.string_value;
        }
        if (name != NULL)
        {
            Var_or_Struct *res1 = lookup_Structtable(name);
            Var_or_Struct *res2 = lookup_Vartable(name);
            if (res1 != NULL || res2 != NULL)
            {
                error = 1;
                typechecking_error(16, opttag->childs[0]->line, "Redefined Struct");
            }
        }
        Type *struc = get_struct();
        DefList_struct(node->childs[3], struc);
        Var_or_Struct *st = get_Struct(name, struc);
        if (error == 0)
        {
            insert_Structtable(st);
        }
        return struc;
    }
}

void DefList_struct(Node *node, Type *struc)
{
    if (node == NULL)
    {
        return;
    }
    check_name(node, "DefList");
    Node *def = node->childs[0];
    Node *deflist = node->childs[1];
    Def_struct(def, struc);
    DefList_struct(deflist, struc);
}

void Def_struct(Node *node, Type *struc)
{
    check_name(node, "Def");
    Node *specifier = node->childs[0];
    Node *declist = node->childs[1];
    Type *type = Specifier(specifier);
    DecList_struct(declist, struc, type);
}

void DecList_struct(Node *node, Type *struc, Type *type)
{
    check_name(node, "DecList");
    if (node->num == 1)
    {
        Node *dec = node->childs[0];
        Dec_struct(dec, struc, type);
    }
    else
    {
        Node *dec = node->childs[0];
        Node *declist = node->childs[2];
        Dec_struct(dec, struc, type);
        DecList_struct(declist, struc, type);
    }
}

void Dec_struct(Node *node, Type *struc, Type *type)
{
    check_name(node, "Dec");
    if (node->num == 3)
    {
        typechecking_error(15, node->line, "Assignment in struct");
        VarDec(node->childs[0], type, struc, NULL, 1);
    }
    else
    {
        VarDec(node->childs[0], type, struc, NULL, 1);
    }
}

void VarDec_struct(Node *node, Type *struc, Type *type)
{
    assert(node != NULL);
    assert(struc != NULL);
    assert(type != NULL);
    if (node->num == 1)
    {
        char *name = node->childs[0]->data.string_value;
        FieldList *field = get_FieldList(name, type);
        int res = addFieldList2struct(struc, field);
        if (res == 0)
        {
            typechecking_error(15, node->line, "duplicate definition in Struct");
        }
    }
    else
    {
        Node *vardec = node->childs[0];
        Type *new_type = get_array(type, node->childs[2]->data.int_value, get_type_size(type));
        VarDec(vardec, new_type, struc, NULL, 1);
    }
}

void ExtDecList(Node *node, Type *type)
{
    check_name(node, "ExtDecList");
    if (node->num == 1)
    {
        VarDec(node->childs[0], type, NULL, NULL, 0);
    }
    else
    {
        VarDec(node->childs[0], type, NULL, NULL, 0);
        ExtDecList(node->childs[2], type);
    }
}

void VarDec_var(Node *node, Type *type)
{
    if (node->num == 1)
    {
        char *name = node->childs[0]->data.string_value;
        Var_or_Struct *var = get_Var(name, type);
        Var_or_Struct *res1 = lookup_Vartable(name);
        Var_or_Struct *res2 = lookup_Structtable(name);
        if (res1 != NULL || res2 != NULL)
        {
            typechecking_error(3, node->line, "Redefined var");
        }
        else
        {
            insert_Vartable(var);
        }
    }
    else
    {
        VarDec(node->childs[0], get_array(type, node->childs[2]->data.int_value, get_type_size(type)), NULL, NULL, 0);
    }
}
FuncType *FunDec(Node *node, Type *rtype)
{
    check_name(node, "FunDec");
    Node *id = node->childs[0];
    char *name = id->data.string_value;
    FuncType *func = get_func(name, rtype);
    if (node->num == 3)
    {
        return func;
    }
    else
    {
        Node *varlist = node->childs[2];
        VarList(varlist, func);
        return func;
    }
}

void VarList(Node *node, FuncType *func)
{
    check_name(node, "VarList");
    if (node->num == 1)
    {
        ParamDec(node->childs[0], func);
    }
    else
    {
        ParamDec(node->childs[0], func);
        VarList(node->childs[2], func);
    }
}

void ParamDec(Node *node, FuncType *func)
{
    check_name(node, "ParamDec");
    Type *type = Specifier(node->childs[0]);
    VarDec(node->childs[1], type, NULL, func, 2);
}

void VarDec_func(Node *node, Type *type, FuncType *func)
{
    if (node->num == 1)
    {
        FuncArg *arg = get_funcarg(type);
        addFuncArg2func(func, arg);
        char *name = node->childs[0]->data.string_value;
        Var_or_Struct *res1 = lookup_Vartable(name);
        Var_or_Struct *res2 = lookup_Structtable(name);
        if (res1 != NULL || res2 != NULL)
        {
            typechecking_error(3, node->line, "Redefined var");
        }
        else
        {
            Var_or_Struct *var = get_Var(name, type);
            insert_Vartable(var);
        }
    }
    else
    {
        VarDec_func(node->childs[0], get_array(type, node->childs[2]->data.int_value, get_type_size(type)), func);
    }
}

void VarDec(Node *node, Type *type, Type *struc, FuncType *func, int kind)
{
    check_name(node, "VarDec");
    // printf("%d\n", kind);
    if (kind == 0) // 普通变量类型
    {
        VarDec_var(node, type);
    }
    else if (kind == 1) // 结构体类型
    {
        VarDec_struct(node, struc, type);
    }
    else // 函数参数
    {
        VarDec_func(node, type, func);
    }
}

void CompSt(Node *node, FuncType *func)
{
    check_name(node, "CompSt");
    DefList(node->childs[1]);
    StmtList(node->childs[2], func);
}

void DefList(Node *node)
{
    if (node == NULL)
    {
        return;
    }
    check_name(node, "DefList");
    Def(node->childs[0]);
    DefList(node->childs[1]);
}

void Def(Node *node)
{
    check_name(node, "Def");
    Type *type = Specifier(node->childs[0]);
    DecList(node->childs[1], type);
}

void DecList(Node *node, Type *type)
{
    check_name(node, "DecList");
    if (node->num == 1)
    {
        Dec(node->childs[0], type);
    }
    else
    {
        Dec(node->childs[0], type);
        DecList(node->childs[2], type);
    }
}

void Dec(Node *node, Type *type)
{
    check_name(node, "Dec");
    if (node->num == 1)
    {
        VarDec(node->childs[0], type, NULL, NULL, 0);
    }
    else
    {
        VarDec(node->childs[0], type, NULL, NULL, 0);
        Type *right_type = Exp(node->childs[2]);
        if (is_same_type(type, right_type) == 0)
        {
            typechecking_error(5, node->line, "Assignment dismatch");
        }
    }
}
void StmtList(Node *node, FuncType *func)
{
    if (node == NULL)
    {
        return;
    }
    check_name(node, "StmtList");
    Stmt(node->childs[0], func);
    StmtList(node->childs[1], func);
}

void Stmt(Node *node, FuncType *func)
{
    check_name(node, "Stmt");
    if (node->num == 1)
    {
        CompSt(node->childs[0], func);
    }
    else if (node->num == 2)
    {
        Exp(node->childs[0]);
    }
    else if (node->num == 3)
    {
        Type *rtype = Exp(node->childs[1]);
        if (is_same_type(func->rtype, rtype) == 0)
        {
            typechecking_error(8, node->line, "Mismatch ReturnType");
        }
    }
    else if (node->num == 5)
    {
        Exp(node->childs[2]);
        Stmt(node->childs[4], func);
    }
    else
    {
        Exp(node->childs[2]);
        Stmt(node->childs[4], func);
        Stmt(node->childs[6], func);
    }
}

Type *Exp(Node *node)
{
    check_name(node, "Exp");
    if (node->num == 1)
    {
        if (strcmp(node->childs[0]->name, "ID") == 0) // ID
        {
            Var_or_Struct *res = lookup_Vartable(node->childs[0]->data.string_value);
            if (res == NULL)
            {
                typechecking_error(1, node->line, "Undefined var");
                return NULL;
            }
            return res->type;
        }
        else if (strcmp(node->childs[0]->name, "INT") == 0) // INT
        {
            return get_int();
        }
        else // FLOAT
        {
            return get_float();
        }
    }
    else if (strcmp(node->childs[0]->name, "ID") == 0) // ID LP RP, ID LP Args RP
    {
        char *name = node->childs[0]->data.string_value;
        FuncType *func = get_func(name, NULL);
        if (node->num == 4) // ID LP Args RP
        {
            Args(node->childs[2], func);
        }
        Var_or_Struct *var = lookup_Vartable(name);
        if (var != NULL)
        {
            typechecking_error(11, node->line, "Var cannot be called");
            return var->type;
        }
        FuncType *res = lookup_Functable(name);
        if (res == NULL)
        {
            typechecking_error(2, node->line, "Undefined function");
            return NULL;
        }
        else
        {
            if (is_same_function(func, res) == 0)
            {
                typechecking_error(9, node->line, "Invalid Call");
                return res->rtype;
            }
            else
            {
                return res->rtype;
            }
        }
    }
    else if (node->num == 2)
    {
        if (strcmp(node->childs[0]->name, "MINUS") == 0) // MINUS Exp
        {
            Type *type = Exp(node->childs[1]);
            if (is_same_type(type, get_int()) == 0 && is_same_type(type, get_float()) == 0)
            {
                typechecking_error(7, node->line, "Unmatched operand type");
            }
            return type;
        }
        else // NOT Exp
        {
            Type *type = Exp(node->childs[1]);
            if (is_same_type(type, get_int()) == 0)
            {
                typechecking_error(7, node->line, "Unmatched operand type");
            }
            return get_int();
        }
    }
    else if (node->num == 3)
    {
        if (strcmp(node->childs[0]->name, "LP") == 0) // LP Exp RP
        {
            return Exp(node->childs[1]);
        }
        else if (strcmp(node->childs[1]->name, "AND") == 0 || strcmp(node->childs[1]->name, "OR") == 0) // Exp AND Exp, Exp OR Exp
        {
            Type *type1 = Exp(node->childs[0]);
            Type *type2 = Exp(node->childs[2]);
            if (is_same_type(type1, get_int()) == 0 || is_same_type(type2, get_int()) == 0)
            {
                typechecking_error(7, node->line, "Unmatched operand type");
            }
            return get_int();
        }
        else if (strcmp(node->childs[1]->name, "ASSIGNOP") == 0) // Exp ASSIGNOP Exp
        {
            Type *type1 = Exp(node->childs[0]);
            Type *type2 = Exp(node->childs[2]);
            if (is_same_type(type1, type2) == 0)
            {
                typechecking_error(5, node->line, "Assignment dismatch");
            }
            if (check_leftvalue(node->childs[0]) == 0)
            {
                typechecking_error(6, node->line, "Right Value appears in the left of Assignment");
            }
            return type1;
        }
        else if (strcmp(node->childs[1]->name, "DOT") == 0) // Exp DOT ID
        {
            Type *type = Exp(node->childs[0]);
            if (type == NULL || type->kind != 2)
            {
                typechecking_error(13, node->line, "Non-struct Var cannot use field");
                return type;
            }
            FieldList *struc = type->u.structure;
            char *name = node->childs[2]->data.string_value;
            while (struc != NULL)
            {
                if (strcmp(struc->name, name) == 0)
                {
                    break;
                }
                struc = struc->next;
            }
            if (struc == NULL)
            {
                typechecking_error(14, node->line, "Unknown field");
                return type;
            }
            else
            {
                return struc->type;
            }
        }
        else // Exp RELOP Exp, Exp PLUS Exp, Exp MINUS Exp, Exp STAR Exp, Exp DIV Exp
        {
            Type *type1 = Exp(node->childs[0]);
            Type *type2 = Exp(node->childs[2]);
            if (is_same_type(type1, type2) == 0)
            {
                typechecking_error(7, node->line, "Operation between different type");
                return type1;
            }
            else if (is_same_type(type1, get_int()) == 0 && is_same_type(type1, get_float()) == 0)
            {
                typechecking_error(7, node->line, "Operand 1 type invalid");
                return type1;
            }
            else if (is_same_type(type2, get_int()) == 0 && is_same_type(type2, get_float()) == 0)
            {
                typechecking_error(7, node->line, "Operand 2 type invalid");
                return type1;
            }
            else
            {
                if (strcmp(node->childs[1]->name, "RELOP") == 0)
                {
                    return get_int();
                }
                else
                {
                    return type1;
                }
            }
        }
    }
    else // Exp LB Exp RB
    {
        Type *type1 = Exp(node->childs[0]);
        Type *type2 = Exp(node->childs[2]);
        if (is_same_type(type2, get_int()) == 0)
        {
            typechecking_error(12, node->line, "array num should be int");
        }
        if (type1->kind != 1)
        {
            typechecking_error(10, node->line, "only array can use []");
            return type1;
        }
        else
        {
            return type1->u.array.elem;
        }
    }
}

void Args(Node *node, FuncType *func)
{
    check_name(node, "Args");
    Type *type1 = Exp(node->childs[0]);
    addFuncArg2func(func, get_funcarg(type1));
    if (node->num != 1)
    {
        Args(node->childs[2], func);
    }
}