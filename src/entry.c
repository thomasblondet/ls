#include "ls.h"

#define OBJ_SIZE 4

Entry make_entry(char* name, char* path, struct stat info) {
    Entry ent;
    ent.name = strdup(name);
    if (!ent.name) {
        return ent;
    }
    ent.path = path;
    ent.info = info;
    return ent;
}

char* get_path(char const*const parent, char const*const entry_name) {
	char* path = nullptr;

	if (!strcmp(parent, ".")) {
		path = strdup(entry_name);
		if (!path)
			return NULL;
	} else {
		path = malloc(strlen(parent) + 2 + strlen(entry_name) + 1);
		if (!path)
			return NULL;
		strcpy(path, parent);
		strcat(path, "/");
		strcat(path, entry_name);
	}
	return path;
}

Entry* get_entries(char const* dir_name, size_t* entries_len) {
    DIR* dp = opendir(dir_name);
    if (!dp)
        return nullptr;

    struct dirent* entry;
    size_t len = 0, n = OBJ_SIZE;
    Entry* entries = calloc(n, sizeof(Entry));
    if (!entries) {
        goto cleanup_dir;
    }

    while ((entry = readdir(dp))) {
        if (len >= n) {
            n *= 2;
            Entry* ptr = realloc(entries, n * sizeof(Entry));
            if (!ptr) {
                goto cleanup_entries;
            }
            entries = ptr;
		}

        if (entry->d_name[0] == '.')
            continue;

        char* parent = get_path(dir_name, entry->d_name);
        if (!parent) {
            goto cleanup_entries;
        }

        struct stat info;
        if (lstat(parent, &info) == -1) {
            free(parent);
            goto cleanup_entries;
        }
        entries[len] = make_entry(entry->d_name, parent, info);
        if (!entries[len].name) {
            goto cleanup_entries;
        }

        ++len;
    }

    *entries_len = len;
    closedir(dp);
    return entries;

    cleanup_entries:
        free_entries(entries, len);
        closedir(dp);
        return nullptr;
    
    cleanup_dir:
        closedir(dp);
        return nullptr;
}

void print_entries(Entry* entries, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        printf("%s\n", entries[i].name);
    }
}

void swap_entries(Entry* a, Entry* b) {
    Entry temp = {
        .info = a->info,
        .name = a->name,
        .path = a->path
    };

    a->info = b->info;
    a->name = b->name;
    a->path = b->path;

    b->info = temp.info;
    b->name = temp.name;
    b->path = temp.path;
}

void free_entries(Entry* entries, size_t len) {
    size_t i = 0;
    do {
        if (entries[i].name)
            free(entries[i].name);
        free(entries[i].path);
        ++i;
    } while (i < len);

    free(entries);
}