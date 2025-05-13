#include "translate.h"

Code *translate_ExtDefList(Node *node)
{
    if (node == NULL)
    {
        return new_Code();
    }
    check_name(node, "ExtDefList");
    Code *code1 = translate_ExtDef(node->childs[0]);
    Code *code2 = translate_ExtDefList(node->childs[1]);
    appendCode2Code(code1, code2);
    return code1;
}

Code *translate_ExtDef(Node *node)
{
    check_name(node, "ExtDef");
    // Specifier FunDec CompSt
    Code *code1 = translate_FunDec(node->childs[1]);
    Code *code2 = translate_CompSt(node->childs[2]);
    appendCode2Code(code1, code2);
    return code1;
}

Code *translate_FunDec(Node *node)
{
    check_name(node, "FunDec");
    Code *code = new_Code();
    IR *ir = new_IR(IR_FUNC, node->childs[0]->data.string_value, NULL, NULL, NULL);
    addIR2Code(code, ir);
    if (node->num == 4)
    {
        Code *code1 = translate_VarList(node->childs[2]);
        appendCode2Code(code, code1);
    }
    return code;
}

Code *translate_VarList(Node *node)
{
    check_name(node, "VarList");
    if (node->num == 1)
    {
        Code *code1 = translate_ParamDec(node->childs[0]);
        return code1;
    }
    else
    {
        Code *code1 = translate_ParamDec(node->childs[0]);
        Code *code2 = translate_VarList(node->childs[2]);
        appendCode2Code(code1, code2);
        return code1;
    }
}

Code *translate_ParamDec(Node *node)
{
    check_name(node, "ParamDec");
    Code *code = new_Code();
    addIR2Code(code, translate_VarDec_func(node->childs[1]));
    return code;
}

IR *translate_VarDec_func(Node *node)
{
    check_name(node, "VarDec");
    if (node->num == 1)
    {
        char *name = node->childs[0]->data.string_value;
        Var_or_Struct *var = lookup_Vartable(name);
        var->is_param = 1;
        return new_IR(IR_PARAM, name, NULL, NULL, NULL);
    }
    else
    {
        return translate_VarDec_func(node->childs[0]);
    }
}

Code *translate_CompSt(Node *node)
{
    check_name(node, "CompSt");
    Code *code1 = translate_DefList(node->childs[1]);
    // printf("1\n");
    Code *code2 = translate_StmtList(node->childs[2]);
    appendCode2Code(code1, code2);
    return code1;
}

Code *translate_DefList(Node *node)
{
    if (node == NULL)
    {
        return new_Code();
    }
    check_name(node, "DefList");
    Code *code1 = translate_Def(node->childs[0]);
    // printf("1\n");
    Code *code2 = translate_DefList(node->childs[1]);
    // printf("1\n");
    appendCode2Code(code1, code2);
    // printf("1\n");
    return code1;
}

Code *translate_Def(Node *node)
{
    check_name(node, "Def");
    Code *code = translate_DecList(node->childs[1]);
    // print_Code(stdout, code);
    return code;
}

Code *translate_DecList(Node *node)
{
    check_name(node, "DecList");
    if (node->num == 1)
    {
        Code *code = translate_Dec(node->childs[0]);
        // print_Code(stdout, code);
        return code;
    }
    else
    {
        // printf("1\n");
        Code *code1 = translate_Dec(node->childs[0]);
        Code *code2 = translate_DecList(node->childs[2]);
        appendCode2Code(code1, code2);
        return code1;
    }
}

Code *translate_Dec(Node *node)
{
    check_name(node, "Dec");
    Node *vardec = node->childs[0];
    while (strcmp(vardec->name, "ID") != 0)
    {
        vardec = vardec->childs[0];
    }
    Var_or_Struct *var = lookup_Vartable(vardec->data.string_value);
    char *name = var->name;
    Type *type = var->type;
    Code *res = new_Code();
    if (type->kind == 1)
    {
        int size = type->u.array.size * type->u.array.elem_size;
        char *alloc_size = (char *)malloc(sizeof(char) * 8);
        sprintf(alloc_size, "%d", size);
        IR *ir = new_IR(IR_DEC, name, alloc_size, NULL, NULL);
        addIR2Code(res, ir);
    }
    if (node->num == 3) // 只可能是对普通局部变量进行初始化赋值
    {
        char *t1 = new_temp();
        Code *code = translate_Exp(node->childs[2], t1);
        IR *ir = new_IR(IR_ASSIGN_1, name, t1, NULL, NULL);
        addIR2Code(code, ir);
        appendCode2Code(res, code);
    }
    // printf("1\n");
    return res;
}

Code *translate_StmtList(Node *node)
{
    if (node == NULL)
    {
        return new_Code();
    }
    check_name(node, "StmtList");
    Code *code1 = translate_Stmt(node->childs[0]);
    Code *code2 = translate_StmtList(node->childs[1]);
    appendCode2Code(code1, code2);
    return code1;
}

Code *translate_Stmt(Node *node)
{
    check_name(node, "Stmt");
    if (node->num == 1) // CompSt
    {
        return translate_CompSt(node->childs[0]);
    }
    else if (node->num == 2) // Exp SEMI
    {
        return translate_Exp(node->childs[0], "tNULL");
    }
    else if (node->num == 3) // RETURN Exp SEMI
    {
        char *t1 = new_temp();
        Code *code = translate_Exp(node->childs[1], t1);
        IR *ir = new_IR(IR_RET, t1, NULL, NULL, NULL);
        addIR2Code(code, ir);
        return code;
    }
    else if (node->num == 5)
    {
        if (strcmp(node->childs[0]->name, "IF") == 0) // IF LP Exp RP Stmt
        {
            char *label1 = new_label();
            char *label2 = new_label();
            Code *code1 = translate_Cond(node->childs[2], label1, label2);
            Code *code2 = translate_Stmt(node->childs[4]);
            IR *ir1 = new_IR(IR_LABEL, label1, NULL, NULL, NULL);
            IR *ir2 = new_IR(IR_LABEL, label2, NULL, NULL, NULL);
            addIR2Code(code1, ir1);
            addIR2Code(code2, ir2);
            appendCode2Code(code1, code2);
            return code1;
        }
        else // WHILE LP Exp RP Stmt
        {
            char *label1 = new_label();
            char *label2 = new_label();
            char *label3 = new_label();
            Code *code1 = translate_Cond(node->childs[2], label2, label3);
            Code *code2 = translate_Stmt(node->childs[4]);
            Code *res = new_Code();
            IR *ir1 = new_IR(IR_LABEL, label1, NULL, NULL, NULL);
            IR *ir2 = new_IR(IR_LABEL, label2, NULL, NULL, NULL);
            IR *ir3 = new_IR(IR_GOTO, label1, NULL, NULL, NULL);
            IR *ir4 = new_IR(IR_LABEL, label3, NULL, NULL, NULL);
            addIR2Code(res, ir1);
            addIR2Code(code1, ir2);
            appendCode2Code(res, code1);
            addIR2Code(code2, ir3);
            addIR2Code(code2, ir4);
            appendCode2Code(res, code2);
            return res;
        }
    }
    else // IF LP Exp RP Stmt ELSE Stmt
    {
        char *label1 = new_label();
        char *label2 = new_label();
        char *label3 = new_label();
        Code *code1 = translate_Cond(node->childs[2], label1, label2);
        Code *code2 = translate_Stmt(node->childs[4]);
        Code *code3 = translate_Stmt(node->childs[6]);
        IR *ir1 = new_IR(IR_LABEL, label1, NULL, NULL, NULL);
        IR *ir2 = new_IR(IR_GOTO, label3, NULL, NULL, NULL);
        IR *ir3 = new_IR(IR_LABEL, label2, NULL, NULL, NULL);
        IR *ir4 = new_IR(IR_LABEL, label3, NULL, NULL, NULL);
        addIR2Code(code1, ir1);
        addIR2Code(code2, ir2);
        addIR2Code(code2, ir3);
        appendCode2Code(code1, code2);
        addIR2Code(code3, ir4);
        appendCode2Code(code1, code3);
        return code1;
    }
}

Code *translate_Cond(Node *node, char *label_true, char *label_false)
{
    check_name(node, "Exp");
    if (node->num == 3 && strcmp(node->childs[1]->name, "RELOP") == 0) // Exp RELOP Exp
    {
        char *t1 = new_temp();
        char *t2 = new_temp();
        Code *code1 = translate_Exp(node->childs[0], t1);
        Code *code2 = translate_Exp(node->childs[2], t2);
        IR *ir1 = new_IR(IR_IF_GOTO, label_true, t1, node->childs[1]->data.string_value, t2);
        IR *ir2 = new_IR(IR_GOTO, label_false, NULL, NULL, NULL);
        appendCode2Code(code1, code2);
        addIR2Code(code1, ir1);
        addIR2Code(code1, ir2);
        return code1;
    }
    else if (node->num == 2 && strcmp(node->childs[0]->name, "NOT") == 0) // NOT Exp
    {
        return translate_Cond(node->childs[1], label_false, label_true);
    }
    else if (node->num == 3 && strcmp(node->childs[1]->name, "AND") == 0) // Exp AND Exp
    {
        char *label1 = new_label();
        Code *code1 = translate_Cond(node->childs[0], label1, label_false);
        Code *code2 = translate_Cond(node->childs[2], label_true, label_false);
        IR *ir = new_IR(IR_LABEL, label1, NULL, NULL, NULL);
        addIR2Code(code1, ir);
        appendCode2Code(code1, code2);
        return code1;
    }
    else if (node->num == 3 && strcmp(node->childs[1]->name, "OR") == 0) // Exp OR Exp
    {
        char *label1 = new_label();
        Code *code1 = translate_Cond(node->childs[0], label_true, label1);
        Code *code2 = translate_Cond(node->childs[2], label_true, label_false);
        IR *ir = new_IR(IR_LABEL, label1, NULL, NULL, NULL);
        addIR2Code(code1, ir);
        appendCode2Code(code1, code2);
        return code1;
    }
    else
    {
        char *t1 = new_temp();
        Code *code1 = translate_Exp(node, t1);
        IR *ir1 = new_IR(IR_IF_GOTO, label_true, t1, "!=", "#0");
        IR *ir2 = new_IR(IR_GOTO, label_false, NULL, NULL, NULL);
        addIR2Code(code1, ir1);
        addIR2Code(code1, ir2);
        return code1;
    }
}

Code *translate_Exp(Node *node, char *place)
{
    check_name(node, "Exp");
    if (node->num == 1)
    {
        if (strcmp(node->childs[0]->name, "ID") == 0) // ID
        {
            char *name = node->childs[0]->data.string_value;
            Var_or_Struct *var = lookup_Vartable(name);
            Type *type = var->type;
            if (type->kind == 0)
            {
                Code *res = new_Code();
                IR *ir = new_IR(IR_ASSIGN_1, place, name, NULL, NULL);
                addIR2Code(res, ir);
                return res;
            }
            else
            {
                if (var->is_param == 1)
                {
                    Code *res = new_Code();
                    IR *ir = new_IR(IR_ASSIGN_1, place, name, NULL, NULL);
                    addIR2Code(res, ir);
                    return res;
                }
                else
                {
                    Code *res = new_Code();
                    IR *ir = new_IR(IR_ASSIGN_ADDR, place, name, NULL, NULL);
                    addIR2Code(res, ir);
                    return res;
                }
            }
        }
        else if (strcmp(node->childs[0]->name, "INT") == 0) // INT
        {
            Code *res = new_Code();
            int val = node->childs[0]->data.int_value;
            char *arg1 = (char *)malloc(sizeof(char) * 8 + 1);
            sprintf(arg1, "#%d", val);
            IR *ir = new_IR(IR_ASSIGN_1, place, arg1, NULL, NULL);
            addIR2Code(res, ir);
            return res;
        }
    }
    else if (strcmp(node->childs[0]->name, "ID") == 0) // ID LP RP, ID LP Args RP
    {
        return translate_Func(node, place);
    }
    else if (node->num == 2)
    {
        if (strcmp(node->childs[0]->name, "MINUS") == 0) // MINUS Exp
        {
            char *t1 = new_temp();
            Code *code1 = translate_Exp(node->childs[1], t1);
            IR *ir = new_IR(IR_ASSIGN_2, place, "#0", "-", t1);
            addIR2Code(code1, ir);
            return code1;
        }
        else // NOT Exp
        {
            return translate_Exp_help(node, place);
        }
    }
    else if (node->num == 3)
    {
        if (strcmp(node->childs[0]->name, "LP") == 0) // LP Exp RP
        {
            return translate_Exp(node->childs[1], place);
        }
        else if (strcmp(node->childs[1]->name, "AND") == 0 || strcmp(node->childs[1]->name, "OR") == 0) // Exp AND Exp, Exp OR Exp
        {
            return translate_Exp_help(node, place);
        }
        else if (strcmp(node->childs[1]->name, "ASSIGNOP") == 0) // Exp ASSIGNOP Exp
        {
            char *rt = new_temp();
            Code *rcode = translate_Exp(node->childs[2], rt);
            Node *lexp = node->childs[0];
            if (strcmp(lexp->childs[0]->name, "ID") == 0)
            {
                char *name = lexp->childs[0]->data.string_value;
                Var_or_Struct *var = lookup_Vartable(name);
                Type *type = var->type;
                if (type->kind == 0)
                {
                    IR *ir1 = new_IR(IR_ASSIGN_1, name, rt, NULL, NULL);
                    IR *ir2 = new_IR(IR_ASSIGN_1, place, name, NULL, NULL);
                    addIR2Code(rcode, ir1);
                    addIR2Code(rcode, ir2);
                    return rcode;
                }
                else
                {
                    char *t1 = new_temp();
                    Info *linfo = translate_array(lexp, t1);
                    Code *lcode = linfo->code;
                    Type *type = linfo->type;
                    int times = type->u.array.elem_size * type->u.array.size / 4;
                    appendCode2Code(rcode, lcode);
                    char *t2 = new_temp();
                    for (int i = 0; i < times; i++)
                    {
                        IR *ir1 = new_IR(IR_ASSIGN_MADDR, t2, rt, NULL, NULL);
                        IR *ir2 = new_IR(IR_MADDR_ASSIGN, t1, t2, NULL, NULL);
                        addIR2Code(rcode, ir1);
                        addIR2Code(rcode, ir2);
                        if (i != times - 1)
                        {
                            IR *ir3 = new_IR(IR_ASSIGN_2, rt, rt, "+", "#4");
                            IR *ir4 = new_IR(IR_ASSIGN_2, t1, t1, "+", "#4");
                            addIR2Code(rcode, ir3);
                            addIR2Code(rcode, ir4);
                        }
                    }
                    return rcode;
                }
            }
            else
            {
                char *laddr = new_temp();
                Info *linfo = translate_array(lexp, laddr);
                Code *lcode = linfo->code;
                Type *type = linfo->type;
                appendCode2Code(rcode, lcode);
                if (type->kind == 0)
                {
                    IR *ir1 = new_IR(IR_MADDR_ASSIGN, laddr, rt, NULL, NULL);
                    IR *ir2 = new_IR(IR_ASSIGN_1, place, rt, NULL, NULL);
                    addIR2Code(rcode, ir1);
                    addIR2Code(rcode, ir2);
                    return rcode;
                }
                else
                {
                    int times = type->u.array.elem_size * type->u.array.size / 4;
                    char *tmp = new_temp();
                    for (int i = 0; i < times; i++)
                    {
                        IR *ir1 = new_IR(IR_ASSIGN_MADDR, tmp, rt, NULL, NULL);
                        IR *ir2 = new_IR(IR_MADDR_ASSIGN, laddr, tmp, NULL, NULL);
                        addIR2Code(rcode, ir1);
                        addIR2Code(rcode, ir2);
                        if (i != times - 1)
                        {
                            IR *ir3 = new_IR(IR_ASSIGN_2, rt, rt, "+", "#4");
                            IR *ir4 = new_IR(IR_ASSIGN_2, laddr, laddr, "+", "#4");
                            addIR2Code(rcode, ir3);
                            addIR2Code(rcode, ir4);
                        }
                    }
                    return rcode;
                }
            }
        }
        else if (strcmp(node->childs[1]->name, "DOT") == 0) // Exp DOT ID
        {
            return new_Code();
        }
        else // Exp RELOP Exp, Exp PLUS Exp, Exp MINUS Exp, Exp STAR Exp, Exp DIV Exp
        {
            if (strcmp(node->childs[1]->name, "RELOP") == 0)
            {
                return translate_Exp_help(node, place);
            }
            else
            {
                char *t1 = new_temp();
                char *t2 = new_temp();
                Code *code1 = translate_Exp(node->childs[0], t1);
                Code *code2 = translate_Exp(node->childs[2], t2);
                IR *ir;
                if (strcmp(node->childs[1]->name, "PLUS") == 0)
                {
                    ir = new_IR(IR_ASSIGN_2, place, t1, "+", t2);
                }
                else if (strcmp(node->childs[1]->name, "MINUS") == 0)
                {
                    ir = new_IR(IR_ASSIGN_2, place, t1, "-", t2);
                }
                else if (strcmp(node->childs[1]->name, "STAR") == 0)
                {
                    ir = new_IR(IR_ASSIGN_2, place, t1, "*", t2);
                }
                else if (strcmp(node->childs[1]->name, "DIV") == 0)
                {
                    ir = new_IR(IR_ASSIGN_2, place, t1, "/", t2);
                }
                addIR2Code(code2, ir);
                appendCode2Code(code1, code2);
                return code1;
            }
        }
    }
    else // Exp LB Exp RB
    {
        char *t1 = new_temp();
        Info *info = translate_array(node, t1);
        if (info->type->kind == 0)
        {
            IR *ir = new_IR(IR_ASSIGN_MADDR, place, t1, NULL, NULL);
            addIR2Code(info->code, ir);
        }
        else
        {
            IR *ir = new_IR(IR_ASSIGN_1, place, t1, NULL, NULL);
            addIR2Code(info->code, ir);
        }
        return info->code;
    }
}

Info *translate_array(Node *node, char *place)
{
    check_name(node, "Exp");
    Info *info = (Info *)malloc(sizeof(info));
    if (node->num == 1)
    {
        info->code = new_Code();
        char *name = node->childs[0]->data.string_value;
        Var_or_Struct *var = lookup_Vartable(name);
        info->type = var->type;
        if (var->is_param == 1)
        {
            IR *ir = new_IR(IR_ASSIGN_1, place, name, NULL, NULL);
            addIR2Code(info->code, ir);
            return info;
        }
        else
        {
            IR *ir = new_IR(IR_ASSIGN_ADDR, place, name, NULL, NULL);
            addIR2Code(info->code, ir);
            return info;
        }
    }
    else
    {
        char *t1 = new_temp();
        Info *info1 = translate_array(node->childs[0], t1);
        char *t2 = new_temp();
        Code *code1 = translate_Exp(node->childs[2], t2);
        info->type = info1->type->u.array.elem;
        Code *code = info1->code;
        int elem_size = info1->type->u.array.elem_size;
        char *elem_sz = (char *)malloc(sizeof(char) * 8 + 1);
        sprintf(elem_sz, "#%d", elem_size);
        IR *ir1 = new_IR(IR_ASSIGN_2, t2, t2, "*", elem_sz);
        IR *ir2 = new_IR(IR_ASSIGN_2, t1, t1, "+", t2);
        IR *ir3 = new_IR(IR_ASSIGN_1, place, t1, NULL, NULL);
        appendCode2Code(code, code1);
        addIR2Code(code, ir1);
        addIR2Code(code, ir2);
        addIR2Code(code, ir3);
        info->code = code;
        return info;
    }
}

Code *translate_Exp_help(Node *node, char *place)
{
    check_name(node, "Exp");
    char *label1 = new_label();
    char *label2 = new_label();
    Code *res = new_Code();
    IR *ir1 = new_IR(IR_ASSIGN_1, place, "#0", NULL, NULL);
    Code *code = translate_Cond(node, label1, label2);
    IR *ir2 = new_IR(IR_LABEL, label1, NULL, NULL, NULL);
    IR *ir3 = new_IR(IR_ASSIGN_1, place, "#1", NULL, NULL);
    IR *ir4 = new_IR(IR_LABEL, label2, NULL, NULL, NULL);
    addIR2Code(res, ir1);
    addIR2Code(code, ir2);
    addIR2Code(code, ir3);
    addIR2Code(code, ir4);
    appendCode2Code(res, code);
    return res;
}

Code *translate_Func(Node *node, char *place)
{
    check_name(node, "Exp");
    if (node->num == 3) // ID LP RP
    {
        char *name = node->childs[0]->data.string_value;
        if (strcmp(name, "read") == 0)
        {
            Code *res = new_Code();
            IR *ir = new_IR(IR_READ, place, NULL, NULL, NULL);
            addIR2Code(res, ir);
            return res;
        }
        else
        {
            Code *res = new_Code();
            IR *ir = new_IR(IR_CALL, place, name, NULL, NULL);
            addIR2Code(res, ir);
            return res;
        }
    }
    else // ID LP Args RP
    {
        char *name = node->childs[0]->data.string_value;
        ArgList *arg_list = (ArgList *)malloc(sizeof(ArgList));
        arg_list->head = NULL;
        Code *code1 = translate_Args(node->childs[2], arg_list);
        if (strcmp(name, "write") == 0)
        {
            IR *ir1 = new_IR(IR_WRITE, arg_list->head->name, NULL, NULL, NULL);
            IR *ir2 = new_IR(IR_ASSIGN_1, place, "#0", NULL, NULL);
            addIR2Code(code1, ir1);
            addIR2Code(code1, ir2);
            return code1;
        }
        Arg *arg = arg_list->head;
        while (arg != NULL)
        {
            IR *ir = new_IR(IR_ARG, arg->name, NULL, NULL, NULL);
            addIR2Code(code1, ir);
            arg = arg->next;
        }
        IR *ir = new_IR(IR_CALL, place, name, NULL, NULL);
        addIR2Code(code1, ir);
        return code1;
    }
}

Code *translate_Args(Node *node, ArgList *arglist)
{
    check_name(node, "Args");
    if (node->num == 1)
    {
        char *t1 = new_temp();
        Code *code = translate_Exp(node->childs[0], t1);
        Arg *arg = (Arg *)malloc(sizeof(Arg));
        arg->name = t1;
        arg->next = arglist->head;
        arglist->head = arg;
        return code;
    }
    else
    {
        char *t1 = new_temp();
        Code *code1 = translate_Exp(node->childs[0], t1);
        Arg *arg = (Arg *)malloc(sizeof(Arg));
        arg->name = t1;
        arg->next = arglist->head;
        arglist->head = arg;
        Code *code2 = translate_Args(node->childs[2], arglist);
        appendCode2Code(code1, code2);
        return code1;
    }
}