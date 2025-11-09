#ifndef TREE_H
#define TREE_H
#include <stdio.h>
#include <stdint.h>

const char* const TREE_SIGNATURE = "My_tree";
const int POISON = -6666;

typedef uint64_t TreeErr_t;
typedef char* TreeElem_t;

enum Err{
    NO_MISTAKE,
    NULL_NODE_PTR,
    INCORRECT_SIGN,
    INCORR_CONNECT_PARENT_CHILD,
    INCORR_LEFT_CONNECT,
    INCORR_RIGHT_CONNECT,
    INCORR_ANSWER,
    INCORR_OUTPUT_FILE,
    CANT_OPEN_OUT_FILE,
    CANT_FIND_NODE,
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

TreeHead_t* TreeCtor();

char* str_tolower(char* str);

TreeNode_t* NodeCtor(TreeElem_t data, TreeNode_t* parent, TreeNode_t* left, TreeNode_t* right);

TreeErr_t TreeDelNodeRecur(TreeNode_t* node, TreeHead_t* head);

TreeErr_t TreeDel(TreeHead_t* head);

TreeErr_t TreeFindNode(TreeNode_t* node, TreeHead_t* head, const char* name, TreeNode_t** node_ptr);

TreeErr_t TreeMakeDefinition(TreeHead_t* head, const char* name);

TreeErr_t TreeAkinate(TreeNode_t* node, TreeHead_t* head);

TreeErr_t TreeAddQuestion(TreeNode_t* node, TreeHead_t* head);

TreeErr_t TreeAddFirstQuestion(TreeHead_t* head);
TreeErr_t TreeFindCommonOpposite(TreeHead_t* head, const char* name1, const char* name2);

// TreeErr_t PrintIncorrNode(const TreeNode_t* node, FILE* dot_file, int* rank);

TreeErr_t PrintNode(const TreeNode_t* node, const TreeHead_t* head, FILE* dot_file, int* rank);

TreeErr_t TreeNodeVerify(const TreeNode_t *node, const TreeHead_t* head);

TreeErr_t TreeVerify(const TreeHead_t* head);

void NodeDtor(TreeNode_t* node);

TreeErr_t PutAkinatorFile(const char* file_name, TreeNode_t *node, const TreeHead_t* head);

#endif // TREE_H