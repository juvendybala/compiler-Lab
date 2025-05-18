#include "generate.h"

int arg_cnt = 0;   // 记录实参个数
int param_cnt = 0; // 记录形参个数

mipscodes *transcode2mipscodes(Code *code)
{
    mipscodes *mcs = new_mipcodes();
    init_mipscodes(mcs);
    IR *cur = code->head;
    while (cur != NULL)
    {
        mipscode *skip_line = new_mipcode(5, NULL, NULL, NULL, NULL);
        addmc2mcs(skip_line, mcs);
        mipscodes *mcs_line = new_mipcodes();
        switch (cur->kind)
        {
        case IR_LABEL:
        {
            char *label = cur->dest;
            mipscode *mc = new_mipcode(0, label, NULL, NULL, NULL);
            addmc2mcs(mc, mcs_line);
        }
        break;
        case IR_FUNC:
        {
            char *name = cur->dest;
            mipscode *mc = new_mipcode(0, name, NULL, NULL, NULL);
            addmc2mcs(mc, mcs_line);
            int offset = calculate_offset(cur);
            mipscode *mc1 = new_mipcode(4, "addi", "$sp,", "$sp,", "-4");
            addmc2mcs(mc1, mcs_line);
            mipscode *mc2 = new_mipcode(3, "sw", "$fp,", "0($sp)", NULL);
            addmc2mcs(mc2, mcs_line);
            mipscode *mc3 = new_mipcode(3, "la", "$fp,", "0($sp)", NULL);
            addmc2mcs(mc3, mcs_line);
            char *c_offset = (char *)malloc(sizeof(char) * 8 + 1);
            sprintf(c_offset, "%d", -offset);
            mipscode *mc4 = new_mipcode(4, "addi", "$sp,", "$sp,", c_offset);
            addmc2mcs(mc4, mcs_line);
            arg_cnt = 0;
            param_cnt = 0;
        }
        break;
        case IR_ASSIGN_1:
        {
            char *dest = cur->dest;
            char *arg1 = cur->arg1;
            if (arg1[0] == '#')
            {
                mipscode *mc = new_mipcode(3, "li", "$t0,", arg1 + 1, NULL);
                addmc2mcs(mc, mcs_line);
            }
            else
            {
                char *c_arg1 = find_offset(arg1);
                mipscode *mc = new_mipcode(3, "lw", "$t0,", c_arg1, NULL);
                addmc2mcs(mc, mcs_line);
            }
            char *c_dest = find_offset(dest);
            mipscode *mc = new_mipcode(3, "sw", "$t0,", c_dest, NULL);
            addmc2mcs(mc, mcs_line);
        }
        break;
        case IR_ASSIGN_2:
        {
            char *arg1 = cur->arg1;
            char *arg2 = cur->arg2;
            if (arg1[0] == '#')
            {
                mipscode *mc = new_mipcode(3, "li", "$t0,", arg1 + 1, NULL);
                addmc2mcs(mc, mcs_line);
            }
            else
            {
                char *c_arg1 = find_offset(arg1);
                mipscode *mc = new_mipcode(3, "lw", "$t0,", c_arg1, NULL);
                addmc2mcs(mc, mcs_line);
            }
            if (arg2[0] == '#')
            {
                mipscode *mc = new_mipcode(3, "li", "$t1,", arg2 + 1, NULL);
                addmc2mcs(mc, mcs_line);
            }
            else
            {
                char *c_arg2 = find_offset(arg2);
                mipscode *mc = new_mipcode(3, "lw", "$t1,", c_arg2, NULL);
                addmc2mcs(mc, mcs_line);
            }
            char *op = cur->op;
            if (strcmp(op, "+") == 0)
            {
                mipscode *mc = new_mipcode(4, "add", "$t0,", "$t0,", "$t1");
                addmc2mcs(mc, mcs_line);
            }
            else if (strcmp(op, "-") == 0)
            {
                mipscode *mc = new_mipcode(4, "sub", "$t0,", "$t0,", "$t1");
                addmc2mcs(mc, mcs_line);
            }
            else if (strcmp(op, "*") == 0)
            {
                mipscode *mc = new_mipcode(4, "mul", "$t0,", "$t0,", "$t1");
                addmc2mcs(mc, mcs_line);
            }
            else if (strcmp(op, "/") == 0)
            {
                mipscode *mc = new_mipcode(4, "div", "$t0,", "$t1", NULL);
                addmc2mcs(mc, mcs_line);
                mipscode *mc1 = new_mipcode(2, "mflo", "$t0", NULL, NULL);
                addmc2mcs(mc, mcs_line);
            }
            else
            {
                printf("something went wrong\n");
                exit(1);
            }
            char *c_dest = find_offset(cur->dest);
            mipscode *mc = new_mipcode(3, "sw", "$t0,", c_dest, NULL);
            addmc2mcs(mc, mcs_line);
        }
        break;
        case IR_ASSIGN_ADDR:
        {
            char *arg1 = cur->arg1;
            char *c_arg1 = find_offset(arg1);
            mipscode *mc = new_mipcode(3, "la", "$t0,", c_arg1, NULL);
            addmc2mcs(mc, mcs_line);
            char *dest = cur->dest;
            char *c_dest = find_offset(dest);
            mipscode *mc1 = new_mipcode(3, "sw", "$t0,", c_dest, NULL);
            addmc2mcs(mc1, mcs_line);
        }
        break;
        case IR_ASSIGN_MADDR:
        {
            char *arg1 = cur->arg1;
            char *c_arg1 = find_offset(arg1);
            mipscode *mc = new_mipcode(3, "lw", "$t0,", c_arg1, NULL);
            addmc2mcs(mc, mcs_line);
            mipscode *mc1 = new_mipcode(3, "lw", "$t0,", "0($t0)", NULL);
            addmc2mcs(mc, mcs_line);
            char *dest = cur->dest;
            char *c_dest = find_offset(dest);
            mipscode *mc2 = new_mipcode(3, "sw", "$t0,", c_dest, NULL);
            addmc2mcs(mc2, mcs_line);
        }
        break;
        case IR_MADDR_ASSIGN:
        {
            char *arg1 = cur->arg1;
            if (arg1[0] == '#')
            {
                mipscode *mc = new_mipcode(3, "li", "$t0,", arg1 + 1, NULL);
                addmc2mcs(mc, mcs_line);
            }
            else
            {
                char *c_arg1 = find_offset(arg1);
                mipscode *mc = new_mipcode(3, "lw", "$t0,", c_arg1, NULL);
            }
            char *dest = cur->dest;
            char *c_dest = find_offset(dest);
            mipscode *mc1 = new_mipcode(3, "lw", "$t1,", c_dest, NULL);
            addmc2mcs(mc1, mcs_line);
            mipscode *mc2 = new_mipcode(3, "sw", "$t0,", "0($t1)", NULL);
            addmc2mcs(mc2, mcs_line);
        }
        break;
        case IR_GOTO:
        {
            char *dest = cur->dest;
            mipscode *mc = new_mipcode(2, "j", dest, NULL, NULL);
            addmc2mcs(mc, mcs_line);
        }
        break;
        case IR_IF_GOTO:
        {
            char *arg1 = cur->arg1;
            char *arg2 = cur->arg2;
            if (arg1[0] == '#')
            {
                mipscode *mc = new_mipcode(3, "li", "$t0,", arg1 + 1, NULL);
                addmc2mcs(mc, mcs_line);
            }
            else
            {
                char *c_arg1 = find_offset(arg1);
                mipscode *mc = new_mipcode(3, "lw", "$t0,", c_arg1, NULL);
                addmc2mcs(mc, mcs_line);
            }
            if (arg2[0] == '#')
            {
                mipscode *mc = new_mipcode(3, "li", "$t1,", arg2 + 1, NULL);
                addmc2mcs(mc, mcs_line);
            }
            else
            {
                char *c_arg2 = find_offset(arg2);
                mipscode *mc = new_mipcode(3, "lw", "$t1,", c_arg2, NULL);
                addmc2mcs(mc, mcs_line);
            }
            char *op = cur->op;
            char *dest = cur->dest;
            if (strcmp(op, "<") == 0)
            {
                mipscode *mc = new_mipcode(4, "blt", "$t0,", "$t1,", dest);
                addmc2mcs(mc, mcs_line);
            }
            else if (strcmp(op, ">") == 0)
            {
                mipscode *mc = new_mipcode(4, "bgt", "$t0,", "$t1,", dest);
                addmc2mcs(mc, mcs_line);
            }
            else if (strcmp(op, "<=") == 0)
            {
                mipscode *mc = new_mipcode(4, "ble", "$t0,", "$t1,", dest);
                addmc2mcs(mc, mcs_line);
            }
            else if (strcmp(op, ">=") == 0)
            {
                mipscode *mc = new_mipcode(4, "bge", "$t0,", "$t1,", dest);
                addmc2mcs(mc, mcs_line);
            }
            else if (strcmp(op, "==") == 0)
            {
                mipscode *mc = new_mipcode(4, "beq", "$t0,", "$t1,", dest);
                addmc2mcs(mc, mcs_line);
            }
            else if (strcmp(op, "!=") == 0)
            {
                mipscode *mc = new_mipcode(4, "bne", "$t0,", "$t1,", dest);
                addmc2mcs(mc, mcs_line);
            }
            else
            {
                printf("Illegal relop\n");
                exit(1);
            }
        }
        break;
        case IR_RET:
        {
            char *dest = cur->dest;
            char *c_dest = find_offset(dest);
            mipscode *mc1 = new_mipcode(3, "lw", "$v0,", c_dest, NULL);
            addmc2mcs(mc1, mcs_line);
            mipscode *mc2 = new_mipcode(3, "move", "$sp,", "$fp", NULL);
            addmc2mcs(mc2, mcs_line);
            mipscode *mc3 = new_mipcode(3, "lw", "$fp,", "0($sp)", NULL);
            addmc2mcs(mc3, mcs_line);
            mipscode *mc4 = new_mipcode(4, "addi", "$sp,", "$sp,", "4");
            addmc2mcs(mc4, mcs_line);
            mipscode *mc5 = new_mipcode(2, "jr", "$ra", NULL, NULL);
            addmc2mcs(mc5, mcs_line);
        }
        break;
        case IR_DEC:
            break;
        case IR_ARG:
        {
            if (arg_cnt == 0)
            {
                IR *arg = cur;
                while (arg->kind == IR_ARG)
                {
                    arg_cnt++;
                    arg = arg->next;
                }
            }
            char *dest = cur->dest;
            if (dest[0] == '#')
            {
                mipscode *mc = new_mipcode(3, "li", "$t0,", dest + 1, NULL);
                addmc2mcs(mc, mcs_line);
            }
            else
            {
                char *c_dest = find_offset(dest);
                mipscode *mc = new_mipcode(3, "lw", "$t0,", c_dest, NULL);
                addmc2mcs(mc, mcs_line);
            }
            if (arg_cnt >= 5)
            {
                // char *offset = (char *)malloc(sizeof(char) * 8 + 1);
                // sprintf(offset, "-%d($sp)", arg_cnt - 1)
                mipscode *mc1 = new_mipcode(4, "addi", "$sp,", "$sp", "-4");
                addmc2mcs(mc1, mcs_line);
                mipscode *mc2 = new_mipcode(3, "sw", "$t0,", "0($sp)", NULL);
                addmc2mcs(mc2, mcs_line);
            }
            else
            {
                char *c_arg = pass_arg(arg_cnt - 1, 0);
                mipscode *mc = new_mipcode(3, "move", c_arg, "$t0", NULL);
                addmc2mcs(mc, mcs_line);
            }
            arg_cnt--;
        }
        break;
        case IR_CALL:
        {
            assert(arg_cnt == 0);
            char *dest = cur->dest;
            char *arg1 = cur->arg1;
            mipscode *mc1 = new_mipcode(4, "addi", "$sp,", "$sp,", "-4");
            addmc2mcs(mc1, mcs_line);
            mipscode *mc2 = new_mipcode(3, "sw", "$ra,", "0($sp)", NULL);
            addmc2mcs(mc2, mcs_line);
            mipscode *mc3 = new_mipcode(2, "jal", arg1, NULL, NULL);
            addmc2mcs(mc3, mcs_line);
            mipscode *mc4 = new_mipcode(3, "lw", "$ra,", "0($sp)", NULL);
            addmc2mcs(mc4, mcs_line);
            mipscode *mc5 = new_mipcode(4, "addi", "$sp,", "$sp,", "4");
            addmc2mcs(mc5, mcs_line);
            FuncType *ft = lookup_Functable(arg1);
            if (ft->argc >= 5)
            {
                char *offset = (char *)malloc(sizeof(char) * 8 + 1);
                sprintf(offset, "%d", (ft->argc - 4) * 4);
                mipscode *mc = new_mipcode(4, "addi", "$sp,", "$sp,", offset);
                addmc2mcs(mc, mcs_line);
            }
            mipscode *mc6 = new_mipcode(3, "move", "$t0,", "$v0", NULL);
            addmc2mcs(mc6, mcs_line);
            char *c_dest = find_offset(dest);
            mipscode *mc7 = new_mipcode(3, "sw", "$t0,", c_dest, NULL);
            addmc2mcs(mc7, mcs_line);
        }
        break;
        case IR_PARAM:
        {
            param_cnt++;
            if (param_cnt <= 4)
            {
                char *c_arg = pass_arg(param_cnt - 1, 1);
                mipscode *mc = new_mipcode(3, "move", "$t0,", c_arg, NULL);
                addmc2mcs(mc, mcs_line);
            }
            else
            {
                char *c_arg = (char *)malloc(sizeof(char) * 8 + 1);
                sprintf(c_arg, "%d($fp)", (param_cnt - 5) * 4 + 8);
                mipscode *mc = new_mipcode(3, "lw", "$t0,", c_arg, NULL);
                addmc2mcs(mc, mcs_line);
            }
            char *c_dest = find_offset(cur->dest);
            mipscode *mc = new_mipcode(3, "sw", "$t0,", c_dest, NULL);
            addmc2mcs(mc, mcs_line);
        }
        break;
        case IR_READ:
        {
            mipscode *mc1 = new_mipcode(4, "addi", "$sp,", "$sp,", "-4");
            addmc2mcs(mc1, mcs_line);
            mipscode *mc2 = new_mipcode(3, "sw", "$ra,", "0($sp)", NULL);
            addmc2mcs(mc2, mcs_line);
            mipscode *mc3 = new_mipcode(2, "jal", "read", NULL, NULL);
            addmc2mcs(mc3, mcs_line);
            mipscode *mc4 = new_mipcode(3, "lw", "$ra,", "0($sp)", NULL);
            addmc2mcs(mc4, mcs_line);
            mipscode *mc5 = new_mipcode(4, "addi", "$sp,", "$sp,", "4");
            addmc2mcs(mc5, mcs_line);
            mipscode *mc6 = new_mipcode(3, "move", "$t0,", "$v0", NULL);
            addmc2mcs(mc6, mcs_line);
            char *c_dest = find_offset(cur->dest);
            mipscode *mc7 = new_mipcode(3, "sw", "$t0,", c_dest, NULL);
            addmc2mcs(mc7, mcs_line);
        }
        break;
        case IR_WRITE:
        {
            char *c_dest = find_offset(cur->dest);
            mipscode *mc = new_mipcode(3, "lw", "$a0,", c_dest, NULL);
            addmc2mcs(mc, mcs_line);
            mipscode *mc1 = new_mipcode(4, "addi", "$sp,", "$sp,", "-4");
            addmc2mcs(mc1, mcs_line);
            mipscode *mc2 = new_mipcode(3, "sw", "$ra,", "0($sp)", NULL);
            addmc2mcs(mc2, mcs_line);
            mipscode *mc3 = new_mipcode(2, "jal", "write", NULL, NULL);
            addmc2mcs(mc3, mcs_line);
            mipscode *mc4 = new_mipcode(3, "lw", "$ra,", "0($sp)", NULL);
            addmc2mcs(mc4, mcs_line);
            mipscode *mc5 = new_mipcode(4, "addi", "$sp,", "$sp,", "4");
            addmc2mcs(mc5, mcs_line);
        }
        break;
        default:
            printf("something went wrong\n");
            exit(1);
        }
        appendmcs2mcs(mcs, mcs_line);
        cur = cur->next;
    }
    return mcs;
}

void init_mipscodes(mipscodes *mcs)
{
    mipscode *mc1 = new_mipcode(1, ".data", NULL, NULL, NULL);
    addmc2mcs(mc1, mcs);
    mipscode *mc2 = new_mipcode(3, "_ret:", ".asciiz", "\"\\n\"", NULL);
    addmc2mcs(mc2, mcs);
    // mipscode *mc3 = new_mipcode(2, ".global", "main", NULL, NULL);
    // addmc2mcs(mc3, mcs);
    mipscode *mc4 = new_mipcode(1, ".text", NULL, NULL, NULL);
    addmc2mcs(mc4, mcs);
    mipscode *mc5 = new_mipcode(1, "read:", NULL, NULL, NULL);
    addmc2mcs(mc5, mcs);
    mipscode *mc6 = new_mipcode(3, "li", "$v0,", "5", NULL);
    addmc2mcs(mc6, mcs);
    mipscode *mc7 = new_mipcode(1, "syscall", NULL, NULL, NULL);
    addmc2mcs(mc7, mcs);
    // mipscode *mc8 = new_mipcode(3, "move", "$v0,", "$0", NULL);
    // addmc2mcs(mc8, mcs);
    mipscode *mc9 = new_mipcode(2, "jr", "$ra", NULL, NULL);
    addmc2mcs(mc9, mcs);
    mipscode *mc10 = new_mipcode(1, "write:", NULL, NULL, NULL);
    addmc2mcs(mc10, mcs);
    mipscode *mc11 = new_mipcode(3, "li", "$v0,", "1", NULL);
    addmc2mcs(mc11, mcs);
    mipscode *mc12 = new_mipcode(1, "syscall", NULL, NULL, NULL);
    addmc2mcs(mc12, mcs);
    mipscode *mc13 = new_mipcode(3, "li", "$v0,", "4", NULL);
    addmc2mcs(mc13, mcs);
    mipscode *mc14 = new_mipcode(3, "la", "$a0,", "_ret", NULL);
    addmc2mcs(mc14, mcs);
    mipscode *mc15 = new_mipcode(1, "syscall", NULL, NULL, NULL);
    addmc2mcs(mc15, mcs);
    mipscode *mc16 = new_mipcode(3, "move", "$v0,", "$0", NULL);
    addmc2mcs(mc16, mcs);
    mipscode *mc17 = new_mipcode(2, "jr", "$ra", NULL, NULL);
    addmc2mcs(mc17, mcs);
}

int calculate_offset(IR *ir)
{
    assert(ir->kind == IR_FUNC);
    IR *cur = ir->next;
    int offset = 0;
    while (cur != NULL && cur->kind != IR_FUNC)
    {
        switch (cur->kind)
        {
        case IR_DEC:
        {
            char *name = cur->dest;
            int size = atoi(cur->arg1);
            Var_or_Struct *var = lookup_Vartable(name);
            assert(var != NULL);
            var->offset = offset;
            offset += size;
        }
        break;
        case IR_ASSIGN_1:
        case IR_ASSIGN_2:
        case IR_ASSIGN_ADDR:
        case IR_ASSIGN_MADDR:
        case IR_CALL:
        case IR_PARAM:
        case IR_READ:
        {
            char *name = cur->dest;
            Var_or_Struct *var = lookup_Vartable(name);
            if (var == NULL) // 表示这是生成中间代码过程中的新的临时变量
            {
                Var_or_Struct *new_var = get_Var(name, NULL);
                insert_Vartable(new_var);
                var = lookup_Vartable(name);
            }
            assert(var != NULL);
            if (var->offset == -1) // 表示尚未在栈上分配空间
            {
                var->offset = offset;
                offset += 4;
            }
        }
        break;
        default:
            break;
        }
        cur = cur->next;
    }
    return offset;
}

char *find_offset(char *name)
{
    Var_or_Struct *var = lookup_Vartable(name);
    assert(var != NULL);
    int offset = var->offset;
    if (offset == -1)
    {
        return NULL;
    }
    else
    {
        char *c_offset = (char *)malloc(sizeof(char) * 12 + 1);
        sprintf(c_offset, "%d($fp)", -offset - 4);
        return c_offset;
    }
}

char *pass_arg(int cnt, int kind)
{
    char *c_arg = (char *)malloc(sizeof(char) * 8 + 1);
    if (kind == 0)
    {
        sprintf(c_arg, "$a%d,", cnt);
    }
    else
    {
        sprintf(c_arg, "$a%d", cnt);
    }
    return c_arg;
}