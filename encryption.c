#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void changeCase(char *str, int len) {
	for(int i = 0; i < len; ++i) {
		str[i] = toupper(str[i]);
	}
}

int main(int argc, char **argv) {
	if (argc != 4) {
		printf("Usage: encryption <original string> <encryption string><mode>\n");
		printf("mode 0=encrypt, mode 1=decrypt\n");
		return(1);
	}
	/* First argument will be string to be encrypted, second argument will be encryption template */
	int len1 = strlen(argv[1]);
	int len2 = strlen(argv[2]);
	int mode = atoi(argv[3]);
	/* preprocessing strings */
	changeCase(argv[1], len1);
	changeCase(argv[2], len2);
	char encrypted[len1+1];
	char diff;
	if (mode) {
		for (int i = 0; i < len1; ++i) {
			diff = argv[1][i] - (argv[2][i%len2] - 'A');
			diff = 'A' + (diff + 'A')%26;
			encrypted[i] = diff;
		}
	}
	else {
		for (int i = 0; i < len1; ++i) {
			diff = (argv[2][i%len2] - 'A') + argv[1][i];
			diff = 'A' + (diff - 'A')%26;
			encrypted[i] = diff;
		}
	}
	encrypted[len1] = '\0';
	printf("%s\n", encrypted);
}
