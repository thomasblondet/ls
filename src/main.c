#include "ls.h"

int flag = 0;

void error(void) {
	if (errno) {
		printf("%s\n", strerror(errno));
	}
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
