#include "ls.h"

int flag = 0;

void error(void) {
	if (errno) {
		printf("%s\n", strerror(errno));
	}
}

int compar(void const* a, void const* b) {
	Entry* e1 = (Entry*)a;
	Entry* e2 = (Entry*)b;
	return strcmp(e1->name, e2->name);
}

void sort(Entry* entries, size_t len) {
	qsort(entries, len, sizeof(Entry), compar);
}

void reverse(Entry* entries, size_t len) {
	size_t start = 0;

	do {
		swap_entries(&entries[start], &entries[len-1]);
		++start;
		--len;
	} while (start < len);
}

void ls(char const* name) {
	struct stat st;
	if (lstat(name, &st) == -1) {
		error();
		return;
	}

	if (S_ISDIR(st.st_mode)) {
		size_t len;
		Entry* entries = get_entries(name, &len);
		if (!entries) {
			error();
			return;
		}

		sort(entries, len);
		if (flag & REVERSE)
        	reverse(entries, len);
		print_entries(entries, len);

		if (flag & RECURSIVE) {
			for (size_t j = 0; j < len; ++j) {
				if (S_ISDIR(entries[j].info.st_mode)) {
					if (strcmp(entries[j].name, ".") && strcmp(entries[j].name, "..")) {
						printf("%s:\n", entries[j].path);
						ls(entries[j].path);
					}
				}
			}
		}

		free_entries(entries, len);

	} else {
		Entry file = make_entry((char*)name, (char*)name, st);
		if (!file.name) {
			error();
			return;
		}

		print_entries(&file, 1);
		free(file.name);
	}
}

int main(int argc, char* argv[]) {
	int c;

	while ((c = getopt(argc, argv, "aRrl")) != -1) {
		switch (c) {
		case 'a':
			flag |= ALL;
			break;
		case 'R':
			flag |= RECURSIVE;
			break;
		case 'r':
			flag |= REVERSE;
			break;
		case 'l':
			flag |= LONG;
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
