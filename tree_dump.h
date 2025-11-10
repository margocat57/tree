#ifndef LIST_DUMP_H
#define LIST_DUMP_H
#include <string.h>
#include <string.h>
#include "akinator.h"

const char* const LOG_FILE = "log.htm";

struct filenames_for_dump{
    char* dot_filename;
    char* svg_filename;
};

void tree_dump_func(const TreeNode_t* node, const TreeHead_t* head, const char* debug_msg, const char *file, const char *func, int line, ...) __attribute__ ((format (printf, 3, 7)));

#endif // LIST_DUMP_H