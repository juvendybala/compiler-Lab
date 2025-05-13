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
#endif