#include "tree.h"
#include "tree_dump.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

static TreeErr_t PutAkinatorTreeToFile(FILE *file, TreeNode_t *node, const TreeHead_t* head);

static bool PlayAgain();

static void TreeQuesion(TreeNode_t* node, TreeHead_t* head);

static size_t TreeFindNodeDepth(TreeNode_t* node);

static TreeNode_t* TreeFindCommonNode(TreeHead_t* head, TreeNode_t* node1, TreeNode_t* node2);

TreeHead_t* TreeCtor(){
    TreeHead_t* head = (TreeHead_t*)calloc(1, sizeof(TreeHead_t));
    head->root = NodeCtor((char*)"NOTHING", NULL, NULL, NULL);
    head->capacity = 1;

    return head;
}

char* str_tolower(char* str){
    assert(str);
    for (int ch = 0; ch < strlen(str); ch++){
        str[ch] = tolower(str[ch]);
    }
    return str;
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

// повторяющиеся функции NodeDtor и TreeDelNode
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

static bool PlayAgain(){
    char again[8] = {};
    printf("Do you want to play again?(input yes if you want)\n");
    scanf("%7[^\n]", again);
    getchar();
    str_tolower(again);
    if(!strncmp(again, "yes", 3)){
        return true;
    }
    return false;
}


TreeErr_t TreeAkinate(TreeNode_t* node, TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(node, head);
    if(err) return err;

    char answer[8] = {};

    if(!strcmp(node->data, "NOTHING")){
        printf("Akinator is empty - need to add first question\n");
        TreeAddFirstQuestion(head);
        if(PlayAgain()){
            return TreeAkinate(head->root, head);
        }
        err = TreeNodeVerify(node, head);
        return err;
    }

    bool get_corr_answer = false;

    while(!get_corr_answer){
        printf("%s?(yes or no?)\n", node->data);
        scanf("%7[^\n]", answer);
        getchar();
        str_tolower(answer);

        if(!strncmp(answer, "yes", 3)){
            if(node->left){
                TreeAkinate(node->left, head);
            }
            else{
                printf("AI WON!!! The world will be invaded by deepseek!!!\n");
                if(PlayAgain()){
                    return TreeAkinate(head->root, head);
                }
            }
            get_corr_answer = true;
        }
        else if(!strncmp(answer, "no", 2)){
            if(node->right){
                TreeAkinate(node->right, head);
            }
            else{
                TreeAddQuestion(node, head);
                if(PlayAgain()){
                    return TreeAkinate(head->root, head);
                }
            }
            get_corr_answer = true;
        }
        else{
            fprintf(stderr, "INCORRECT ANSWER - try one more time\n");
        }
    }

    err = TreeNodeVerify(node, head);
    return err;
}

static void TreeQuesion(TreeNode_t* node, TreeHead_t* head){
    bool get_answer_without_no = false;
    char feature[100] = {};

    while(!get_answer_without_no){
        printf("What is dividing feature?\n");
        printf("Do not use words no or not in the feature\n");
        scanf("%99[^\n]", feature);
        getchar();

        char *copy_string = strdup(feature);
        copy_string = str_tolower(copy_string); 
        if(!strstr(copy_string, " no") && !strstr(copy_string, " not")){
            free(node->data);
            node->data = strdup(feature);
            head->capacity += 2;
            get_answer_without_no = true;
        }
        else{
            fprintf(stderr, "Your answer contains no or not - please give answer without no or note\n");
        }
        free(copy_string);
    }
}


TreeErr_t TreeAddQuestion(TreeNode_t* node, TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(node, head);
    if(err) return err;

    node->right = NodeCtor(node->data, node, NULL, NULL);

    printf("Who it was?\n");
    char person[16] = {};
    scanf("%15[^\n]", person);
    getchar();
    node->left = NodeCtor(person, node, NULL, NULL);

    TreeQuesion(node, head);

    err = TreeNodeVerify(node, head);
    return err;
}

TreeErr_t TreeAddFirstQuestion(TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(head->root, head);
    if(err) return err;

    TreeQuesion(head->root, head);

    printf("For who this feature is true?\n");
    char person_left[16] = {};
    scanf("%15[^\n]", person_left);
    getchar();
    head->root->left = NodeCtor(person_left, head->root, NULL, NULL);

    printf("For who this feature is false?\n");
    char person_right[16] = {};
    scanf("%15[^\n]", person_right);
    getchar();
    head->root->right = NodeCtor(person_right, head->root, NULL, NULL);

    head->capacity+=2;

    err = TreeNodeVerify(head->root, head);
    return err;
}

TreeErr_t TreeFindNode(TreeNode_t* node, TreeHead_t* head, const char* name, TreeNode_t** node_ptr){
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

TreeErr_t TreeMakeDefinition(TreeHead_t* head, const char* name){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(head->root, head);
    if(err) return err;

    TreeNode_t* adr_found = NULL;
    TreeFindNode(head->root, head, name, &adr_found);

    if(!adr_found){
        printf("Can't find node with such object");
        return CANT_FIND_NODE;
    }

    printf("\n%s -", adr_found->data);

    while(adr_found->parent){
        if(adr_found == adr_found->parent->left){
            printf(" %s,", adr_found->parent->data);
        }
        if(adr_found == adr_found->parent->right){
            printf(" not %s,", adr_found->parent->data);
        }
        adr_found = adr_found->parent;
    }


    err = TreeNodeVerify(head->root, head);
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

static TreeNode_t* TreeFindCommonNode(TreeHead_t* head, TreeNode_t* node1, TreeNode_t* node2){
    size_t depth1 = TreeFindNodeDepth(node1);
    size_t depth2 = TreeFindNodeDepth(node2);

    while(depth1 > depth2){
        node1 = node1->parent;
        depth1--;
    }
    while(depth2 > depth1){
        node2 = node2->parent;
        depth2--;
    }
    while(node1 != node2 && node1 && node2){
        node1 = node1->parent;
        node2 = node2->parent;
    }
    if(!node1 || !node2){
        return head->root;
    }
    return node1;
}

TreeErr_t TreeFindCommonOpposite(TreeHead_t* head, const char* name1, const char* name2){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(head->root, head);
    if(err) return err;

    TreeNode_t* adr1_found = NULL;
    TreeFindNode(head->root, head, name1, &adr1_found);

    TreeNode_t* adr2_found = NULL;
    TreeFindNode(head->root, head, name2, &adr2_found);

    if(!adr1_found || !adr2_found){
        printf("Can't find node by name\n");
        return CANT_FIND_NODE;
    }
    printf("\n\nComparing %s and %s", name1, name2);

    TreeNode_t* common_node = TreeFindCommonNode(head, adr1_found, adr2_found);
    TreeNode_t* node = common_node;

    printf("\nCommon: ");
    if(common_node == head->root){
        printf(" nothing");
    }
    else{
        while(node->parent){
            if(node == node->parent->left){
                printf(" %s,", node->parent->data);
            }
            if(node == node->parent->right){
                printf(" not %s,", node->parent->data);
            }
            node = node->parent;
        }
    }

    printf("\nOpposite: ");
    printf("(for %s)", name1);
    node = adr1_found;
    while(node != common_node){
        if(node == node->parent->left){
            printf(" %s,", node->parent->data);
        }
        if(node == node->parent->right){
            printf(" not %s,", node->parent->data);
        }
        node = node->parent;
    }

    printf("\n(for %s)", name2);
    node = adr2_found;
    while(node != common_node){
        if(node == node->parent->left){
            printf(" %s,", node->parent->data);
        }
        if(node == node->parent->right){
            printf(" not %s,", node->parent->data);
        }
        node = node->parent;
    }

    err = TreeNodeVerify(head->root, head);
    return err;
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
    err = TreeNodeVerify(head->root, head);
    if(err) return err;

    TreeDelNodeRecur(head->root, head);
    memset(head, 0, sizeof(TreeHead_t));
    free(head);

    return err;
}

TreeErr_t PrintNode(const TreeNode_t* node, const TreeHead_t* head, FILE* dot_file, int* rank){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(head->root, head);
    if(err) return err;

    if(node->left){
        fprintf(dot_file, " node_%p -> node_%p[color = \"#964B00\"];\n", node, node->left); 
        (*rank)++;
        PrintNode(node->left, head, dot_file, rank);
    }

    if(node->left && node->right){
        fprintf(dot_file, " node_%p[shape=\"Mrecord\", style=\"filled\", fillcolor=\"#98FB98\", rank=%d, color = \"#964B00\", penwidth=1.0, label=\"{{%s?} | {YES | NO}} \"];\n", node, *rank, node->data);
    }
    else{
        fprintf(dot_file, " node_%p[shape=\"Mrecord\", style=\"filled\", fillcolor=\"#98FB98\", rank=%d, color = \"#964B00\", penwidth=1.0, label=\"{{%s} | {0 | 0}} \"];\n", node, *rank, node->data);
    }

    if(node->right){
        fprintf(dot_file, " node_%p -> node_%p[color = \"#964B00\"];\n", node, node->right);
        (*rank)++;
        PrintNode(node->right, head, dot_file, rank);
    }
    (*rank)--;

    err = TreeNodeVerify(node, head);
    return err;
}

TreeErr_t TreeVerify(const TreeHead_t* head){
    return TreeNodeVerify(head->root, head);
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

    if(node->left && node->right){
        return TreeNodeVerify(node->left, head) && TreeNodeVerify(node->right, head);
    }

    return NO_MISTAKE;
}

static TreeErr_t PutAkinatorTreeToFile(FILE *file, TreeNode_t *node, const TreeHead_t* head){
    if(!file){
        fprintf(stderr, "INCORRECT OUTPUT FILE\n");
        return INCORR_OUTPUT_FILE;
    }
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(head->root, head);
    if(err) return err;

    fprintf(file, "( \"%s\"", node->data);

    if(node->left){
        PutAkinatorTreeToFile(file, node->left, head);
    } 
    else{
        fprintf(file, " nill");
    }

    if(node->right){
        PutAkinatorTreeToFile(file, node->right, head);
    } 
    else{
        fprintf(file, " nill");
    }

    fprintf(file, " )");
    err = TreeNodeVerify(node, head);
    return err;
}

TreeErr_t PutAkinatorFile(const char* file_name, TreeNode_t *node, const TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(head->root, head);
    if(err) return err;

    FILE* fp = fopen(file_name, "w");
    if(!fp){
        fprintf(stderr, "Can't open output file\n");
        return CANT_OPEN_OUT_FILE;
    }

    PutAkinatorTreeToFile(fp, node, head);
    fclose(fp);

    err = TreeNodeVerify(node, head);
    return err;
}
