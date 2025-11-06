#include "tree.h"
#include "tree_dump.h"
#include <stdio.h>

int main(){
    TreeHead_t head = {};
    TreeAddNode(10, &head);
    TreeAddNode(5, &head);
    TreeAddNode(20, &head);
    TreeAddNode(3, &head);
    TreeAddNode(7, &head);
    TreeAddNode(15, &head);
    tree_dump_func(head.root, "Printing tree before del", __FILE__, __func__,  __LINE__);

    TreeDelNode(head.root->right, &head);
    tree_dump_func(head.root, "Printing tree after del node", __FILE__, __func__,  __LINE__);

    TreeDel(&head);
    tree_dump_func(head.root, "Printing tree after del full", __FILE__, __func__,  __LINE__);

    return 0;
}