#include <stdio.h>
#include "ast.h"
#include "typechecking.h"
#include "translate.h"
#include "generate.h"
extern FILE *yyin;
// extern int yylex();
extern int yyparse();

Node *root = NULL;
int error = 0;
int has_struct = 0;

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        return 1;
    }
    if (!(yyin = fopen(argv[1], "r")))
    {
        perror(argv[1]);
        return 1;
    }
    yyparse();
    // printf("parse end\n");
    fclose(yyin);
    if (error)
    {
        return 0;
    }
    else
    {
        // print_ast(root, 0);
        add_func();
        ExtDefList(root->childs[0]);
    }
    if (has_struct == 0)
    {
        // printf("translation starts\n");
        Code *code = translate_ExtDefList(root->childs[0]);
        // printf("translation ends\n");
        FILE *f = fopen(argv[2], "w");
        if (!f)
        {
            perror(argv[2]);
            return 1;
        }
        // print_Code(f, code);
        mipscodes *mcs = transcode2mipscodes(code);
        print_mipscodes(f, mcs);
    }
    else
    {
        printf("Cannot translate because code contains struct\n");
    }
    return 0;
}
