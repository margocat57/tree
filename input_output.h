#ifndef INPUT_OUTPUT_H
#define INPUT_OUTPUT_H
#include "tree.h"

TreeHead_t* MakeAkinatorTree(const char *name_of_file);

TreeErr_t PutAkinatorFile(const char* file_name, TreeNode_t *node, const TreeHead_t* head);

void FreeMemoryAtAkinatorTree(TreeHead_t* head);

#endif // INPUT_OUTPUT_H