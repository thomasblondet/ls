#include "ls.h"

#define OBJ_SIZE 4

Entry make_entry(char* name, char* path, struct stat info)
{
    Entry ent;
    ent.name = strdup(name);
    if (!ent.name) {
        return ent;
    }
    ent.path = path;
    ent.info = info;
    return ent;
}

char* get_path(char const* parent, char const* entry_name)
{
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

Entry* get_entries(char const* dir_name, size_t* entries_len)
{
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

        if ((entry->d_name[0] == '.') & !(flag & ALL))
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

blkcnt_t total_blocks_count(Entry* entries, size_t len)
{
    blkcnt_t total = 0;
    size_t i = 0;

    do {
        total += entries[i].info.st_blocks;
        ++i;
    } while (i < len);

    return total;
}

void print_long_format(Entry entry)
{
    char modes[] = "----------";
    
    if (entry.name[0] == '.' && !(flag & ALL))
        return;

    if (S_ISLNK(entry.info.st_mode))
        modes[0] = 'l';
    if (S_ISDIR(entry.info.st_mode))
        modes[0] = 'd';

    /* owner */
    if (entry.info.st_mode & S_IRUSR)
        modes[1] = 'r';
    if (entry.info.st_mode & S_IWUSR)
        modes[2] = 'w';
    if (entry.info.st_mode & S_IXUSR)
        modes[3] = 'x';

    /* group */
    if (entry.info.st_mode & S_IRGRP)
        modes[4] = 'r';
    if (entry.info.st_mode & S_IWGRP)
        modes[5] = 'w';
    if (entry.info.st_mode & S_IXGRP)
        modes[6] = 'x';

    /* others */
    if (entry.info.st_mode & S_IROTH)
        modes[7] = 'r';
    if (entry.info.st_mode & S_IWOTH)
        modes[8] = 'w';
    if (entry.info.st_mode & S_IXOTH)
        modes[9] = 'x';

    struct passwd* pwd = getpwuid(entry.info.st_uid);
    struct group* grp = getgrgid(entry.info.st_gid);

    char time_buffer[BUFSIZ];
    struct tm* time = localtime(&entry.info.st_mtimespec.tv_sec);
    strftime(time_buffer, sizeof(time_buffer), "%b %e %H:%M", time);

    char link_buffer[BUFSIZ + 1] = { };
    char tmp[BUFSIZ];
    int n = 0;
    if (S_ISLNK(entry.info.st_mode)) {
        n = readlink(entry.name, link_buffer, BUFSIZ - 1);
        sprintf(tmp, "%s -> %s", entry.name, link_buffer);
    }
    
    printf("%s %2hu %-8s %-8s %6lld %s %s\n", modes, entry.info.st_nlink, pwd->pw_name,
        grp->gr_name, entry.info.st_size, time_buffer, (n == 0) ? entry.name : tmp);
}

void print_entries(Entry* entries, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        if (flag & LONG) {
            if (i == 0)
                printf("total %lld\n", (long long)(total_blocks_count(entries, len)));
            print_long_format(entries[i]);
        }
        else
            printf("%s\n", entries[i].name);
    }
}

void swap_entries(Entry* a, Entry* b)
{
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

void free_entries(Entry* entries, size_t len)
{
    size_t i = 0;

    do {
        if (entries[i].name)
            free(entries[i].name);
        free(entries[i].path);
        ++i;
    } while (i < len);

    free(entries);
}