#ifndef LS_H
#define LS_H

#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define RECURSIVE 1

typedef struct {
    char* name;
    char* path;
    struct stat info;
} Entry;

Entry make_entry(char* name, char* path, struct stat info);
void print_entries(Entry* entries, size_t len);
void free_entries(Entry* entries, size_t len);
void fatal(char const*const str);

#endif
