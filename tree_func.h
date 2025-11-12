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

TreeErr_t TreeFindNode(TreeNode_t* node, TreeHead_t* head, const char* name, TreeNode_t** node_ptr);

stack_t_t* TreeFindPathToNode(TreeNode_t* node, TreeNode_t* node_dst);

TreeNode_t* TreeFindCommonNode(TreeHead_t* head, TreeNode_t* node1, TreeNode_t* node2);

TreeErr_t PrintNode(const TreeNode_t* node, const TreeHead_t* head, FILE* dot_file, int* rank);

TreeErr_t TreeNodeVerify(const TreeNode_t *node, const TreeHead_t* head);

TreeErr_t TreeVerify(const TreeHead_t* head);

void NodeDtor(TreeNode_t* node);

void SayAndPrintSaid(const char* format, ...) __attribute__ ((format (printf, 1, 2)));

#endif //TREE_FUNC_H