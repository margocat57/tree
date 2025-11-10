#ifndef TREE_FUNC_H
#define TREE_FUNC_H
#include <stdio.h>
#include "tree.h"

TreeHead_t* TreeCtor();

TreeNode_t* NodeCtor(TreeElem_t data, TreeNode_t* parent, TreeNode_t* left, TreeNode_t* right);

TreeErr_t TreeDelNodeRecur(TreeNode_t* node, TreeHead_t* head);

TreeErr_t TreeDel(TreeHead_t* head);

TreeErr_t TreeFindNode(TreeNode_t* node, TreeHead_t* head, const char* name, TreeNode_t** node_ptr);

TreeNode_t* TreeFindCommonNode(TreeHead_t* head, TreeNode_t* node1, TreeNode_t* node2);

TreeErr_t PrintNode(const TreeNode_t* node, const TreeHead_t* head, FILE* dot_file, int* rank);

TreeErr_t TreeNodeVerify(const TreeNode_t *node, const TreeHead_t* head);

TreeErr_t TreeVerify(const TreeHead_t* head);

void NodeDtor(TreeNode_t* node);

#endif //TREE_FUNC_H