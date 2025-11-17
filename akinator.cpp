#include "akinator.h"
#include "tree_dump.h"
#include "tree_func.h"
#include "input_output.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

const size_t MAX_SHORT_ANSWER_SIZE = 8;
const size_t MAX_QUESTION_SIZE = 100;
const size_t MAX_PERSON_NAME_SIZE = 16;
const char* YES = "yes";
const char* NO = "no";

#define CHECK_AND_RET(bad_condition, msg, err)\
    if(bad_condition){ \
        fprintf(stderr, msg); \
        return err; \
    } \

static TreeErr_t TreeAddObjAndQuestion(TreeNode_t* node, TreeHead_t* head);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Small functions that used in many other functions

static char* str_tolower(char* str){
    assert(str);
    size_t len = strlen(str);
    for (size_t ch = 0; ch < len; ch++){
        str[ch] = tolower(str[ch]);
    }
    return str;
}

static void clear_input_buffer(void){
    int c = 0; 
    do{
        c = getchar();
    }while(c != '\n');
}

static void akinator_scanf(char* buffer, size_t max_num_of_symb_to_read){
    printf(LIGHT_PURPLE); 
    fgets(buffer, max_num_of_symb_to_read, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    printf(COLOR_RESET);
}

static void akinator_char_scanf(char* buffer){
    printf(LIGHT_PURPLE); 
    scanf("%c", buffer);
    clear_input_buffer();
    printf(COLOR_RESET);
}

//-------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// Akinator menu - function

static void PrintMenu();

static TreeErr_t MenuDefinition(TreeHead_t* head);

static TreeErr_t MenuCompare(TreeHead_t* head);

static void MenuDump(TreeHead_t* head);

static TreeErr_t MenuSaveQuit(TreeHead_t* head);

TreeErr_t AkinatorMenuAndMainFunc(TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE;
    DEBUG_TREE(err = TreeVerify(head);)
    if(err) return err;

    bool is_quit = false;
    char choise = 'x';

    while(!is_quit){
        PrintMenu();

        akinator_char_scanf(&choise);
        switch(choise){
            case 'G':   CHECK_AND_RET_TREEERR(TreeAkinate(head));                  break;
            case 'M':   CHECK_AND_RET_TREEERR(MenuDefinition(head));               break;
            case 'C':   CHECK_AND_RET_TREEERR(MenuCompare(head));                  break;   
            case 'D':   MenuDump(head);                                            break;
            case 'S':   CHECK_AND_RET_TREEERR(MenuSaveQuit(head)); is_quit = true; break;
            default:    is_quit = true;                                            break;
        }
    }

    DEBUG_TREE(err = TreeVerify(head));
    return err;
}

static void PrintMenu(){
    SayAndPrintSaid(GREY_BLUE, "\nWhat do you want?\n");
    printf( GREY_BLUE   "[G]uess? (put G)\n" 
                        "[M]ake definition? (put M)\n"
                        "[C]ompare objects? (put C)\n" 
                        "[D]ump tree? (put D)\n" 
                        "[S]ave and quit? (put S)\n" 
                        "Quit without saving? (put other letter)\n" COLOR_RESET);
}

static TreeErr_t MenuDefinition(TreeHead_t* head){
    char buffer[MAX_PERSON_NAME_SIZE] = {};
    SayAndPrintSaid(GREY_BLUE, "Definition of who do you want to make?\n");
    akinator_scanf(buffer, MAX_PERSON_NAME_SIZE);
    return TreeMakeDefinition(head, buffer);
}

static TreeErr_t MenuCompare(TreeHead_t* head){
    char buffer[MAX_PERSON_NAME_SIZE] = {};
    char buffer2[MAX_PERSON_NAME_SIZE] = {};

    SayAndPrintSaid(GREY_BLUE, "Compare who do you want?\n");
    printf(GREY_BLUE); printf("At first: "); printf(COLOR_RESET);
    akinator_scanf(buffer, MAX_PERSON_NAME_SIZE);
    printf(GREY_BLUE); printf("and "); printf(COLOR_RESET);
    akinator_scanf(buffer2, MAX_PERSON_NAME_SIZE);
    return TreeFindCommonOpposite(head, buffer, buffer2); 
}

static void MenuDump(TreeHead_t* head){
    char buffer_dump[MAX_QUESTION_SIZE] = {};
    SayAndPrintSaid(GREY_BLUE, "Enter the reason of dump:\n");
    akinator_scanf(buffer_dump, MAX_QUESTION_SIZE);
    tree_dump_func(head->root, head, buffer_dump, __FILE__, __func__,  __LINE__);
    SayAndPrintSaid(GREY_BLUE, "Tree dumped to log file\n");
}

static TreeErr_t MenuSaveQuit(TreeHead_t* head){
    char buffer_file[MAX_QUESTION_SIZE] = {};

    SayAndPrintSaid(GREY_BLUE, "Put name of file where write:\n" COLOR_RESET);
    akinator_scanf(buffer_file, MAX_QUESTION_SIZE);
    return PutAkinatorFile(buffer_file, head->root, head);
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
    DEBUG_TREE(err = TreeVerify(head);)
    if(err) return err;

    if(!StartGame(head)){
        DEBUG_TREE(err = TreeVerify(head);)
        return err;
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

    DEBUG_TREE(err = TreeNodeVerify(node, head);)
    return err;
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
    akinator_scanf(answer, MAX_SHORT_ANSWER_SIZE);
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
    akinator_scanf(again, MAX_SHORT_ANSWER_SIZE);
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
    DEBUG_TREE(err = TreeNodeVerify(node, head);)
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

    DEBUG_TREE(err = TreeNodeVerify(node, head);)
    return err;
}

static TreeNode_t* AddPerson(TreeNode_t* node, const char* str){
    assert(str);
    assert(node);

    SayAndPrintSaid(GREY_BLUE, "%s\n", str);
    char person[MAX_PERSON_NAME_SIZE] = {};
    akinator_scanf(person, MAX_PERSON_NAME_SIZE);
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
    akinator_scanf(question, MAX_QUESTION_SIZE);
}

static bool IsForbiddenSymbolsInStr(char* question){
    assert(question);
    char *copy_string = strdup(question);
    copy_string = str_tolower(copy_string);

    // взять указатель - что слеова что справа
    bool is_forbidden_symbols =  strstr(copy_string, " no") || strstr(copy_string, " not");
    free(copy_string);

    return is_forbidden_symbols;
}

static bool IsFeatureTrueForNewPerson(char* answer, TreeNode_t* node_left){
    assert(answer);
    assert(node_left);
    SayAndPrintSaid(GREY_BLUE, "Is you feature true for %s?(input %s if not)\n", node_left->data, NO);
    akinator_scanf(answer, MAX_SHORT_ANSWER_SIZE);
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
    DEBUG_TREE(err = TreeVerify(head);)
    if(err) return err;

    SayAndPrintSaid(GREY_BLUE, "Akinator is empty - need to add first question\n");
    TreeQuestion(head->root);

    head->root->left = AddPerson(head->root, "For who this feature is true?");
    head->root->right = AddPerson(head->root, "For who this feature is false?");
    head->capacity += 2;

    DEBUG_TREE(err = TreeVerify(head);)
    return err;
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// Telling definition of object

static void OutputDefinitionAndOpp(stack_t_t* path, TreeNode_t* node);

TreeErr_t TreeMakeDefinition(TreeHead_t* head, const char* name){
    assert(head);
    assert(name);

    TreeErr_t err = NO_MISTAKE;
    DEBUG_TREE(err = TreeVerify(head);)
    if(err) return err;

    stack_t_t* path = stack_ctor(head->capacity, __FILE__, __func__, __LINE__);

    bool is_found = false;
    CHECK_AND_RET(CreatePath(head->root, head, name, path, &is_found), 
                "Can't create correect path", CANT_MAKE_DEF_COMP);
    if(!is_found){
        fprintf(stderr, "incorr name - can't find");
        return CANT_MAKE_DEF_COMP;
    }

    SayAndPrintSaid(LIGHT_PURPLE, "%s -", name);

    OutputDefinitionAndOpp(path, head->root);

    stack_free(path);

    DEBUG_TREE(err = TreeVerify(head);)
    return err;
}

static void OutputDefinitionAndOpp(stack_t_t* path, TreeNode_t* node){
    assert(path);
    assert(node);

    int elem = 0;
    while(!stack_pop(path, &elem)){
        if(elem == LEFT){
            SayAndPrintSaid(LIGHT_PURPLE, " %s", node->data);
            node = node ->left;
        }
        else if(elem == RIGHT){
            SayAndPrintSaid(LIGHT_PURPLE, " not %s", node->data);
            node = node ->right;
        }
    }
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// Find common and opposite

static void PrintCommon(stack_t_t* path1, stack_t_t* path2, TreeNode_t** node);

TreeErr_t TreeFindCommonOpposite(TreeHead_t* head, const char* name1, const char* name2){
    assert(name1);
    assert(name2);
    assert(head);

    TreeErr_t err = NO_MISTAKE;
    DEBUG_TREE(err = TreeVerify(head);)
    if(err) return err;

    stack_t_t* path1 = stack_ctor(head->capacity, __FILE__, __func__, __LINE__);

    bool is_found1 = false;
    CHECK_AND_RET(CreatePath(head->root, head, name1, path1, &is_found1), 
                "Can't cteate correect path", CANT_MAKE_DEF_COMP);
    
    stack_t_t* path2 = stack_ctor(head->capacity, __FILE__, __func__, __LINE__);

    bool is_found2 = false;
    CHECK_AND_RET(CreatePath(head->root, head, name2, path2, &is_found2), 
                "Can't cteate correect path", CANT_MAKE_DEF_COMP);

    if(!is_found1 || !is_found2){
        fprintf(stderr, "incorr name - can't find");
        return CANT_MAKE_DEF_COMP;
    }

    SayAndPrintSaid(LIGHT_PURPLE, "Comparing %s and %s\n", name1, name2);

    SayAndPrintSaid(LIGHT_PURPLE, "Common: ");

    TreeNode_t* common_node = head->root;
    PrintCommon(path1, path2, &common_node);

    SayAndPrintSaid(LIGHT_PURPLE, "\nOpposite for %s:", name1);
    OutputDefinitionAndOpp(path1, common_node);

    SayAndPrintSaid(LIGHT_PURPLE, "\nOpposite for %s:", name2);
    OutputDefinitionAndOpp(path2, common_node);

    stack_free(path1);
    stack_free(path2);

    DEBUG_TREE(err = TreeVerify(head);)
    return err;
}

static void PrintCommon(stack_t_t* path1, stack_t_t* path2, TreeNode_t** node){
    assert(path1 && path2 && node && *node);

    int elem1 = 0;
    int elem2 = 0;
    stack_pop(path1, &elem1);
    stack_pop(path2, &elem2);
    while(elem1 == elem2){
        if(elem1 == LEFT){
            SayAndPrintSaid(LIGHT_PURPLE, " %s", (*node)->data);
            (*node) = (*node) ->left;
        }
        else if(elem2 == RIGHT){
            SayAndPrintSaid(LIGHT_PURPLE, " not %s", (*node)->data);
            (*node) = (*node) ->right;
        }
        stack_pop(path1, &elem1);
        stack_pop(path2, &elem2);
    }

    // потому что удалили первый несовпадающий элемент
    stack_push(path1, &elem1);
    stack_push(path2, &elem2);
}

