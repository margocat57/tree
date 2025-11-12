#ifndef AKINATOR_H
#define AKINATOR_H
#include "tree.h"

TreeErr_t TreeMakeDefinition(TreeHead_t* head, const char* name);

TreeErr_t TreeAkinate(TreeHead_t* head);

TreeErr_t TreeAddFirstQuestion(TreeHead_t* head);

TreeErr_t TreeFindCommonOpposite(TreeHead_t* head, const char* name1, const char* name2);

#endif // AKINATOR_H