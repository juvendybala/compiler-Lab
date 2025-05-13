#include "mipscode.h"

mipscode *new_mipcode(int num, char *op, char *operand1, char *operand2, char *operand3)
{
    mipscode *mc = (mipscode *)malloc(sizeof(mipscode));
    mc->num = num;
    mc->op = op;
    mc->operand1 = operand1;
    mc->operand2 = operand2;
    mc->operand3 = operand3;
    mc->next = NULL;
    return mc;
}

void print_mipcode(FILE *f, mipscode *mc)
{
    switch (mc->num)
    {
    case 1:
        fprintf(f, "%s", mc->op);
        break;
    case 2:
        fprintf(f, "%s %s", mc->op, mc->operand1);
        break;
    case 3:
        fprintf(f, "%s %s %s", mc->op, mc->operand1, mc->operand2);
        break;
    case 4:
        fprintf(f, "%s %s %s %s", mc->op, mc->operand1, mc->operand2, mc->operand3);
        break;
    case 5:
        break;
    default:
        printf("Invalid num\n");
        exit(1);
    }
    fprintf(f, "\n");
}

mipscodes *new_mipcodes()
{
    mipscodes *mcs = (mipscodes *)malloc(sizeof(mipscodes));
    mcs->head = mcs->tail = NULL;
    return mcs;
}

void addmc2mcs(mipscode *mc, mipscodes *mcs)
{
    assert(mcs != NULL);
    assert(mc != NULL);
    if (mcs->head == NULL)
    {
        mcs->head = mcs->tail = mc;
    }
    else
    {
        mcs->tail->next = mc;
        mcs->tail = mc;
    }
}

void appendmcs2mcs(mipscodes *mcs1, mipscodes *mcs2)
{
    assert(mcs1 != NULL);
    assert(mcs2 != NULL);
    if (mcs1->head == NULL)
    {
        mcs1->head = mcs2->head;
        mcs1->tail = mcs2->tail;
    }
    else if (mcs2->head == NULL)
    {
        return;
    }
    else
    {
        mcs1->tail->next = mcs2->head;
        mcs1->tail = mcs2->tail;
    }
}

void print_mipscodes(FILE *f, mipscodes *mcs)
{
    mipscode *cur = mcs->head;
    while (cur != NULL)
    {
        print_mipcode(f, cur);
        cur = cur->next;
    }
}