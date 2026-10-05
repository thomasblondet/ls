#include "ls.h"

Entry make_entry(char* name, struct stat info) {
    Entry ent = {
        .name = strdup(name),
        .info = info
    };
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
    }
}