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

static size_t count_symb(char* buffer, char symb){
    assert(buffer);

    size_t count = 0;
    for (const char *p = buffer; *p != '\0'; p++) {
        if (*p == symb) {
            count++;
        }
    }
    return count;
}

static bool is_nil(char* buffer, size_t p){
    if(!isalpha(buffer[p-1]) && buffer[p] == 'n' && buffer[p+1] == 'i' && buffer[p+2] == 'l' && !isalpha(buffer[p+3])){
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// Creating buffer for reading akinator info from disk

static bool incorr_work_with_stat(const char *name_of_file, struct stat *all_info_about_file);

static char* read_file_to_string_array(const char *name_of_file){
    assert(name_of_file != NULL);
    FILE *fptr = fopen(name_of_file, "r");
    assert(fptr != NULL);

    struct stat file_info = {};
    bool is_stat_err = incorr_work_with_stat(name_of_file, &(file_info));
    if (is_stat_err){
        return NULL;
    }

    char *all_strings_in_file = (char *)calloc(file_info.st_size + 1, sizeof(char));
    assert(all_strings_in_file != NULL);

    assert(fread(all_strings_in_file, sizeof(char), file_info.st_size, fptr) == file_info.st_size);

    fclose(fptr);
    return all_strings_in_file;
}

static bool incorr_work_with_stat(const char *name_of_file, struct stat *all_info_about_file){
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

static bool IsFileCorrect(char* buffer);

static TreeNode_t* ReadNode(size_t* pos, TreeNode_t* node_parent, char* buffer, TreeHead_t* head);

static void ReadStrchr(size_t* pos, char* buffer, TreeNode_t* node_for_header, TreeHead_t* head);

// проверка на битость файла - перед тем как создавать дерево
TreeHead_t* MakeAkinatorTree(const char *name_of_file){
    assert(name_of_file);

    char* buffer = read_file_to_string_array(name_of_file);
    if(!buffer){
        return NULL;
    }
    if(!IsFileCorrect(buffer)){
        fprintf(stderr, "File is not correct - can't work\n");
        return NULL;
    }
    TreeHead_t* head = TreeCtor(buffer);
    size_t pos = 0;
    head->root = ReadNode(&pos, NULL, buffer, head);
    if(TreeVerify(head)){
        fprintf(stderr, "File is not correct - can't work with created tree\n");
        FreeMemoryAtAkinatorTree(head);
        return NULL;
    }
    return head;
}

static bool IsFileCorrect(char* buffer){
    assert(buffer);
    size_t pos = 0;
    size_t temp_pos = 0;
    size_t first_symb = count_symb(buffer , '(');
    size_t second_symb = count_symb(buffer , ')');
    size_t third_symb = count_symb(buffer , '"');
    if(first_symb != second_symb){
        return false;
    }
    if(third_symb % 2 != 0){
        return false;
    }
    if(third_symb != 2 * first_symb || third_symb != 2 * second_symb){
        return false;
    }
    size_t count = 0;
    char* ch = buffer;
    for (size_t p = 1; buffer[p] != '\0'; p++) {
        if(is_nil(buffer, p)){
            count++;
        }
    }
    if(count % 2 != 0){
        return false;
    }
    return true;
}




static void ReadHeader(size_t* pos, char* buffer, TreeNode_t* node_for_header, TreeHead_t* head);

static TreeNode_t* ReadNode(size_t* pos, TreeNode_t* node_parent, char* buffer, TreeHead_t* head){
    assert(pos);
    assert(buffer);
    assert(head);

    skip_space(buffer, pos);
    if(buffer[(*pos)] == '('){
        TreeNode_t* node = NodeCtor(NULL, node_parent, NULL, NULL, false);
        (*pos)++; //skip '('
        skip_space(buffer, pos);

        // ReadStrchr(pos, buffer, node, head); 
        ReadHeader(pos, buffer, node, head); 
        skip_space(buffer, pos);

        node->left = ReadNode(pos, node, buffer, head);

        node->right = ReadNode(pos, node, buffer, head);

        tree_dump_func(node, head, "After making left && right|%s", __FILE__, __func__, __LINE__, buffer + (*pos));

        skip_space(buffer, pos);
        (*pos)++; //skip ')'
        skip_space(buffer, pos);
        return node;
    }
    else if(!strncmp(buffer + *pos, "nil", 3)){
        (*pos) += strlen("nil");
        return NULL;
    }
    return NULL;
}



static void ReadHeader(size_t* pos, char* buffer, TreeNode_t* node_for_header, TreeHead_t* head){
    assert(buffer);
    assert(pos);
    assert(node_for_header);
    assert(head);

    size_t len = 0;
    sscanf(buffer + *pos, " \"%*[^\"]\"%n", &len);
    (*pos)++;

    node_for_header->data = buffer + *pos;
    // если вдруг затесался знак вопроса-надо убрать будет мешать когда делаем определение
    size_t first_question = strcspn(node_for_header->data, "?");
    if(first_question >= len - 2){
        // len-2 потому что sscanf учитывает кавычки - а нам нужно получить чисто длину строки
        (*pos) += len - 2; 

        buffer[(*pos)] = '\0'; // чтобы в узел не копировался весь буффер
        (*pos)++; // нужно получить следующий символ после '\0'
    }
    else{
        (*pos) += first_question; // сдвигаемся до первого вопроса
        buffer[(*pos)] = '\0'; // заменяем его '\0' - обрезаем строку

        (*pos) += len - 2 - first_question; // сдвигаемся уже до кавычки
        buffer[(*pos)] = '\0'; // заменяем ее '\0' - обрезаем строку
        (*pos)++; // нужно получить следующий символ после '\0'
    }

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
    // если вдруг затесался знак вопроса-надо убрать будет мешать когда делаем определение
    size_t first_question = strcspn(node_for_header->data, "?");
    // сравниваем его с длиной строки - если он за пределами строки то нам это не интересно
    if(first_question >= len){
        (*pos) += len;

        buffer[(*pos)] = '\0'; // чтобы в узел не копировался весь буффер
        (*pos)++;  // нужно получить следующий символ после '\0'
    }
    else{
        (*pos) += first_question; // сдвигаемся до первого вопроса
        buffer[(*pos)] = '\0'; // заменяем его '\0' - обрезаем строку

        (*pos) += len - first_question; // сдвигаемся уже до кавычки
        buffer[(*pos)] = '\0'; // заменяем ее '\0' - обрезаем строку
        (*pos)++; // нужно получить следующий символ после '\0'
    }

    head->capacity++; // удобнее всего увеличить capacity здесь
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
    err = TreeVerify(head);
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
    assert(head);

    TreeErr_t err = NO_MISTAKE_T;
    err = TreeVerify(head);
    if(err) return err;

    fprintf(file, "( \"%s\"", node->data);

    if(node->left){
        PutAkinatorTreeToFile(file, node->left, head);
    } 
    else{
        fprintf(file, " nil");
    }

    if(node->right){
        PutAkinatorTreeToFile(file, node->right, head);
    } 
    else{
        fprintf(file, " nil");
    }

    fprintf(file, " ) ");
    err = TreeNodeVerify(node, head);
    return err;
}

//---------------------------------------------------------------------------
//--------------------------------------------------------------------------
// Free memory for data needed for akinator

void FreeMemoryAtAkinatorTree(TreeHead_t* head){
    TreeDel(head);

    free(head->buffer);
    head->buffer = NULL;
}