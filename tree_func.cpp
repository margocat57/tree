#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tree_func.h"

static size_t TreeFindNodeDepth(TreeNode_t* node);

static void PrintNodeConnect(const TreeNode_t* node, const TreeNode_t* node_child, FILE* dot_file, int* rank);

static TreeNode_t* ReduceDepth(TreeNode_t* node, size_t* depth_change, const size_t* depth_compare);

static TreeErr_t TreeVerifyRecursive(TreeNode_t* node, const TreeHead_t* head);

TreeHead_t* TreeCtor(){
    TreeHead_t* head = (TreeHead_t*)calloc(1, sizeof(TreeHead_t));
    head->root = NodeCtor((char* const)"NOTHING", NULL, NULL, NULL);
    head->capacity = 1;

    return head;
}

TreeNode_t* NodeCtor(TreeElem_t data, TreeNode_t* parent, TreeNode_t* left, TreeNode_t* right){
    TreeNode_t* node = (TreeNode_t*)calloc(1, sizeof(TreeNode_t));
    if(!node){
        fprintf(stderr, "Can't alloc data for node");
        return NULL;
    }

    if(data){
        node->data = strdup(data);  
    } 
    else {
        node->data = NULL;
    }
    node->left = left;
    node->right = right;
    node->parent = parent;
    node->signature = TREE_SIGNATURE;

    return node;
}

void NodeDtor(TreeNode_t* node){
    if(node->data){
        memset(node->data, 0, sizeof(TreeElem_t));
        free(node->data);
        node->data = NULL;
    }
    if(node){
        memset(node, 0, sizeof(TreeNode_t));
        free(node);
    }
}

TreeErr_t TreeFindNode(TreeNode_t* node, TreeHead_t* head, const char* name, TreeNode_t** node_ptr){
    assert(name);
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(head->root, head);
    if(err) return err;

    if(!strcmp(name, node->data)){
        *node_ptr = node;
        err = TreeNodeVerify(node, head);
        return err;
    }
    if(node->left){
        TreeFindNode(node->left, head, name, node_ptr);
    }
    if(node->right){
        TreeFindNode(node->right, head, name, node_ptr);
    }

    err = TreeNodeVerify(node, head);
    return err;
}

static size_t TreeFindNodeDepth(TreeNode_t* node){
    size_t depth = 0;
    while(node){
        depth++;
        node = node->parent;
    }
    return depth;
}

static TreeNode_t* ReduceDepth(TreeNode_t* node, size_t* depth_change, const size_t* depth_compare){
    while(*depth_change > *depth_compare){
        node = node->parent;
        (*depth_change)--;
    }
    return node;
}

TreeNode_t* TreeFindCommonNode(TreeHead_t* head, TreeNode_t* node1, TreeNode_t* node2){
    size_t depth1 = TreeFindNodeDepth(node1);
    size_t depth2 = TreeFindNodeDepth(node2);

    node1 = ReduceDepth(node1, &depth1, &depth2);
    node2 = ReduceDepth(node2, &depth2, &depth1);

    while(node1 != node2 && node1 && node2){
        node1 = node1->parent;
        node2 = node2->parent;
    }
    if(!node1 || !node2){
        return head->root;
    }
    return node1;
}

TreeErr_t TreeDelNodeRecur(TreeNode_t* node, TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(node, head);
    if(err) return err;

    TreeNode_t* parent = node->parent;

    if(node->left){
        TreeDelNodeRecur(node->left, head);
    }
    if(node->right){
        TreeDelNodeRecur(node->right, head);
    }

    if(parent != NULL){
        if(parent->left == node){
            parent->left = NULL;
        } 
        if(parent->right == node){
            parent->right = NULL;
        }
    }

    NodeDtor(node);
    node = NULL;
    head->capacity--;

    return err;
}

TreeErr_t TreeDel(TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    err = TreeVerify(head);
    if(err) return err;

    TreeDelNodeRecur(head->root, head);
    memset(head, 0, sizeof(TreeHead_t));
    free(head);

    return err;
}

static void PrintNodeConnect(const TreeNode_t* node, const TreeNode_t* node_child, FILE* dot_file, int* rank){
    if(node_child->parent == node){
        fprintf(dot_file, " node_%p -> node_%p[color = \"#964B00\", dir = both];\n", node, node_child); 
    }
    else{
        fprintf(dot_file, " node_%p[shape=\"Mrecord\", style=\"filled\", fillcolor=\"#ff0000\", rank=%d, color = \"#964B00\", penwidth=1.0, label=\"{{INCORRECT NODE} | {0 | 0}} \"];\n", node_child->parent, *rank);
        fprintf(dot_file, " node_%p -> node_%p[color = \"#0000FF\"];\n", node, node_child);
        if(node_child->parent){
            fprintf(dot_file, " node_%p -> node_%p[color = \"#FF4F00\"];\n", node_child->parent, node_child);
        }
    }
    (*rank)++;
}

TreeErr_t PrintNode(const TreeNode_t* node, const TreeHead_t* head, FILE* dot_file, int* rank){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(head->root, head);
    if(err) return err;

    if(node->left){
        PrintNodeConnect(node, node->left, dot_file, rank);
        PrintNode(node->left, head, dot_file, rank);
    }

    if(node->left && node->right){
        fprintf(dot_file, " node_%p[shape=\"Mrecord\", style=\"filled\", fillcolor=\"#98FB98\", rank=%d, color = \"#964B00\", penwidth=1.0, label=\"{{%p} | {%s?} | {YES | NO }} \"];\n", node, *rank, node, node->data);
    }
    else{
        fprintf(dot_file, " node_%p[shape=\"Mrecord\", style=\"filled\", fillcolor=\"#98FB98\", rank=%d, color = \"#964B00\", penwidth=1.0, label=\"{{%p} |{%s} | {0 | 0}} \"];\n", node, *rank, node, node->data);
    }

    if(node->right){
        PrintNodeConnect(node, node->right, dot_file, rank);
        PrintNode(node->right, head, dot_file, rank);
    }
    (*rank)--;

    err = TreeNodeVerify(node, head);
    return err;
}

TreeErr_t TreeVerify(const TreeHead_t* head){
    return TreeVerifyRecursive(head->root, head);
}

static TreeErr_t TreeVerifyRecursive(TreeNode_t* node, const TreeHead_t* head){
    TreeErr_t err = TreeNodeVerify(node, head);
    if(err) return err;
    
    if(node->left){
        err = TreeVerifyRecursive(node->left, head);
        if(err) return err;
    }

    if(node->right){
        err = TreeVerifyRecursive(node->right, head);
        if(err) return err;
    }

    return NO_MISTAKE;
}

TreeErr_t TreeNodeVerify(const TreeNode_t *node, const TreeHead_t* head){
    if(!node || !head){
        fprintf(stderr, "NULL node ptr\n");
        return NULL_NODE_PTR;
    }
    if(node->signature != TREE_SIGNATURE){
        fprintf(stderr, "Incorr signature\n");
        return INCORRECT_SIGN;
    }

    // вообще ниже по хорошему нужен дамп дерева
    if((node != head->root && (node->parent->left != node && node->parent->right != node)) 
        || (node == head->root && node->parent != NULL)){
        fprintf(stderr, "Incorr connection between parent(%p) and child(%p) nodes\n", node->parent, node);
        return INCORR_CONNECT_PARENT_CHILD;
    }
    if(node->left && node != node->left->parent){
        fprintf(stderr, "Incorr connection between parent(%p) and LEFT child(%p) nodes\n", node, node->left);
        return INCORR_LEFT_CONNECT;
    }
    if(node->right && node != node->right->parent){
        fprintf(stderr, "Incorr connection between parent(%p) and RIGHT child(%p) nodes\n", node, node->right);
        return INCORR_RIGHT_CONNECT;
    }

    return NO_MISTAKE;
}