#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "tree_func.h"
#include "input_output.h"
#include "tree_dump.h"

void FreeMemoryAtAkinatorTree(TreeHead_t* head);


//----------------------------------------------------------------------------
// Helping functions to find spaces

static void skip_space(char* str, size_t* pos){
    assert(str);
    assert(pos);

    char ch = str[(*pos)];
    while(isspace(ch) && ch != '\0'){
        (*pos)++;
        ch = str[(*pos)];
    }
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// Creating buffer for reading akinator info from disk

static bool is_stat_err(const char *name_of_file, struct stat *all_info_about_file);

static char* read_file_to_string_array(const char *name_of_file){
    assert(name_of_file != NULL);
    FILE *fptr = fopen(name_of_file, "r");
    if(!fptr){
        fprintf(stderr, "Can't open file\n");
        return NULL;
    }

    struct stat file_info = {};
    if(is_stat_err(name_of_file, &(file_info))){
        return NULL;
    }

    char *all_strings_in_file = (char *)calloc(file_info.st_size + 1, sizeof(char));
    if(!all_strings_in_file){
        fprintf(stderr, "Array for strings allocation error\n");
        return NULL;
    }

    if(fread(all_strings_in_file, sizeof(char), file_info.st_size, fptr) != file_info.st_size){
        fprintf(stderr, "Can't read all symbols from file\n");
        return NULL;
    }

    fclose(fptr);
    return all_strings_in_file;
}

static bool is_stat_err(const char *name_of_file, struct stat *all_info_about_file){
    assert(name_of_file != NULL);
    assert(all_info_about_file != NULL);

    if (stat(name_of_file, all_info_about_file) == -1){
        perror("Stat error");
        fprintf(stderr, "Error code: %d\n", errno);
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// Make akinator tree

static TreeErr_t ReadNode(size_t* pos, TreeNode_t* node_parent, char* buffer, TreeNode_t** node_to_write, TreeHead_t* head);

static void ReadStrchr(size_t* pos, char* buffer, TreeNode_t* node_for_header, TreeHead_t* head);

TreeHead_t* MakeAkinatorTree(const char *name_of_file){
    assert(name_of_file);

    char* buffer = read_file_to_string_array(name_of_file);
    if(!buffer){
        return NULL;
    }
    TreeHead_t* head = TreeCtor(buffer);
    size_t pos = 0;
    if(ReadNode(&pos, NULL, buffer, &(head->root), head)){
        FreeMemoryAtAkinatorTree(head);
        return NULL;
    }
    DEBUG_TREE(
    if(TreeVerify(head)){
        fprintf(stderr, "File is not correct - can't work with created tree\n");
        FreeMemoryAtAkinatorTree(head);
        return NULL;
    }
    )
    return head;
}

static void ReadHeader(size_t* pos, char* buffer, TreeNode_t* node_for_header, TreeHead_t* head);


static TreeErr_t ReadNode(size_t* pos, TreeNode_t* node_parent, char* buffer, TreeNode_t** node_to_write, TreeHead_t* head){
    assert(pos);
    assert(buffer);
    assert(head);
    assert(node_to_write);

    skip_space(buffer, pos);
    if(buffer[(*pos)] == '('){
        TreeNode_t* node = NodeCtor(NULL, node_parent, NULL, NULL, false);
        (*pos)++; //skip '('
        skip_space(buffer, pos);

        if(buffer[(*pos)] != '"'){
            fprintf(stderr, "Incorr file no \"\n");
            return INCORR_FILE;
        }

        // ReadStrchr(pos, buffer, node, head); 
        ReadHeader(pos, buffer, node, head); 
        skip_space(buffer, pos);

        if(ReadNode(pos, node, buffer, &(node->left) ,head)){
            fprintf(stderr, "Incorr file\n");
            return INCORR_FILE;
        }

        if(ReadNode(pos, node, buffer, &(node->right) ,head)){
            fprintf(stderr, "Incorr file\n");
            return INCORR_FILE;
        }

        tree_dump_func(node, head, "After making left && right|%s", __FILE__, __func__, __LINE__, buffer + (*pos));

        skip_space(buffer, pos);
        if(buffer[(*pos)] != ')'){
            fprintf(stderr, "Incorr file no )\n");
            return INCORR_FILE;
        }
        (*pos)++; //skip ')'
        skip_space(buffer, pos);
        *node_to_write = node;
        return NO_MISTAKE_T;
    }
    else if(!strncmp(buffer + *pos, "nil", 3)){
        (*pos) += strlen("nil");
        *node_to_write = NULL;
        skip_space(buffer, pos);
        return NO_MISTAKE_T;
    }
    fprintf(stderr, "Incorr file undef symbol(%s), pos(%llu)\n", buffer + *pos, *pos);
    return INCORR_FILE;
}

//-------------------------------------------------------------------------------------------
// Reading string from file

static void FindQuestionAndCutBuff(size_t* pos, char* buffer, TreeNode_t* node_for_header, size_t len_str_without_quotes);

static void ReadHeader(size_t* pos, char* buffer, TreeNode_t* node_for_header, TreeHead_t* head){
    assert(buffer);
    assert(pos);
    assert(node_for_header);
    assert(head);

    size_t len = 0;
    sscanf(buffer + *pos, " \"%*[^\"]\"%n", &len);
    (*pos)++;

    node_for_header->data = buffer + *pos;

    FindQuestionAndCutBuff(pos, buffer, node_for_header, len - 2);

    head->capacity++; // удобнее всего увеличить capacity здесь
}

static void ReadStrchr(size_t* pos, char* buffer, TreeNode_t* node_for_header, TreeHead_t* head){
    assert(buffer);
    assert(pos);
    assert(node_for_header);
    assert(head);

    size_t len = 0;
    skip_space(buffer, pos);
    (*pos)++; // skip "
    node_for_header->data = buffer + *pos;

    char* ch = strchr(buffer + *pos, '"');
    len = ch - (buffer + *pos);
    FindQuestionAndCutBuff(pos, buffer, node_for_header, len);

    head->capacity++; // удобнее всего увеличить capacity здесь
}

//-------------------------------------------------------------------------------------------
// Finding question mark and cutting buffer for

static void CutBuffer(size_t* pos, char* buffer, size_t len);

static void CutQuestionAndBuffer(size_t* pos, char* buffer, size_t len, size_t first_question);

static void FindQuestionAndCutBuff(size_t* pos, char* buffer, TreeNode_t* node_for_header, size_t len_str_without_quotes){
    // если вдруг затесался знак вопроса-надо убрать будет мешать когда делаем определение
    size_t first_question = strcspn(node_for_header->data, "?"); 
    if(first_question >= len_str_without_quotes){
        CutBuffer(pos, buffer, len_str_without_quotes);
    }
    else{
        CutQuestionAndBuffer(pos, buffer, len_str_without_quotes, first_question);
    }
}

static void CutBuffer(size_t* pos, char* buffer, size_t len){
    (*pos) += len; 

    buffer[(*pos)] = '\0'; // чтобы в узел не копировался весь буффер
    (*pos)++; // нужно получить следующий символ после '\0'
}

static void CutQuestionAndBuffer(size_t* pos, char* buffer, size_t len, size_t first_question){
    (*pos) += first_question; // сдвигаемся до первого вопроса
    buffer[(*pos)] = '\0'; // заменяем его '\0' - обрезаем строку
    CutBuffer(pos, buffer, len - first_question); //режем буффер уже стандартно
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// For save akinator to disk

static TreeErr_t PutAkinatorTreeToFile(FILE *file, TreeNode_t *node, const TreeHead_t* head);

TreeErr_t PutAkinatorFile(const char* file_name, TreeNode_t *node, const TreeHead_t* head){
    if(!file_name){
        fprintf(stderr, "INCORRECT OUTPUT FILE\n");
        return INCORR_OUTPUT_FILE;
    }
    assert(head);

    TreeErr_t err = NO_MISTAKE_T;
    DEBUG_TREE(err = TreeVerify(head);)
    if(err) return err;

    FILE* fp = fopen(file_name, "w");
    if(!fp){
        fprintf(stderr, "Can't open output file\n");
        return CANT_OPEN_OUT_FILE;
    }

    CHECK_AND_RET_TREEERR(PutAkinatorTreeToFile(fp, node, head))
    fclose(fp);

    DEBUG_TREE(err = TreeNodeVerify(node, head);)
    return err;
}

static TreeErr_t PutAkinatorTreeToFile(FILE *file, TreeNode_t *node, const TreeHead_t* head){
    if(!file){
        fprintf(stderr, "INCORRECT OUTPUT FILE\n");
        return INCORR_OUTPUT_FILE;
    }
    assert(head);

    TreeErr_t err = NO_MISTAKE_T;
    DEBUG_TREE(err = TreeVerify(head);)
    if(err) return err;

    fprintf(file, "( \"%s\"", node->data);

    if(node->left){
        CHECK_AND_RET_TREEERR(PutAkinatorTreeToFile(file, node->left, head))
    } 
    else{
        fprintf(file, " nil");
    }

    if(node->right){
        CHECK_AND_RET_TREEERR(PutAkinatorTreeToFile(file, node->right, head));
    } 
    else{
        fprintf(file, " nil");
    }

    fprintf(file, " ) ");
    DEBUG_TREE(err = TreeNodeVerify(node, head);)
    return err;
}

//---------------------------------------------------------------------------
//--------------------------------------------------------------------------
// Free memory for data needed for akinator

void FreeMemoryAtAkinatorTree(TreeHead_t* head){
    free(head->buffer);
    head->buffer = NULL;

    TreeDel(head);
}