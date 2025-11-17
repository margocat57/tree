#ifndef TREE_FUNC_H
#define TREE_FUNC_H
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "tree.h"
#include "stack_for_akinator/stack_func.h"
#include "stack_for_akinator/stack.h"

const int LEFT = 1;
const int RIGHT = 0;

TreeHead_t* TreeCtor(char* buffer);

TreeNode_t* NodeCtor(TreeElem_t data, TreeNode_t* parent, TreeNode_t* left, TreeNode_t* right, bool need_data_free);

TreeErr_t TreeDelNodeRecur(TreeNode_t* node, TreeHead_t* head);

TreeErr_t TreeDel(TreeHead_t* head);

TreeErr_t CreatePath(TreeNode_t* node, TreeHead_t* head, const char* name, stack_t_t* stack, bool *found);

TreeErr_t PrintNode(const TreeNode_t* node, const TreeHead_t* head, FILE* dot_file, int* rank);

TreeErr_t TreeNodeVerify(const TreeNode_t *node, const TreeHead_t* head);

TreeErr_t TreeVerify(const TreeHead_t* head);

void NodeDtor(TreeNode_t* node);

void SayAndPrintSaid(const char* color, const char* format, ...) __attribute__ ((format (printf, 2, 3)));

#endif //TREE_FUNC_H