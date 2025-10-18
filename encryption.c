#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLEN 100

void changeCase(char *str, int len) {
	for(int i = 0; i < len; ++i) {
		str[i] = toupper(str[i]);
	}
}

int main(void) {
	/* First argument will be string to be encrypted, second argument will be encryption template */
	/* Third argument will be encryption mode */
	char str1[MAXLEN], str2[MAXLEN];
	int mode;
	fgets(str1, MAXLEN, stdin);
	fgets(str2, MAXLEN, stdin);
	scanf("%d", &mode);
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	--len1; --len2; // fgets adds a /n character to every string processed
	/* preprocessing strings */
	changeCase(str1, len1);
	changeCase(str2, len2);
	char encrypted[len1+1];
	char diff;
	if (mode) {
		for (int i = 0; i < len1; ++i) {
			diff = str1[i] - (str2[i%len2] - 'A');
			diff = 'A' + (diff + 'A')%26;
			encrypted[i] = diff;
		}
	}
	else {
		for (int i = 0; i < len1; ++i) {
			diff = (str2[i%len2] - 'A') + str1[i];
			diff = 'A' + (diff - 'A')%26;
			encrypted[i] = diff;
		}
	}
	encrypted[len1] = '\0';
	printf("%s\n", encrypted);
	return 0;
}
