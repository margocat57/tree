#include "tree.h"
#include "tree_dump.h"
#include <stdio.h>

//новая таска
//Сделать проверку на регистр
// обрабатывать пустое дерево
// сделать вывод дерева в тхт
// сделать обработку ошибок

// в дампе указатель на подсвеченный элемент - рисуем другим цветом
// проверка связи родитель -- ребенок - в верификаторе есть
// и в дампе по хорошему тоже добавить проверку на висячий узел
int main(){
    TreeHead_t* head = TreeCtor();
    TreeNode_t * l1 = NodeCtor("Animal", NULL, NULL, NULL);
    TreeNode_t * l2_l = NodeCtor("Poltorashka",l1, NULL, NULL);
    TreeNode_t * l2_r = NodeCtor("Teaches matan",l1, NULL, NULL);
    TreeNode_t * l3_l = NodeCtor("Lukashov",l2_r, NULL, NULL);
    TreeNode_t * l3_r = NodeCtor("PashaT",l2_r, NULL, NULL);
    head->root=l1;
    l1->left=l2_l;
    l1->right=l2_r;
    l2_r->left = l3_l;
    l2_r->right = l3_r;
    TreeAkinate(head->root, head);
    TreeMakeDefinition(head, "Dagaev");
    TreeFindCommonOpposite(head, "Dagaev", "Lukashov");
    TreeFindCommonOpposite(head, "Dagaev", "PashaT");
    TreeFindCommonOpposite(head, "Dagaev", "Poltorashka");

    tree_dump_func(head->root, head, "Printing tree before del", __FILE__, __func__,  __LINE__);

    // tree_dump_func(head.root, &head, "After damaging with elem 57", __FILE__, __func__,  __LINE__);
    // TreeDelNode(head.root->right, &head);
    // tree_dump_func(head.root, &head, "Printing tree after del node", __FILE__, __func__,  __LINE__);
    PutAkinatorFile("tree.txt", head->root, head);

    TreeDel(head);
    // tree_dump_func(head.root, &head, "Printing tree after del full", __FILE__, __func__,  __LINE__);

    return 0;
}