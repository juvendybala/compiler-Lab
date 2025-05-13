%locations
%{
    #include <stdio.h>
    #include "ast.h"
    #include "lex.yy.c"
    extern int error;
    extern Node *root;
    int iserror[1000] = {0};
    void yyerror(const char *msg)
    {
        if(iserror[yylineno] == 0){
            printf("Error type B at Line %d: %s\n", yylineno, msg);
            iserror[yylineno] = 1;
        }
        
    }
    #define YYERROR_VERBOSE 1
%}

/* %code requires {
    #include "ast.h"
} */

%union{
    Node* node;
}

%token<node> INT FLOAT
%token<node> ID
%token<node> SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT UMINUS
%left LP RP LB RB DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%type<node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args
%%
Program : ExtDefList{$$ = create_node(0, "Program", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);root = $$;}
    ;
ExtDefList : ExtDef ExtDefList{$$ = create_node(0, "ExtDefList", @$.first_line, NULL, -1); set_node_childs($$, 2, $1, $2);}
    | {$$ = NULL;}
    ;
ExtDef : Specifier ExtDecList SEMI{$$ = create_node(0, "ExtDef", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | Specifier SEMI{$$ = create_node(0, "ExtDef", @$.first_line, NULL, -1); set_node_childs($$, 2, $1, $2);}
    | Specifier FunDec CompSt{$$ = create_node(0, "ExtDef", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | error SEMI{$$ = NULL; error = 1;}
    | Specifier FunDec error SEMI{$$ = NULL; error = 1;}
    | Specifier error{$$ = NULL; error = 1;}
    ;
ExtDecList : VarDec{$$ = create_node(0, "ExtDecList", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);}
    | VarDec COMMA ExtDecList{$$ = create_node(0, "ExtDecList", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | VarDec error ExtDecList{$$ = NULL; error = 1;}
    | VarDec error{$$ = NULL; error = 1;}
    | error COMMA ExtDecList{$$ = NULL; error = 1;}
    ;
Specifier : TYPE{$$ = create_node(0, "Specifier", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);}
    | StructSpecifier{$$ = create_node(0, "Specifier", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);}
    ;
StructSpecifier : STRUCT OptTag LC DefList RC{$$ = create_node(0, "StructSpecifier", @$.first_line, NULL, -1); set_node_childs($$, 5, $1, $2, $3, $4, $5);}
    | STRUCT Tag{$$ = create_node(0, "StructSpecifier", @$.first_line, NULL, -1); set_node_childs($$, 2, $1, $2);}
    | STRUCT OptTag LC error RC{ $$ = NULL; error = 1;}
    ;
OptTag : ID{$$ = create_node(0, "OptTag", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);}
    | {$$ = NULL;}
    ;
Tag : ID{$$ = create_node(0, "Tag", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);}
    ;
VarDec : ID{$$ = create_node(0, "VarDec", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);}
    | VarDec LB INT RB{$$ = create_node(0, "VarDec", @$.first_line, NULL, -1); set_node_childs($$, 4, $1, $2, $3, $4);}
    | VarDec LB error RB{$$ = NULL; error = 1;}
    | VarDec LB error{$$ = NULL; error = 1;}
    ;
FunDec : ID LP VarList RP{$$ = create_node(0, "FunDec", @$.first_line, NULL, -1); set_node_childs($$, 4, $1, $2, $3, $4);}
    | ID LP RP{$$ = create_node(0, "FunDec", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | error LP VarList RP{$$ = NULL; error = 1;}
    | ID LP error RP{$$ = NULL; error = 1;}
    ;
VarList : ParamDec COMMA VarList{$$ = create_node(0, "VarList", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | ParamDec {$$ = create_node(0, "VarList", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);}
    | error COMMA VarList{$$ = NULL; error = 1;}
    ;
ParamDec : Specifier VarDec{$$ = create_node(0, "ParamDec", @$.first_line, NULL, -1); set_node_childs($$, 2, $1, $2);}
    ;
CompSt : LC DefList StmtList RC{$$ = create_node(0, "CompSt", @$.first_line, NULL, -1); set_node_childs($$, 4, $1, $2, $3, $4);}
    | error RC{$$ = NULL; error = 1;}
    ;
StmtList : Stmt StmtList{$$ = create_node(0, "StmtList", @$.first_line, NULL, -1); set_node_childs($$, 2, $1, $2);}
    |{$$ = NULL;}
    ;
Stmt : Exp SEMI{$$ = create_node(0, "Stmt", @$.first_line, NULL, -1); set_node_childs($$, 2, $1, $2);}
    | CompSt{$$ = create_node(0, "Stmt", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);}
    | RETURN Exp SEMI{$$ = create_node(0, "Stmt", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE{$$ = create_node(0, "Stmt", @$.first_line, NULL, -1); set_node_childs($$, 5, $1, $2, $3, $4, $5);}
    | IF LP Exp RP Stmt ELSE Stmt{$$ = create_node(0, "Stmt", @$.first_line, NULL, -1); set_node_childs($$, 7, $1, $2, $3, $4, $5, $6, $7);} 
    | WHILE LP Exp RP Stmt {{$$ = create_node(0, "Stmt", @$.first_line, NULL, -1); set_node_childs($$, 5, $1, $2, $3, $4, $5);}}
    | error SEMI{$$ = NULL; error = 1;}
    | RETURN Exp error{$$ = NULL; error = 1;}
    | RETURN error SEMI{$$ = NULL; error = 1;}
    ;
DefList : Def DefList{$$ = create_node(0, "DefList", @$.first_line, NULL, -1); set_node_childs($$, 2, $1, $2);}
    |{$$ = NULL;}
    ;
Def : Specifier DecList SEMI{$$ = create_node(0, "Def", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | Specifier error SEMI{$$ = NULL; error = 1;}
    ;
DecList : Dec{$$ = create_node(0, "DecList", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);}
    | Dec COMMA DecList{$$ = create_node(0, "DecList", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | Dec error DecList{$$ = NULL; error = 1;}
    | Dec error{$$ = NULL; error = 1;}
    ;
Dec : VarDec{$$ = create_node(0, "Dec", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);}
    | VarDec ASSIGNOP Exp{$$ = create_node(0, "Dec", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | VarDec ASSIGNOP error{$$ = NULL; error = 1;}
    ;
Exp : Exp ASSIGNOP Exp{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | Exp AND Exp{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | Exp OR Exp{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | Exp RELOP Exp{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | Exp PLUS Exp{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | Exp MINUS Exp{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | Exp STAR Exp{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | Exp DIV Exp{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | LP Exp RP{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | MINUS Exp %prec UMINUS{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 2, $1, $2);}
    | NOT Exp{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 2, $1, $2);}
    | ID LP Args RP{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 4, $1, $2, $3, $4);}
    | ID LP RP{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | Exp LB Exp RB{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 4, $1, $2, $3, $4);}
    | Exp DOT ID{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | ID{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);}
    | INT{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);}
    | FLOAT{$$ = create_node(0, "Exp", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);}
    | Exp ASSIGNOP error{$$ = NULL; error = 1;}
    | Exp AND error {$$ = NULL; error = 1;}
    | Exp OR error {$$ = NULL; error = 1;}
    | Exp RELOP error {$$ = NULL; error = 1;}
    | Exp PLUS error{$$ = NULL;error = 1;}
    | Exp MINUS error {$$ = NULL;error = 1;}
    | Exp STAR error {$$ = NULL; error = 1;}
    | Exp DIV error{$$ = NULL; error = 1;}
    | Exp DOT error{$$=NULL; error = 1;}
    | LP error RP{$$ = NULL; error  = 1;}
    | MINUS error %prec UMINUS{$$ = NULL; error = 1;}
    | NOT error{$$ = NULL; error = 1;}
    | ID LP error RP{$$=NULL; error = 1;}
    | Exp LB error RB{$$ = NULL; error = 1;}
    ;
Args : Exp COMMA Args{$$ = create_node(0, "Args", @$.first_line, NULL, -1); set_node_childs($$, 3, $1, $2, $3);}
    | Exp{$$ = create_node(0, "Args", @$.first_line, NULL, -1); set_node_childs($$, 1, $1);}
    ;
%%