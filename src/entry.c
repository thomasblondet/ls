#include "ls.h"

Entry make_entry(char* name, char* path, struct stat info) {
    Entry ent;
    ent.name = strdup(name);
    if (!ent.name) {
        fatal("out of memory");
    }
    ent.path = path;
    ent.info = info;
    return ent;
}

void print_entries(Entry* entries, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        printf("%s\n", entries[i].name);
    }
}

void free_entries(Entry* entries, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        free(entries[i].name);
        free(entries[i].path);
    }
}