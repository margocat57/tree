#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "tree_func.h"
#include "input_output.h"
#include "tree_dump.h"
#include "my_assert_without_ndebug.h"
//----------------------------------------------------------------------------
// Helping functions to find spaces

static void skip_space(char* str, size_t* pos){
    char ch = str[(*pos)];
    while(isspace(ch) && ch != '\0'){
        (*pos)++;
        ch = str[(*pos)];
    }
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// Creating buffer for reading akinator info from disk

static bool incorr_work_with_stat(const char *name_of_file, struct stat *all_info_about_file);

static char* read_file_to_string_array(const char *name_of_file){
    MY_ASSERT_WTHOUT_NDEBUG(name_of_file != NULL);
    FILE *fptr = fopen(name_of_file, "r");
    MY_ASSERT_WTHOUT_NDEBUG(fptr != NULL);

    struct stat file_info = {};
    bool is_stat_err = incorr_work_with_stat(name_of_file, &(file_info));
    if (is_stat_err){
        return NULL;
    }

    char *all_strings_in_file = (char *)calloc(file_info.st_size + 1, sizeof(char));
    MY_ASSERT_WTHOUT_NDEBUG(all_strings_in_file != NULL);

    MY_ASSERT_WTHOUT_NDEBUG(fread(all_strings_in_file, sizeof(char), file_info.st_size, fptr) == file_info.st_size);

    fclose(fptr);
    return all_strings_in_file;
}

static bool incorr_work_with_stat(const char *name_of_file, struct stat *all_info_about_file){
    MY_ASSERT_WTHOUT_NDEBUG(name_of_file != NULL);
    MY_ASSERT_WTHOUT_NDEBUG(all_info_about_file != NULL);

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

static TreeNode_t* ReadNode(size_t* pos, TreeNode_t* node_parent, char* buffer, TreeHead_t* head);

static void ReadStrchr(size_t* pos, char* buffer, TreeNode_t* node_for_header, TreeHead_t* head);

TreeHead_t* MakeAkinatorTree(const char *name_of_file){
    char* buffer = read_file_to_string_array(name_of_file);
    if(!buffer){
        return NULL;
    }
    TreeHead_t* head = TreeCtor(buffer);
    size_t pos = 0;
    head->root = ReadNode(&pos, NULL, buffer, head);
    return head;
}

static void ReadHeader(size_t* pos, char* buffer, TreeNode_t* node_for_header, TreeHead_t* head);

static void Dump(size_t* pos, char* buffer, TreeNode_t* node, TreeHead_t* head){
    fprintf(stderr, "-------\n", node->data);
    if(node->parent){
        fprintf(stderr, "%s\n", node->data);
        node = node->parent;
    }
    fprintf(stderr, "after %s\n", node->data);
    tree_dump_func(node, head, "%s", __FILE__, __func__, __LINE__, buffer + (*pos));
}

static TreeNode_t* ReadNode(size_t* pos, TreeNode_t* node_parent, char* buffer, TreeHead_t* head){
    skip_space(buffer, pos);
    if(buffer[(*pos)] == '('){
        TreeNode_t* node = NodeCtor(NULL, node_parent, NULL, NULL, false);
        (*pos)++; //skip '('
        skip_space(buffer, pos);

        ReadStrchr(pos, buffer, node, head); 
        skip_space(buffer, pos);

        // Dump(pos, buffer, node, head);
        tree_dump_func(node, head, "Before making node->left|%s", __FILE__, __func__, __LINE__, buffer + (*pos));

        node->left = ReadNode(pos, node, buffer, head);

        // Dump(pos, buffer, node, head);
        tree_dump_func(node, head, "Before making node->right|%s", __FILE__, __func__, __LINE__, buffer + (*pos));

        node->right = ReadNode(pos, node, buffer, head);

        // Dump(pos, buffer, node, head);
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
    size_t len = 0;
    sscanf(buffer + *pos, " \"%*[^\"]\"%n", &len);
    (*pos)++;

    node_for_header->data = buffer + *pos;
    (*pos) += len - 2;

    buffer[(*pos)] = '\0'; // чтобы в узел не копировался весь буффер
    (*pos)++;

    head->capacity++;
}

static void ReadStrchr(size_t* pos, char* buffer, TreeNode_t* node_for_header, TreeHead_t* head){
    size_t len = 0;
    skip_space(buffer, pos);
    (*pos)++; // skip "
    node_for_header->data = buffer + *pos;

    char* ch = strchr(buffer + *pos, '"');
    len = ch - (buffer + *pos);

    (*pos) += len;
    buffer[(*pos)] = '\0'; // чтобы в узел не копировался весь буффер
    (*pos)++;

    head->capacity++;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// For save akinator to disk

static TreeErr_t PutAkinatorTreeToFile(FILE *file, TreeNode_t *node, const TreeHead_t* head);

TreeErr_t PutAkinatorFile(const char* file_name, TreeNode_t *node, const TreeHead_t* head){
    TreeErr_t err = NO_MISTAKE_T;
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
    TreeErr_t err = NO_MISTAKE_T;
    err = TreeNodeVerify(head->root, head);
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