#include <stdio.h>
#include <string.h>
#include <ctype.h>

void changeCase(char *str, int len) {
	for(int i = 0; i < len; ++i) {
		str[i] = toupper(str[i]);
	}
}

int main(int argc, char **argv) {
	if (argc != 3) {
		printf("Usage: encryption <original string> <encryption string>\n");
		return(1);
	}
	/* First argument will be string to be encrypted, second argument will be encryption template */
	int len1 = strlen(argv[1]);
	int len2 = strlen(argv[2]);
	/* preprocessing strings */
	changeCase(argv[1], len1);
	changeCase(argv[2], len2);
	char encrypted[len1];
	for (int i = 0; i < len1; ++i) {
		char diff = (argv[2][i%len2] - 'A') + argv[1][i];
		diff = 'A' + (diff - 'A')%26;
		encrypted[i] = diff;
	}
	printf("%s\n", encrypted);
}
