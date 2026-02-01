#pragma once
#include <Arduino.h>
#include "VarList.h"

#define VARS_TYPE_  int32_t

using Vars = VarList<VARS_TYPE_>;   // 
extern Vars vars;
void add_vars_commands();
