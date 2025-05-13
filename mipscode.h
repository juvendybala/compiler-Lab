#ifndef MIPSCODE_H
#define MIPSCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct mipscode mipscode;
typedef struct mipscodes mipscodes;

struct mipscode
{
    int num; // 标定code的字节数，方便输出
    char *op;
    char *operand1;
    char *operand2;
    char *operand3;
    mipscode *next;
};

struct mipscodes
{
    mipscode *head;
    mipscode *tail;
};

mipscode *new_mipcode(int num, char *op, char *operand1, char *operand2, char *operand3);
void print_mipcode(FILE *f, mipscode *mc);

mipscodes *new_mipcodes();
void addmc2mcs(mipscode *mc, mipscodes *mcs);
void appendmcs2mcs(mipscodes *mcs1, mipscodes *mcs2);
void print_mipscodes(FILE *f, mipscodes *mcs);
#endif