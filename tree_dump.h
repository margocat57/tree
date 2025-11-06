#ifndef LIST_DUMP_H
#define LIST_DUMP_H
#include <string.h>
#include "tree.h"

const char* const LOG_FILE = "log.htm";

struct filenames_for_dump{
    char* dot_filename;
    char* svg_filename;
};

void tree_dump_func(const TreeNode_t* node, const char* debug_msg, const char *file, const char *func,  int line);

#endif // LIST_DUMP_H