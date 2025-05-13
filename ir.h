#ifndef IR_H
#define IR_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct IR IR;
typedef struct Code Code;
typedef enum
{
    IR_LABEL,
    IR_FUNC,
    IR_ASSIGN_1,
    IR_ASSIGN_2,
    IR_ASSIGN_ADDR,
    IR_ASSIGN_MADDR,
    IR_MADDR_ASSIGN,
    IR_GOTO,
    IR_IF_GOTO,
    IR_RET,
    IR_DEC,
    IR_ARG,
    IR_CALL,
    IR_PARAM,
    IR_READ,
    IR_WRITE
} IR_kind;

struct IR
{
    IR_kind kind;
    char *dest;
    char *arg1;
    char *op;
    char *arg2;
    IR *next;
};

struct Code
{
    IR *head, *tail;
};

IR *new_IR(int kind, char *dest, char *arg1, char *op, char *arg2);
void print_IR(FILE *f, IR *ir);

Code *new_Code();
void addIR2Code(Code *code, IR *ir);
void appendCode2Code(Code *code1, Code *code2); // 新的code1包括老的code1和code2两部分
void print_Code(FILE *f, Code *code);

char *new_label();
char *new_temp();
#endif