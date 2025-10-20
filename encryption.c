#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLEN 100
#define NUMCOMMANDS 4

char *commands[NUMCOMMANDS] = {"PASSKEY", "ENCRYPT", "DECRYPT", "QUIT"};

void changeCase(char *str, int len) {
	for(int i = 0; i < len; ++i) {
		str[i] = toupper(str[i]);
	}
}

static inline char checkCommand(char *str) {
	char check = -1; // I didn't want to import bool.h so I'm just going to use a char instead
	for (int i = 0; i < NUMCOMMANDS; ++i) {
		if (!strcmp(str, commands[i])) {
			check = i;
		}
	}
	return check;
}

static inline char checkAlpha(char *str, int len) {
	for (int i = 0; i < len; ++i) {
		if (!isalpha(str[i])) {
			return -1;
		}
	}
	return 0;
}

int main(void) {
	char buf[MAXLEN+10]; // maximum string size 
	char passkey[MAXLEN];
	passkey[0] = '\0';
	char quit = 0;

	char c;
	while (true) {
		read(STDIN_FILENO, buf, MAXLEN+10);
		buf[strcspn(buf, "\n")] = '\0';
		char *command = strtok(buf, " ");
		changeCase(command, strlen(command));

		char check = checkCommand(command);

		if (check == 3) {
			break;
		}

		if (check == -1) {
			printf("ERROR %s is an unrecognized command.\n", command);
			printf("Options are: \n\tPASSKEY\n\tENCRYPT\n\tDECRYPT\n\tQUIT\n\n");
			continue;
		}

		char *result = strtok(NULL, "");
		if (result == NULL) {
			printf("Missing argument.\n");
			continue;
		}

		if (check && strlen(passkey) == 0) {
			printf("ERROR Passkey not set.\n");
			continue;
		}

		int len1 = strlen(result);
		int len2 = strlen(passkey);

		if (checkAlpha(result, len1) == -1) {
			printf("ERROR %s is an invalid argument.\n", result);
			continue;
		}
		
		changeCase(result, strlen(result));
		switch (check) {
			case 0 : // SET PASSKEY
				strcpy(passkey, result);
				result = "";
				break;
			
			case 1 : // ENCRYPT
				for (int i = 0; i < len1; ++i) {
					char diff = result[i] + (passkey[i%len2] - 'A');
					diff = 'A' + (diff - 'A')%26;
					result[i] = diff;
				}
				break;

			case 2 : // DECRYPT
				for (int i = 0; i < len1; ++i) {
					char diff = result[i] - (passkey[i%len2] - 'A');
					diff = 'A' + (diff + 'A')%26;
					result[i] = diff;
				}
				break;
		}

		printf("RESULT %s\n", result);
	}
	return 0;
}
