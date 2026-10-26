#pragma once

#include "../glframework/core.h"

// 预编译宏
#ifdef DEBUG
#define GL_CALL(func) func;CheckError();
#else
#define GL_CALL(func) func;
#endif
void CheckError();