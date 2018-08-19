#ifndef __INITENGIN_H
#define __INITENGIN_H
#include "macros.h"
NS_ENGIN_BEGIN
#define LOG2MAX 1025
extern int log2Table[LOG2MAX];
void initLog2Table();
NS_ENGIN_END
#endif