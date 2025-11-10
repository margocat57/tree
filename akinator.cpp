#include "akinator.h"
#include "tree_dump.h"
#include "tree_func.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

const int MAX_SHORT_ANSWER_SIZE = 8;
const int MAX_QUESTION_SIZE = 100;
const int MAX_PERSON_NAME_SIZE = 16;
// нужно для того вдруг мы захотим поменять строки с помощью которых отвечаем да или нет
#define YES "yes"
#define NO "no"

static TreeErr_t PutAkinatorTreeToFile(FILE *file, TreeNode_t *node, const TreeHead_t* head);

static bool StartGameWithEmptyTree(TreeNode_t* node, TreeHead_t* head);

static bool PlayAgain();

static bool GetUserAnswer(TreeNode_t* node);

static TreeErr_t AIWon(TreeNode_t* node, TreeHead_t* head);

static TreeErr_t HandleAddQuestion(TreeNode_t* node, TreeHead_t* head);

static bool GetFeatureFromUser(char* feature);

static bool ContainsForbiddenWords(char* feature);

static bool CheckAndSetNode(char* feature, TreeNode_t* node, TreeHead_t* head);

static void PrintCommon(TreeHead_t* head, TreeNode_t *common_node);

static void PrintOpposite(const char* name1, const char* name2, TreeNode_t *adr1_found, TreeNode_t *adr2_found, TreeNode_t *common_node);

static void PrintOppositeCommonDefinition(TreeNode_t *node, TreeNode_t *check_node);

static void TreeQuesion(TreeNode_t* node, TreeHead_t* head);

static TreeNode_t* AddPerson(TreeNode_t* node, const char* str);

static char* str_tolower(char* str){
    assert(str);
    for (int ch = 0; ch < strlen(str); ch++){
        str[ch] = tolower(str[ch]);
    }
    return str;
}

static void clear_input_buffer(void){
    int c = 0; 
    while ((c = getchar()) != '\n') {;}
}


static bool PlayAgain(){
    char again[MAX_SHORT_ANSWER_SIZE] = {};
    printf("Do you want to play again?(input " YES " if you want)\n");
    scanf("%7[^\n]", again);
    clear_input_buffer();
    str_tolower(again);
    if(!strncmp(again, YES, sizeof(YES))){
        return true;
    }
    return false;
}

static bool StartGameWithEmptyTree(TreeNode_t* node, TreeHead_t* head){
    if(!strcmp(node->data, "NOTHING")){
        printf("Akinator is empty - need to add first question\n");
        TreeAddFirstQuestion(head);
        if(PlayAgain()){
            TreeAkinate(head->root, head);
        }
        return true;
    }
    return false;
}

static bool GetUserAnswer(TreeNode_t* node){
    char answer[MAX_SHORT_ANSWER_SIZE] = {};

    while(true){
        printf("%s? (" YES " or " NO "?)\n", node->data);
        scanf("%7[^\n]", answer);
        clear_input_buffer();
        str_tolower(answer);

        if(!strncmp(answer, YES, sizeof(YES))){
            return true;
        } 
        else if(!strncmp(answer, NO, sizeof(NO))){
            return false;
        } 
        else {
            fprintf(stderr, "INCORRECT ANSWER - try one more time\n");
        }
    }
}

static TreeErr_t AIWon(TreeNode_t* node, TreeHead_t* head){
    printf("AI WON!!! The world will be invaded by deepseek!!!\n");
    if(PlayAgain()){
        return TreeAkinate(head->root, head);
    }
    return TreeNodeVerify(node, head);
}

static TreeErr_t HandleAddQuestion(TreeNode_t* node, TreeHead_t* head){
    TreeAddQuestion(node, head);
    if(PlayAgain()){
        return TreeAkinate(head->root, head);
    }
    return TreeNodeVerify(node, head);
}

TreeErr_t TreeAkinate(TreeNode_t* node, TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(node, head);
    if(err) return err;

    char answer[MAX_SHORT_ANSWER_SIZE] = {};

    if(StartGameWithEmptyTree(node, head)){
        err = TreeNodeVerify(node, head);
        return err;
    }

    bool user_answer = GetUserAnswer(node);

    if(user_answer){
        if(node->left){
            return TreeAkinate(node->left, head);
        } 
        else{
            return AIWon(node, head);
        }
    }
    else{
        if(node->right){
            return TreeAkinate(node->right, head);
        } 
        else{
            return HandleAddQuestion(node, head);
        }
    }

    err = TreeNodeVerify(node, head);
    return err;
}

static bool GetFeatureFromUser(char* feature){
    printf("What is dividing feature?\n");
    printf("Do not use words no or not in the feature\n");
    
    if(scanf("%99[^\n]", feature) != 1){
        clear_input_buffer();
        return false;
    }
    clear_input_buffer();
    return true;
}

static bool ContainsForbiddenWords(char* feature){
    assert(feature);

    char *copy_string = strdup(feature);
    copy_string = str_tolower(copy_string); 

    bool is_forbiden_words = strstr(copy_string, " no") || strstr(copy_string, " not");
    free(copy_string);

    return is_forbiden_words;
}

static bool CheckAndSetNode(char* feature, TreeNode_t* node, TreeHead_t* head){
    if(ContainsForbiddenWords(feature)){
        fprintf(stderr, "Your answer contains no or not - please give answer without no or note\n");
        return false;
    }

    free(node->data);
    node->data = strdup(feature);
    head->capacity += 2;
    return true;
}

static void TreeQuesion(TreeNode_t* node, TreeHead_t* head){
    char feature[MAX_QUESTION_SIZE] = {};

    while(true){
        if(!GetFeatureFromUser(feature)){
            printf("Can't get information about person - try again\n");
        }

        if(CheckAndSetNode(feature, node, head)){
            break;
        }
    }
}

TreeErr_t TreeAddQuestion(TreeNode_t* node, TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(node, head);
    if(err) return err;

    node->right = NodeCtor(node->data, node, NULL, NULL);
    node->left = AddPerson(node, "Who it was?");

    TreeQuesion(node, head);

    err = TreeNodeVerify(node, head);
    return err;
}

TreeErr_t TreeAddFirstQuestion(TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(head->root, head);
    if(err) return err;

    TreeQuesion(head->root, head);

    head->root->left = AddPerson(head->root, "For who this feature is true?");
    head->root->right = AddPerson(head->root, "For who this feature is false?");
    head->capacity += 2;

    err = TreeNodeVerify(head->root, head);
    return err;
}

static TreeNode_t* AddPerson(TreeNode_t* node, const char* str){
    assert(str);
    printf("%s\n", str);
    char person[MAX_PERSON_NAME_SIZE] = {};
    scanf("%15[^\n]", person);
    clear_input_buffer();
    return NodeCtor(person, node, NULL, NULL);
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
    PrintOppositeCommonDefinition(adr_found, NULL);

    err = TreeNodeVerify(head->root, head);
    return err;
}

static void PrintCommon(TreeHead_t* head, TreeNode_t *common_node){
    printf("\nCommon: ");
    if(common_node == head->root){
        printf(" nothing");
    }
    else{
        PrintOppositeCommonDefinition(common_node, NULL);
    }
}

static void PrintOpposite(const char* name1, const char* name2, TreeNode_t *adr1_found, TreeNode_t *adr2_found, TreeNode_t *common_node){
    printf("\nOpposite: ");
    printf("(for %s)", name1);
    PrintOppositeCommonDefinition(adr1_found, common_node);

    printf("\n(for %s)", name2);
    PrintOppositeCommonDefinition(adr2_found, common_node);
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

    TreeNode_t* common_node = TreeFindCommonNode(head, adr1_found, adr2_found);

    printf("\n\nComparing %s and %s", name1, name2);
    PrintCommon(head, common_node);
    PrintOpposite(name1, name2, adr1_found, adr2_found, common_node);

    err = TreeNodeVerify(head->root, head);
    return err;
}

// да, тернарный оператор тот еще костыль но я честно не понимаю как здесь 
// не плодить две функции без компаратора
static void PrintOppositeCommonDefinition(TreeNode_t *node, TreeNode_t *check_node){
    assert(node);
    while((check_node == NULL ? node->parent != NULL : node != check_node)){
        if(node == node->parent->left){
            printf(" %s,", node->parent->data);
        }
        if(node == node->parent->right){
            printf(" not %s,", node->parent->data);
        }
        node = node->parent;
    }
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

    fprintf(file, " ) ");
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
