#include "tree.h"
#include "tree_dump.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

TreeNode_t* NodeCtor(TreeElem_t data, TreeNode_t* parent, TreeNode_t* left, TreeNode_t* right){
    TreeNode_t* node = (TreeNode_t*)calloc(1, sizeof(TreeNode_t));
    if(!node){
        fprintf(stderr, "Can't alloc data for node");
        return NULL;
    }

    node->data = data;
    node->left = left;
    node->right = right;
    node->parent = parent;
    node->signature = TREE_SIGNATURE;

    return node;
}


void NodeDtor(TreeNode_t* node){
    if(node){
        memset(node, 0, sizeof(TreeNode_t));
        free(node);
    }
}

// макрос генерирующий функцию - то есть разные виды компараторов
TreeErr_t TreeAddNode(TreeElem_t elem, TreeHead_t* head){
    if(!head) return NULL_HEAD_PTR;

    TreeNode_t* next_node = head->root;
    if(!next_node){
        head->root = NodeCtor(elem, NULL, NULL, NULL);
        head->capacity = 1;
        return NO_MISTAKE;
    }

    TreeErr_t err = NO_MISTAKE;
    err = TreeVerify(next_node);
    if(err) return err;

    while(true){
        if(elem <= next_node->data){
            if(!next_node->left){
                next_node->left = NodeCtor(elem, next_node, NULL, NULL);
                head->capacity++;
                next_node = next_node->left;
                break;
            }
            next_node = next_node->left;
        }
        if(elem > next_node->data){
            if(!next_node->right){
                next_node->right = NodeCtor(elem, next_node, NULL, NULL);
                head->capacity++;
                next_node = next_node->right;
                break;
            }
            next_node = next_node->right;
        }
    }
    err = TreeVerify(next_node);
    return err;
}

TreeErr_t TreeDelNode(TreeNode_t* node, TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    err = TreeVerify(node);
    if(err) return err;

    TreeNode_t* parent = node->parent;

    if(node->left){
        TreeDelNode(node->left, head);
    }
    if(node->right){
        TreeDelNode(node->right, head);
    }

    if (parent != NULL) {
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
    err = TreeVerify(head->root);
    if(err) return err;

    TreeDelNode(head->root, head);
    head->root = NULL;

    return err;
}

TreeErr_t PrintNode(const TreeNode_t* node, FILE* dot_file, int* rank){
    TreeErr_t err = NO_MISTAKE;
    err = TreeVerify(node);
    if(err) return err;

    if(node->left){
        fprintf(dot_file, " node_%p -> node_%p;\n", node, node->left); 
        (*rank)++;
        PrintNode(node->left, dot_file, rank);
    }
    fprintf(dot_file, " node_%p[shape=\"Mrecord\", style=\"filled\", fillcolor=\"#98FB98\", rank=%d, color = \"#000000\", penwidth=1.0, label=\"{{parent = %p} | {adress = %p} | {elem = %d} | {left = %p | right = %p}} \"];\n", node, *rank, node->parent ,node, node->data, node->left, node->right);
    if(node->right){
        fprintf(dot_file, " node_%p -> node_%p;\n", node, node->right);
        (*rank)++;
        PrintNode(node->right, dot_file, rank);
    }
    (*rank)--;

    err = TreeVerify(node);
    return err;
}

TreeErr_t TreeVerify(const TreeNode_t *node){
    if(!node){
        fprintf(stderr, "NULL node ptr\n");
        return NULL_NODE_PTR;
    }
    if(node->signature != TREE_SIGNATURE){
        fprintf(stderr, "Incorr signature\n");
        return INCORRECT_SIGN;
    }

    return NO_MISTAKE;
}