#ifndef GENERATE_H
#define GENERATE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mipscode.h"
#include "ir.h"
#include "symboltable.h"

mipscodes *transcode2mipscodes(Code *code);
void init_mipscodes(mipscodes *mcs);
int calculate_offset(IR *ir);
char *find_offset(char *name);
char *pass_arg(int cnt, int kind);
#endif