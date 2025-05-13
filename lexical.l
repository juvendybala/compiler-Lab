%option yylineno
%{
    #include <stdio.h>
    #include "syntax.tab.h" 
    #include "ast.h"
    extern int error;
    extern int iserror[1000];
    int yycolumn = 1;
    #define YY_USER_ACTION\
        yylloc.first_line = yylloc.last_line = yylineno;\
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1;\
        yycolumn += yyleng;
%}

delim [ \t\r]
ws {delim}+
newline "\n"
digit [0-9]
letter [_a-zA-Z]
decimal {digit}|([1-9]{digit}+)
octal 0[1-7][0-7]*|00
hexdecimal (0x|0X)[1-9a-fA-F][0-9a-fA-F]*|0x0|0X0
FLOAT {decimal}\.{digit}+
ID {letter}({letter}|{digit})*
%%
{ws} {}
{newline} {yycolumn = 1;}
";" {yylval.node = create_node(1, "SEMI", yylineno, NULL, -1);return SEMI;}
"," {yylval.node = create_node(1, "COMMA", yylineno, NULL, -1);return COMMA;}
"=" {yylval.node = create_node(1, "ASSIGNOP", yylineno, NULL, -1);return ASSIGNOP;}
"<"|">"|">="|"<="|"=="|"!=" {yylval.node = create_node(1, "RELOP", yylineno, yytext, -1);return RELOP;}
"+" {yylval.node = create_node(1, "PLUS", yylineno, NULL, -1);return PLUS;}
"-" {yylval.node = create_node(1, "MINUS", yylineno, NULL, -1);return MINUS;}
"*" {yylval.node = create_node(1, "STAR", yylineno, NULL, -1);return STAR;}
"/" {yylval.node = create_node(1, "DIV", yylineno, NULL, -1);return DIV;}
"&&" {yylval.node = create_node(1, "AND", yylineno, NULL, -1);return AND;}
"||" {yylval.node = create_node(1, "OR", yylineno, NULL, -1);return OR;}
"." {yylval.node = create_node(1, "DOT", yylineno, NULL, -1);return DOT;}
"!" {yylval.node = create_node(1, "NOT", yylineno, NULL, -1);return NOT;}
"int" {yylval.node = create_node(1, "TYPE", yylineno, yytext, -1);return TYPE;}
"float" {yylval.node = create_node(1, "TYPE", yylineno, yytext, -1);return TYPE;}
"(" {yylval.node = create_node(1, "LP", yylineno, NULL, -1);return LP;}
")" {yylval.node = create_node(1, "RP", yylineno, NULL, -1);return RP;}
"[" {yylval.node = create_node(1, "LB", yylineno, NULL, -1);return LB;}
"]" {yylval.node = create_node(1, "RB", yylineno, NULL, -1);return RB;}
"{" {yylval.node = create_node(1, "LC", yylineno, NULL, -1);return LC;}
"}" {yylval.node = create_node(1, "RC", yylineno, NULL, -1);return RC;}
"struct" {yylval.node = create_node(1, "STRUCT", yylineno, NULL, -1);return STRUCT;}
"return" {yylval.node = create_node(1, "RETURN", yylineno, NULL, -1);return RETURN;}
"if" {yylval.node = create_node(1, "IF", yylineno, NULL, -1);return IF;}
"else" {yylval.node = create_node(1, "ELSE", yylineno, NULL, -1);return ELSE;}
"while" {yylval.node = create_node(1, "WHILE", yylineno, NULL, -1);return WHILE;}
{FLOAT} {yylval.node = create_node(1, "FLOAT", yylineno, yytext, -1);return FLOAT;}
{octal} {yylval.node = create_node(1, "INT", yylineno, yytext, 8);return INT;}
{hexdecimal} {yylval.node = create_node(1, "INT", yylineno, yytext, 16);return INT;}
{decimal} {yylval.node = create_node(1, "INT", yylineno, yytext, 10);return INT;}
{ID} {yylval.node = create_node(1, "ID", yylineno, yytext, -1);return ID;}
. {if(iserror[yylineno] == 0){error = 1; printf("Error type A at Line %d: Mysterious character \"%s\".\n", yylineno, yytext); iserror[yylineno] = 1;}}
%%