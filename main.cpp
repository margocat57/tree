#include "akinator.h"
#include "tree_func.h"
#include "tree_dump.h"
#include "input_output.h"
#include <stdio.h>

int main(){
    TreeHead_t* head = MakeAkinatorTree("akinator.txt");
    /*
    TreeHead_t* head = TreeCtor("akinator.txt");
    TreeNode_t * l1 = NodeCtor("Animal", NULL, NULL, NULL, false);
    TreeNode_t * l2_l = NodeCtor("Poltorashka",l1, NULL, NULL, false);
    TreeNode_t * l2_r = NodeCtor("Teaches matan",l1, NULL, NULL, false);
    TreeNode_t * l3_l = NodeCtor("Lukashov",l2_r, NULL, NULL, false);
    TreeNode_t * l3_r = NodeCtor("PashaT",l2_r, NULL, NULL, false);
    head->root=l1;
    l1->left=l2_l;
    l1->right=l2_r;
    l2_r->left = l3_l;
    l2_r->right = l3_r;
    */

    tree_dump_func(head->root, head, "Printing tree before adding", __FILE__, __func__,  __LINE__);
    TreeAkinate(head);
    /*
    TreeMakeDefinition(head, "Dagaev");
    TreeFindCommonOpposite(head, "Dagaev", "Lukashov");
    TreeFindCommonOpposite(head, "Dagaev", "PashaT");
    TreeFindCommonOpposite(head, "Dagaev", "Poltorashka");

    tree_dump_func(head->root, head, "Printing tree after adding", __FILE__, __func__,  __LINE__);

    PutAkinatorFile("akinator.txt", head->root, head);
    */

    TreeDel(head);

    return 0;
}