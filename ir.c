#include "ir.h"

int label_num = 0;
int temp_num = 0;

IR *new_IR(int kind, char *dest, char *arg1, char *op, char *arg2)
{
    IR *ir = (IR *)malloc(sizeof(IR));
    ir->kind = kind;
    ir->dest = dest;
    ir->arg1 = arg1;
    ir->op = op;
    ir->arg2 = arg2;
    ir->next = NULL;
    return ir;
}

void print_IR(FILE *f, IR *ir)
{
    switch (ir->kind)
    {
    case IR_LABEL:
        fprintf(f, "LABEL %s :", ir->dest);
        break;
    case IR_FUNC:
        fprintf(f, "FUNCTION %s :", ir->dest);
        break;
    case IR_ASSIGN_1:
        fprintf(f, "%s := %s", ir->dest, ir->arg1);
        break;
    case IR_ASSIGN_2:
        fprintf(f, "%s := %s %s %s", ir->dest, ir->arg1, ir->op, ir->arg2);
        break;
    case IR_ASSIGN_ADDR:
        fprintf(f, "%s := &%s", ir->dest, ir->arg1);
        break;
    case IR_ASSIGN_MADDR:
        fprintf(f, "%s := *%s", ir->dest, ir->arg1);
        break;
    case IR_MADDR_ASSIGN:
        fprintf(f, "*%s := %s", ir->dest, ir->arg1);
        break;
    case IR_GOTO:
        fprintf(f, "GOTO %s", ir->dest);
        break;
    case IR_IF_GOTO:
        fprintf(f, "IF %s %s %s GOTO %s", ir->arg1, ir->op, ir->arg2, ir->dest);
        break;
    case IR_RET:
        fprintf(f, "RETURN %s", ir->dest);
        break;
    case IR_DEC:
        fprintf(f, "DEC %s %s", ir->dest, ir->arg1);
        break;
    case IR_ARG:
        fprintf(f, "ARG %s", ir->dest);
        break;
    case IR_CALL:
        fprintf(f, "%s := CALL %s", ir->dest, ir->arg1);
        break;
    case IR_PARAM:
        fprintf(f, "PARAM %s", ir->dest);
        break;
    case IR_READ:
        fprintf(f, "READ %s", ir->dest);
        break;
    case IR_WRITE:
        fprintf(f, "WRITE %s", ir->dest);
        break;
    default:
        printf("something went wrong\n");
    }
    fprintf(f, "\n");
}

Code *new_Code()
{
    Code *code = (Code *)malloc(sizeof(Code));
    code->head = code->tail = NULL;
    return code;
}

void addIR2Code(Code *code, IR *ir)
{
    assert(code != NULL);
    assert(ir != NULL);
    if (code->head == NULL)
    {
        code->head = code->tail = ir;
    }
    else
    {
        code->tail->next = ir;
        code->tail = ir;
    }
}

void appendCode2Code(Code *code1, Code *code2)
{
    assert(code1 != NULL);
    assert(code2 != NULL);
    if (code1->head == NULL)
    {
        code1->head = code2->head;
        code1->tail = code2->tail;
    }
    else if (code2->head == NULL)
    {
        return;
    }
    else
    {
        code1->tail->next = code2->head;
        code1->tail = code2->tail;
    }
}

void print_Code(FILE *f, Code *code)
{
    IR *cur = code->head;
    while (cur != NULL)
    {
        print_IR(f, cur);
        cur = cur->next;
    }
}

char *new_label()
{
    label_num++;
    char *label = (char *)malloc(1 + 8 * sizeof(char));
    sprintf(label, "L%d", label_num);
    return label;
}

char *new_temp()
{
    temp_num++;
    char *temp = (char *)malloc(1 + 8 * sizeof(char));
    sprintf(temp, "t%d", temp_num);
    return temp;
}