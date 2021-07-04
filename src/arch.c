#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/utsname.h>

/*
arch - prints the current machine's architecture
Available arguments:
	--help: show this help message
	--version: show the version of the program (WIP)
*/

int main(int argc, char** argv) {
	if (argc == 1) {
		struct utsname e;
		uname(&e);
		printf(e.machine);
		return 0;
	} else {
		for (int i = 0; i < argc; i++) {
			char* arg = argv[i];
			/*printf(arg);
			printf(" ");*/
			if (!strcmp(arg, "--help")) {
				char* help = 
					"Drake's Epic Coreutils (working title)\n"
					"arch - prints the current machine's architecture\n"
					"Available arguments:\n"
					"	--help: show this help message\n"
					"	--version: show the version of the program (WIP)";
				printf("%s\n", help);
				return 0;
			} else if (!strcmp(arg, "--version")) {
				printf("6.9.69\n");
				return 0;
			}
		}
	}
}
