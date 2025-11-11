#include "akinator.h"
#include "tree_dump.h"
#include "tree_func.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define MAX_SHORT_ANSWER_SIZE 8
#define MAX_QUESTION_SIZE 100
#define MAX_PERSON_NAME_SIZE 16
#define LEFT 1
#define RIGHT 0
// нужно для того вдруг мы захотим поменять строки с помощью которых отвечаем да или нет
#define YES "yes"
#define NO "no"

static TreeErr_t PutAkinatorTreeToFile(FILE *file, TreeNode_t *node, const TreeHead_t* head);

static TreeErr_t TreeAddObjAndQuestion(TreeNode_t* node, TreeHead_t* head);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Small functions that used in many other functions

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

//-------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// TreeAkinate - main akinator function

enum TYPEANSWERS{
    NO_ANS  = 0,
    YES_ANS = 1,
    INCORRECT = 2,
};

static bool StartGame(TreeHead_t* head);

static TYPEANSWERS GetCheckAnswer(char* answer, TreeNode_t* node);

static bool CheckAndProcessYes(TreeNode_t** node, TreeHead_t* head);

static bool CheckAndProcessNo(TreeNode_t** node, TreeHead_t* head);

static bool PlayAgain();

TreeErr_t TreeAkinate(TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(head->root, head);
    if(err) return err;

    if(!StartGame(head)){
        return TreeNodeVerify(head->root, head);
    }

    bool get_corr_answer = false;
    TYPEANSWERS typeans = INCORRECT;

    char answer[MAX_SHORT_ANSWER_SIZE] = {};
    TreeNode_t* node = head->root;

    while(!get_corr_answer){
        typeans = GetCheckAnswer(answer, node);
        if(typeans == YES_ANS){
            get_corr_answer = CheckAndProcessYes(&node, head);
        }
        else if(typeans == NO_ANS){
            get_corr_answer = CheckAndProcessNo(&node, head);
        }
    }

    return TreeNodeVerify(node, head);
}

static bool StartGame(TreeHead_t* head){
    if(!strcmp(head->root->data, "NOTHING")){
        TreeAddFirstQuestion(head);
        if(PlayAgain()){
            return true;
        }
        return false;
    }
    return true;
}

static TYPEANSWERS GetCheckAnswer(char* answer, TreeNode_t* node){
    printf("%s(yes or no?)\n", node->data);
    scanf("%7[^\n]", answer);
    clear_input_buffer();
    answer =  str_tolower(answer);
    if(!strncmp(answer, YES, sizeof(YES))){
        return YES_ANS;
    }
    if(!strncmp(answer, NO, sizeof(NO))){
        return NO_ANS;
    }
    fprintf(stderr, "INCORRECT ANSWER - try one more time\n");
    return INCORRECT;
}

static bool CheckAndProcessYes(TreeNode_t** node, TreeHead_t* head){
    assert(head && node && *node);
    if((*node)->left){
        *node = (*node)->left;
    }
    else{
        printf("AI WON!!! The world will be invaded by deepseek!!!\n");
        if(!PlayAgain()){
            return true;
        }
        *node = head->root;
    }
    return false;
}

static bool CheckAndProcessNo(TreeNode_t** node, TreeHead_t* head){
    assert(head && node && *node);
    if((*node)->right){
        *node = (*node)->right;
    }
    else{
        TreeAddObjAndQuestion(*node, head);
        if(!PlayAgain()){
            return true;
        }
        (*node) = head->root;
    }
    return false;
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

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
// Function for adding new nodes and questions for them if not first question
// For adding first node in empty tree - see below

static TreeNode_t* AddPerson(TreeNode_t* node, const char* str);

static void TreeQuesion(TreeNode_t* node, TreeHead_t* head);

static TreeErr_t TreeAddObjAndQuestion(TreeNode_t* node, TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(node, head);
    if(err) return err;

    node->right = NodeCtor(node->data, node, NULL, NULL);
    node->left = AddPerson(node, "Who it was?");

    TreeQuesion(node, head);
    head->capacity += 2;

    return TreeNodeVerify(node, head);
}

static TreeNode_t* AddPerson(TreeNode_t* node, const char* str){
    assert(str);
    printf("%s\n", str);
    char person[MAX_PERSON_NAME_SIZE] = {};
    scanf("%15[^\n]", person);
    clear_input_buffer();
    return NodeCtor(person, node, NULL, NULL);
}

//-------------------------------------------------------------------------------
// For asking dividing questions

static void GetDividingQuestion(char* question);

static bool IsForbiddenSymbolsInStr(char* question);

static void TreeQuesion(TreeNode_t* node, TreeHead_t* head){
    bool get_answer_without_no = false;
    char question[MAX_QUESTION_SIZE] = {};

    while(!get_answer_without_no){
        GetDividingQuestion(question);

        if(IsForbiddenSymbolsInStr(question)){
            fprintf(stderr, "Your answer contains no or not - please give answer without no or note\n");
        }
        else{
            free(node->data);
            node->data = strdup(question);
            get_answer_without_no = true;
        }
    }
}

static void GetDividingQuestion(char* question){
    assert(question);
    printf("What is dividing question?\n");
    printf("Do not use words no or not in the question\n");
    scanf("%99[^\n]", question);
    clear_input_buffer();
}

static bool IsForbiddenSymbolsInStr(char* question){
    assert(question);
    char *copy_string = strdup(question);
    copy_string = str_tolower(copy_string);

    bool is_forbidden_symbols =  strstr(copy_string, " no") || strstr(copy_string, " not");
    free(copy_string);

    return is_forbidden_symbols;
}

//------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
// For asking first question

TreeErr_t TreeAddFirstQuestion(TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(head->root, head);
    if(err) return err;

    printf("Akinator is empty - need to add first question\n");
    TreeQuesion(head->root, head);

    head->root->left = AddPerson(head->root, "For who this feature is true?");
    head->root->right = AddPerson(head->root, "For who this feature is false?");
    head->capacity += 2;

    return TreeNodeVerify(head->root, head);
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// Telling definition of object

static void OutputCommands(stack_t_t* path, TreeNode_t* adr_found, TreeNode_t* st_node, TreeHead_t* head);

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

    stack_t_t* path = TreeFindPathToNode(adr_found, head->root);
    if(!path){
        return CANT_FIND_PATH;
    }

    SayAndPrintSaid("\n%s -", adr_found->data);

    OutputCommands(path, adr_found, head->root, head);

    stack_free(path);

    err = TreeNodeVerify(head->root, head);
    return err;
}

static void OutputCommands(stack_t_t* path, TreeNode_t* adr_found, TreeNode_t* st_node, TreeHead_t* head){
    TreeNode_t* node = st_node;
    int elem = 0;
    while(node != adr_found){
        stack_pop(path, &elem);
        if(elem == LEFT){
            SayAndPrintSaid(" %s", node->data);
            node = node ->left;
        }
        if(elem == RIGHT){
            SayAndPrintSaid(" not %s", node->data);
            node = node ->right;
        }
    }
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// Пока что тут копипаст - подумаю как пофиксить

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

    stack_t_t* path = TreeFindPathToNode(common_node, head->root);
    if(!path){
        return CANT_FIND_PATH;
    }

    SayAndPrintSaid("\n\nComparing %s and %s\n", name1, name2);

    SayAndPrintSaid("\nCommon: ");

    OutputCommands(path, common_node, head->root, head);

    stack_free(path);

    stack_t_t* path1 = TreeFindPathToNode(adr1_found, common_node);
    if(!path1){
        return CANT_FIND_PATH;
    }

    SayAndPrintSaid("\nOpposite for %s: ", adr1_found->data);

    OutputCommands(path1, adr1_found, common_node, head);

    stack_free(path1);

    stack_t_t* path2 = TreeFindPathToNode(adr2_found, common_node);
    if(!path2){
        return CANT_FIND_PATH;
    }

    SayAndPrintSaid("\nOpposite for %s: ", adr2_found->data);

    OutputCommands(path2, adr2_found, common_node, head);

    stack_free(path2);

    err = TreeNodeVerify(head->root, head);
    return err;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// For save akinator to disk

static TreeErr_t PutAkinatorTreeToFile(FILE *file, TreeNode_t *node, const TreeHead_t* head);

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
