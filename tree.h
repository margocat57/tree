#ifndef TREE_H
#define TREE_H
#include <stdio.h>
#include <stdint.h>

const char* const TREE_SIGNATURE = "My_tree";
const int POISON = -6666;

typedef uint64_t TreeErr_t;
typedef int TreeElem_t;

enum Err{
    NO_MISTAKE,
    NULL_NODE_PTR,
    NULL_HEAD_PTR,
    INCORRECT_SIGN,
};

struct TreeNode_t{
    TreeElem_t data;
    TreeNode_t* left;
    TreeNode_t* right;
    TreeNode_t* parent;
    const char* signature;
};

struct TreeHead_t{
    TreeNode_t* root;
    size_t capacity;
};

TreeNode_t* NodeCtor(TreeElem_t data, TreeNode_t* parent, TreeNode_t* left, TreeNode_t* right);

TreeErr_t TreeAddNode(TreeElem_t elem, TreeHead_t* head);

TreeErr_t TreeDelNode(TreeNode_t* node, TreeHead_t* head);

TreeErr_t TreeDel(TreeHead_t* head);

TreeErr_t PrintNode(const TreeNode_t* node, FILE* dot_file, int* rank);

TreeErr_t TreeVerify(const TreeNode_t *node);

void NodeDtor(TreeNode_t* node);

#endif // TREE_H