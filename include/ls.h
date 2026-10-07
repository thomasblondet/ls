#ifndef LS_H
#define LS_H

#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <uuid/uuid.h>

#define ALL 1
#define LONG 2
#define REVERSE 4
#define RECURSIVE 8

typedef struct {
    char* name;
    char* path;
    struct stat info;
} Entry;

extern int flag;

Entry* get_entries(char const* dir_name, size_t* entries_len);
Entry make_entry(char* name, char* path, struct stat info);
void print_entries(Entry* entries, size_t len);
void swap_entries(Entry* a, Entry* b);
void free_entries(Entry* entries, size_t len);

#endif
