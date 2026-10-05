#include "ls.h"

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
		Entry all_entries[64] = { };
		size_t i = 0;

		while ((entry = readdir(dp))) {
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
			all_entries[i] = make_entry(entry->d_name, parent, info);
			++i;
		}
	
		print_entries(all_entries, i);
	
		if (flag & RECURSIVE) {
			for (size_t j = 0; j < i; ++j) {
				if (S_ISDIR(all_entries[j].info.st_mode)) {
					printf("%s:\n", all_entries[j].path);
					ls(all_entries[j].path);
				}
			}
		}
		free_entries(all_entries, i);
		closedir(dp);
	} else {
		Entry unique = make_entry((char*)name, (char*)name, st);
		print_entries(&unique, 1);
		free_entries(&unique, 1);
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
