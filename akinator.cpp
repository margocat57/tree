#include "akinator.h"
#include "tree_dump.h"
#include "tree_func.h"
#include "input_output.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

const int MAX_SHORT_ANSWER_SIZE = 8;
const int MAX_QUESTION_SIZE = 100;
const int MAX_PERSON_NAME_SIZE = 16;
const char* YES = "yes";
const char* NO = "no";

static TreeErr_t TreeAddObjAndQuestion(TreeNode_t* node, TreeHead_t* head);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Small functions that used in many other functions

static char* str_tolower(char* str){
    assert(str);
    for (size_t ch = 0; ch < strlen(str); ch++){
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
// Akinator menu - funcyion

TreeErr_t AkinatorMenuAndMainFunc(TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    err = TreeVerify(head);
    if(err) return err;

    bool is_quit = false;
    char buffer[MAX_PERSON_NAME_SIZE] = {};
    char buffer2[MAX_PERSON_NAME_SIZE] = {};
    char buffer_file[MAX_QUESTION_SIZE] = {};
    char choise = 'x';

    while(!is_quit){
        SayAndPrintSaid(GREY_BLUE, "\nWhat do you want?\n");
        printf(GREY_BLUE "[G]uess? (put G)\n" COLOR_RESET);
        printf(GREY_BLUE "[M]ake definition? (put M)\n" COLOR_RESET);
        printf(GREY_BLUE "[C]ompare objects? (put C)\n" COLOR_RESET);
        printf(GREY_BLUE "[D]ump tree? (put S)\n" COLOR_RESET);
        printf(GREY_BLUE "[S]ave and quit? (put S)\n" COLOR_RESET);
        printf(GREY_BLUE "Quit without saving? (put other letter)\n" COLOR_RESET);

        printf(LIGHT_PURPLE); scanf(" %c", &choise); printf(COLOR_RESET);
        clear_input_buffer();
        switch(choise){
            case 'G':   TreeAkinate(head); break;
            case 'M':   SayAndPrintSaid(GREY_BLUE, "Definition of who do you want to make?\n");
                        printf(LIGHT_PURPLE); scanf("%s", buffer); printf(COLOR_RESET);
                        TreeMakeDefinition(head, buffer);
                        break;
            case 'C':   SayAndPrintSaid(GREY_BLUE, "Compare who do you want?\n");
                        printf(GREY_BLUE); printf("At first: "); printf(COLOR_RESET);
                        printf(LIGHT_PURPLE); scanf("%s", buffer); printf(COLOR_RESET);
                        printf(GREY_BLUE); printf("and "); printf(COLOR_RESET);
                        printf(LIGHT_PURPLE); scanf(" %s", buffer2); printf(COLOR_RESET);
                        TreeFindCommonOpposite(head, buffer, buffer2); 
                        break;   
            case 'D':   tree_dump_func(head->root, head, "Printing tree", __FILE__, __func__,  __LINE__);
                        break;
            case 'S':   SayAndPrintSaid(GREY_BLUE, "Put name of file where write:\n" COLOR_RESET);
                        printf(LIGHT_PURPLE); scanf("%s", buffer_file); printf(COLOR_RESET);
                        PutAkinatorFile(buffer_file, head->root, head);
                        is_quit = true;
                        break;
            default:    is_quit = true;
        }
        err = TreeVerify(head);
        if(err) return err;
    }

    return TreeVerify(head);
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
    assert(head);

    TreeErr_t err = NO_MISTAKE;
    err = TreeVerify(head);
    if(err) return err;

    if(!StartGame(head)){
        return TreeVerify(head);
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
    assert(head);
    if(!strcmp(head->root->data, NOTHING)){
        TreeAddFirstQuestion(head);
        if(PlayAgain()){
            return true;
        }
        return false;
    }
    return true;
}

static TYPEANSWERS GetCheckAnswer(char* answer, TreeNode_t* node){
    assert(answer);
    assert(node);

    SayAndPrintSaid(GREY_BLUE, "This is %s?(%s or %s?)\n", node->data, YES, NO);
    printf(LIGHT_PURPLE);
    scanf("%7[^\n]", answer);
    printf(COLOR_RESET);
    clear_input_buffer();
    answer =  str_tolower(answer);
    if(!strncmp(answer, YES, strlen(YES))){
        return YES_ANS;
    }
    if(!strncmp(answer, NO, strlen(NO))){
        return NO_ANS;
    }
    fprintf(stderr, "INCORRECT ANSWER - try one more time\n");
    return INCORRECT;
}

static bool CheckAndProcessYes(TreeNode_t** node, TreeHead_t* head){
    assert(node);
    assert(*node);
    assert(head);

    assert(head && node && *node);
    if((*node)->left){
        *node = (*node)->left;
    }
    else{
        SayAndPrintSaid(GREY_BLUE, "AI WON!!! The world will be invaded by deepseek!!!\n");
        if(!PlayAgain()){
            return true;
        }
        *node = head->root;
    }
    return false;
}

static bool CheckAndProcessNo(TreeNode_t** node, TreeHead_t* head){
    assert(node);
    assert(*node);
    assert(head);

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
    SayAndPrintSaid(GREY_BLUE, "Do you want to play again?(input %s if you want)\n", YES);
    printf(LIGHT_PURPLE);
    scanf("%7[^\n]", again);
    printf(COLOR_RESET);
    clear_input_buffer();
    str_tolower(again);
    if(!strncmp(again, YES, strlen(YES))){
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
// Function for adding new nodes and questions for them if not first question
// For adding first node in empty tree - see below

static TreeNode_t* AddPerson(TreeNode_t* node, const char* str);

static void TreeQuestion(TreeNode_t* node);

static bool IsFeatureTrueForNewPerson(char* answer, TreeNode_t* node_left);

static TreeErr_t TreeAddObjAndQuestion(TreeNode_t* node, TreeHead_t* head){
    assert(head);

    TreeErr_t err = NO_MISTAKE;
    err = TreeNodeVerify(node, head);
    if(err) return err;

    node->right = NodeCtor(node->data, node, NULL, NULL, node->need_data_free);
    node->left = AddPerson(node, "Who it was?");

    TreeQuestion(node);
    char answer[MAX_SHORT_ANSWER_SIZE] = {};
    if(!IsFeatureTrueForNewPerson(answer, node->left)){
        TreeNode_t* temp = node->right;
        node->right = node->left;
        node->left = temp;
    }

    head->capacity += 2;

    return TreeNodeVerify(node, head);
}

static TreeNode_t* AddPerson(TreeNode_t* node, const char* str){
    assert(str);
    assert(node);

    SayAndPrintSaid(GREY_BLUE, "%s\n", str);
    char person[MAX_PERSON_NAME_SIZE] = {};
    printf(LIGHT_PURPLE);
    scanf("%15[^\n]", person);
    printf(COLOR_RESET);
    clear_input_buffer();
    return NodeCtor(strdup(person), node, NULL, NULL, true);
}

//-------------------------------------------------------------------------------
// For asking dividing questions

static void GetDividingQuestion(char* question, TreeNode_t* node);

static bool IsForbiddenSymbolsInStr(char* question);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
static void TreeQuestion(TreeNode_t* node){
    assert(node);
    bool get_answer_without_no = false;
    char question[MAX_QUESTION_SIZE] = {};

    while(!get_answer_without_no){
        GetDividingQuestion(question, node);

        if(IsForbiddenSymbolsInStr(question)){
            fprintf(stderr, "Your answer contains no or not - please give answer without no or note\n");
        }
        else{
            SayAndPrintSaid(GREY_BLUE, "I remembered, you can't fool me now\n");
            if(node->need_data_free){
                /** 
                 * Обоснование необходимости снятия const смотрите в функции NodeDtor,
                 * файле tree_func.cpp, где происходит аналогичное
                */
                char* free_ptr = (char*)node->data;
                free(free_ptr);
            }
            node->data = strdup(question);
            node->need_data_free = true;
            get_answer_without_no = true;
        }
    }
}
#pragma GCC diagnostic pop

static void GetDividingQuestion(char* question, TreeNode_t* node){
    assert(question);
    SayAndPrintSaid(GREY_BLUE, "How %s is different fron %s?\n", node->left->data, node->right->data);
    printf(GREY_BLUE "Do not use words no or not in your question\n" COLOR_RESET);
    printf(GREY_BLUE "It is ..." COLOR_RESET);
    printf(LIGHT_PURPLE);
    scanf("%99[^\n]", question);
    printf(COLOR_RESET);
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

static bool IsFeatureTrueForNewPerson(char* answer, TreeNode_t* node_left){
    assert(answer);
    assert(node_left);
    SayAndPrintSaid(GREY_BLUE, "Is you feature true for %s?(input %s if not)\n", node_left->data, NO);
    printf(LIGHT_PURPLE);
    scanf("%7[^\n]", answer);
    printf(COLOR_RESET);
    clear_input_buffer();
    answer =  str_tolower(answer);
    if(!strncmp(answer, NO, strlen(NO))){
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
// For asking first question

TreeErr_t TreeAddFirstQuestion(TreeHead_t* head){
    assert(head);

    TreeErr_t err = NO_MISTAKE;
    err = TreeVerify(head);
    if(err) return err;

    SayAndPrintSaid(GREY_BLUE, "Akinator is empty - need to add first question\n");
    TreeQuestion(head->root);

    head->root->left = AddPerson(head->root, "For who this feature is true?");
    head->root->right = AddPerson(head->root, "For who this feature is false?");
    head->capacity += 2;

    return TreeVerify(head);
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// Telling definition of object

static void OutputCommands(stack_t_t* path, TreeNode_t* adr_found, TreeNode_t* st_node, TreeHead_t* head);

TreeErr_t TreeMakeDefinition(TreeHead_t* head, const char* name){
    assert(head);
    assert(name);

    TreeErr_t err = NO_MISTAKE;
    err = TreeVerify(head);
    if(err) return err;

    TreeNode_t* adr_found = NULL;
    TreeFindNode(head->root, head, name, &adr_found);

    if(!adr_found){
        SayAndPrintSaid(GREY_BLUE, "Can't find node with such object\n");
        return CANT_FIND_NODE;
    }

    stack_t_t* path = TreeFindPathToNode(adr_found, head->root);
    if(!path){
        return CANT_FIND_PATH;
    }

    SayAndPrintSaid(LIGHT_PURPLE, "%s -", adr_found->data);

    OutputCommands(path, adr_found, head->root, head);

    stack_free(path);

    err = TreeVerify(head);
    return err;
}

static void OutputCommands(stack_t_t* path, TreeNode_t* adr_found, TreeNode_t* st_node, TreeHead_t* head){
    assert(path);
    assert(st_node);
    assert(head);

    TreeNode_t* node = st_node;
    int elem = 0;
    while(node != adr_found){
        stack_pop(path, &elem);
        if(elem == LEFT){
            SayAndPrintSaid(LIGHT_PURPLE, " %s", node->data);
            node = node ->left;
        }
        if(elem == RIGHT){
            SayAndPrintSaid(LIGHT_PURPLE, " not %s", node->data);
            node = node ->right;
        }
    }
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// Find common and opposite

static TreeErr_t PrintOpposite(TreeNode_t* adr_found, TreeNode_t* common_node, TreeHead_t* head);

TreeErr_t TreeFindCommonOpposite(TreeHead_t* head, const char* name1, const char* name2){
    assert(name1);
    assert(name2);
    assert(head);

    TreeErr_t err = NO_MISTAKE;
    err = TreeVerify(head);
    if(err) return err;

    TreeNode_t* adr1_found = NULL;
    TreeFindNode(head->root, head, name1, &adr1_found);

    TreeNode_t* adr2_found = NULL;
    TreeFindNode(head->root, head, name2, &adr2_found);

    if(!adr1_found || !adr2_found){
        SayAndPrintSaid(GREY_BLUE, "Can't find node by name\n");
        return CANT_FIND_NODE;
    }

    TreeNode_t* common_node = TreeFindCommonNode(head, adr1_found, adr2_found);

    stack_t_t* path = TreeFindPathToNode(common_node, head->root);
    if(!path){
        return CANT_FIND_PATH;
    }

    SayAndPrintSaid(LIGHT_PURPLE, "Comparing %s and %s\n", name1, name2);

    SayAndPrintSaid(LIGHT_PURPLE, "Common: ");

    OutputCommands(path, common_node, head->root, head);

    stack_free(path);

    err = PrintOpposite(adr1_found, common_node, head);
    if(err) return err;

    err = PrintOpposite(adr2_found, common_node, head);
    if(err) return err;

    err = TreeVerify(head);
    return err;
}

static TreeErr_t PrintOpposite(TreeNode_t* adr_found, TreeNode_t* common_node, TreeHead_t* head){
    stack_t_t* path = TreeFindPathToNode(adr_found, common_node);
    if(!path){
        return CANT_FIND_PATH;
    }

    SayAndPrintSaid(LIGHT_PURPLE, "\nOpposite for %s: ", adr_found->data);

    OutputCommands(path, adr_found, common_node, head);

    stack_free(path);

    return NO_MISTAKE;
}

