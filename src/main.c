#include "ls.h"
#define OBJ_SIZE 4

int flag = 0;

void fatal(char const*const str) {
	perror(str);
	exit(1);
}

void ls(char const* name) {
	struct stat st;
	if (lstat(name, &st) == -1) {
		fatal("lstat");
	}

	if (S_ISDIR(st.st_mode)) {
		DIR* dp = opendir(name);
		if (!dp) {
			fatal("opendir");
		}

		struct dirent* entry;
		size_t len = 0, n = OBJ_SIZE;

		Entry* entries = calloc(n, sizeof(Entry));
		if (!entries) {
			fatal("out of memory");
		}
		
		while ((entry = readdir(dp))) {
			if (len >= n) {
				n *= 2;
				Entry* ptr = realloc(entries, n * sizeof(Entry));
				if (!entries) {
					free(entries);
					fatal("out of memory");
				}
				entries = ptr;
			}
			
			if (entry->d_name[0] == '.')
				continue;

			char* parent = nullptr;
			if (!strcmp(name, ".")) {
				parent = strdup(entry->d_name);
			} else {
				parent = malloc(strlen(name) + 2 + strlen(entry->d_name) + 1);
				strcpy(parent, name);
				strcat(parent, "/");
				strcat(parent, entry->d_name);
			}
			struct stat info;
			if (lstat(parent, &info) == -1) {
				fatal("lstat");
			}
			entries[len] = make_entry(entry->d_name, parent, info);
			++len;
		}
	
		print_entries(entries, len);

		if (flag & RECURSIVE) {
			for (size_t j = 0; j < len; ++j) {
				if (S_ISDIR(entries[j].info.st_mode)) {
					printf("%s:\n", entries[j].path);
					ls(entries[j].path);
				}
			}
		}

		free_entries(entries, len);
		closedir(dp);
	} else {
		Entry unique = make_entry((char*)name, (char*)name, st);
		print_entries(&unique, 1);
		free(unique.name);
	}
}

int main(int argc, char* argv[]) {
	int c;

	while ((c = getopt(argc, argv, "R")) != -1) {
		switch (c) {
		case 'R':
			flag |= RECURSIVE;
			break;
		default:
			break;
		}
	}
	argc -= optind;
    argv += optind;

	if (!argc) {
		ls(".");
	} else {
		for (int i = 0; i < argc; ++i) {
			ls(argv[i]);
		}
	}

	return 0;
}
