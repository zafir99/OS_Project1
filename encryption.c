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

static inline void writetobuf(char *buf, char *str, int offset, int len) {
	for (int i = 0; i < len; ++i) {
		buf[offset+i] = str[i];
	}
}

int main(void) {
	char buf[MAXLEN+10]; // maximum string size 
	int msglen = 1024;
	char msgbuf[msglen];
	char passkey[MAXLEN];
	passkey[0] = '\0';
	char quit = 0;

	char c;
	while (1) {
		int offset = 0;
		memset(msgbuf, 0, msglen);
		read(STDIN_FILENO, buf, MAXLEN+10);
		buf[strcspn(buf, "\n")] = '\0';
		char *command = strtok(buf, " ");
		changeCase(command, strlen(command));

		char check = checkCommand(command);

		if (check == 3) {
			break;
		}

		if (check == -1) {
			int commandlen = strlen(command);
			writetobuf(msgbuf, "ERROR ", offset, 6);
			offset += 6;
			writetobuf(msgbuf, command, offset, commandlen);
			offset += commandlen;
			char ermsg[] = " is an unrecognized command.\n";
			writetobuf(msgbuf, ermsg, offset, sizeof(ermsg));
			offset += sizeof(ermsg);
			char optmsg[] = "Options are: \n\tPASSKEY\n\tENCRYPT\n\tDECRYPT\n\tQUIT\n\n";
			writetobuf(msgbuf, optmsg, offset, sizeof(optmsg));
			offset += sizeof(optmsg);
			write(STDOUT_FILENO, msgbuf, msglen);
			continue;
			//printf("ERROR %s is an unrecognized command.\n", command);
			//printf("Options are: \n\tPASSKEY\n\tENCRYPT\n\tDECRYPT\n\tQUIT\n\n");
		}

		char *result = strtok(NULL, "");
		if (result == NULL) {
			const char missmsg[] = "Missing argument.\n";
			write(STDOUT_FILENO, missmsg, sizeof(missmsg));
			//printf("Missing argument.\n");
			continue;
		}

		if (check && strlen(passkey) == 0) {
			const char passnotmsg[] = "ERROR Passkey not set.\n";
			write(STDOUT_FILENO, passnotmsg, sizeof(passnotmsg));
			//printf("ERROR Passkey not set.\n");
			continue;
		}

		int len1 = strlen(result);
		int len2 = strlen(passkey);

		if (checkAlpha(result, len1) == -1) {
			const char invArgmsg[] = " is an invalid argument.\n";
			write(STDOUT_FILENO, "ERROR ", 6);
			write(STDOUT_FILENO, result, len1);
			write(STDOUT_FILENO, invArgmsg, sizeof(invArgmsg));
			//printf("ERROR %s is an invalid argument.\n", result);
			continue;
		}
		
		changeCase(result, strlen(result));
		switch (check) {
			case 0 : // SET PASSKEY
				memset(passkey, 0, MAXLEN);
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

		char resmsg[] = "RESULT ";
		writetobuf(msgbuf, resmsg, offset, sizeof(resmsg));
		offset += sizeof(resmsg);
		writetobuf(msgbuf, result, offset, len1);
		offset += len1;
		writetobuf(msgbuf, "\n", offset, 2);
		write(STDOUT_FILENO, msgbuf, 1024);
		//printf("RESULT %s\n", result);
	}
	return 0;
}
