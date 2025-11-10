#ifndef AKINATOR_H
#define AKINATOR_H
#include "tree.h"

TreeErr_t TreeMakeDefinition(TreeHead_t* head, const char* name);

TreeErr_t TreeAkinate(TreeNode_t* node, TreeHead_t* head);

TreeErr_t TreeAddQuestion(TreeNode_t* node, TreeHead_t* head);

TreeErr_t TreeAddFirstQuestion(TreeHead_t* head);

TreeErr_t TreeFindCommonOpposite(TreeHead_t* head, const char* name1, const char* name2);

TreeErr_t PutAkinatorFile(const char* file_name, TreeNode_t *node, const TreeHead_t* head);

#endif // AKINATOR_H