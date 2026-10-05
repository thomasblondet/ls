#include "ls.h"

void ls(char const* path) {
	DIR* dp = opendir(path);
	struct dirent* entry;

	Entry all_entries[64];
	size_t i = 0;
	while ((entry = readdir(dp))) {
		if (entry->d_name[0] == '.')
			continue;
		struct stat info;
		lstat(entry->d_name, &info);
		all_entries[i] = make_entry(entry->d_name, info);
		++i;
	}
	print_entries(all_entries, i);
}

int main(void) {
	ls(".");
	return 0;
}
