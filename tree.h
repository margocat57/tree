#ifndef TREE_H
#define TREE_H
#include <stdint.h>
#include <string.h>

const char* const TREE_SIGNATURE = "My_tree";
const int POISON = -6666;
const char* const NOTHING = "NOTHING";

typedef uint64_t TreeErr_t;
typedef const char* TreeElem_t;

enum Err{
    NO_MISTAKE_T,
    NULL_NODE_PTR,
    INCORRECT_SIGN,
    INCORR_CONNECT_PARENT_CHILD,
    INCORR_LEFT_CONNECT,
    INCORR_RIGHT_CONNECT,
    INCORR_ANSWER,
    INCORR_OUTPUT_FILE,
    CANT_OPEN_OUT_FILE,
    CANT_FIND_NODE,
    CANT_FIND_PATH,
    INCORR_AKINATE_CONNECT,
};

struct TreeNode_t{
    TreeElem_t data;
    TreeNode_t* left;
    TreeNode_t* right;
    TreeNode_t* parent;
    const char* signature;
    bool need_data_free;
};

struct TreeHead_t{
    char* buffer;
    TreeNode_t* root;
    size_t capacity;
};


#endif //TREE_H