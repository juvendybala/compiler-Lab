#include "ast.h"

Node *create_node(int is_leaf, const char *name, int line, char *content, int base)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->is_leaf = is_leaf;
    int name_len = strlen(name);
    node->name = (char *)malloc(name_len + 1);
    strcpy(node->name, name);
    node->line = line;
    if (strcmp("ID", name) == 0 || strcmp("TYPE", name) == 0 || strcmp("RELOP", name) == 0)
    {
        int content_len = strlen(content);
        node->data.string_value = (char *)malloc(content_len + 1);
        strcpy(node->data.string_value, content);
        // if (strcmp("TYPE", name) == 0)
        // {
        //     printf("%s %s\n", node->data.string_value, content);
        // }
    }
    else if (strcmp("INT", name) == 0)
    {
        node->data.int_value = strtol(content, NULL, base);
    }
    else if (strcmp("FLOAT", name) == 0)
    {
        node->data.float_value = strtof(content, NULL);
    }
    return node;
}

void set_node_childs(Node *now, int num, ...)
{
    if (!now)
    {
        printf("1\n");
        return;
    }
    // printf("1\n");
    // printf("1\n");
    // printf("now = %p\n", (void *)now);
    now->num = num;
    now->childs = (Node **)malloc(num * sizeof(Node *));
    va_list args;
    va_start(args, num);

    for (int i = 0; i < num; i++)
    {
        now->childs[i] = va_arg(args, Node *);
    }

    va_end(args);
}

void print_ast(Node *root, int indentation)
{
    if (root == NULL)
    {
        return;
    }
    for (int i = 0; i < indentation; i++)
    {
        printf(" ");
    }
    if (root->is_leaf == 1)
    {
        if (strcmp("ID", root->name) == 0)
        {
            printf("ID: %s\n", root->data.string_value);
        }
        else if (strcmp("TYPE", root->name) == 0)
        {
            printf("TYPE: %s\n", root->data.string_value);
        }
        else if (strcmp("INT", root->name) == 0)
        {
            printf("INT: %d\n", root->data.int_value);
        }
        else if (strcmp("FLOAT", root->name) == 0)
        {
            printf("FLOAT: %f\n", root->data.float_value);
        }
        else
        {
            printf("%s\n", root->name);
        }
    }
    else
    {
        printf("%s (%d)\n", root->name, root->line);
        for (int i = 0; i < root->num; i++)
        {
            // printf("  ");
            print_ast(root->childs[i], indentation + 2);
        }
    }
}