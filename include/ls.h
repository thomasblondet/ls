#ifndef LS_H
#define LS_H

#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>

typedef struct {
    char* name;
    struct stat info;
} Entry;

Entry make_entry(char* name, struct stat info);
void print_entries(Entry* entries, size_t len);
void free_entries(Entry* entries, size_t len);

#endif
