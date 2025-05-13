#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct Node Node;

struct Node
{
    int is_leaf;
    char *name; // 代表该节点的名字
    int line;   // 代表行号
    int num;    // 代表子女的数量
    Node **childs;
    union
    {
        int int_value;
        float float_value;
        char *string_value;
    } data;
};

Node *create_node(int is_leaf, const char *name, int line, char *content, int base);

void set_node_childs(Node *now, int num, ...);

void print_ast(Node *root, int indentation);

#endif