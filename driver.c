#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#define temp_file "temphist.txt"
#define MAXLEN 100
#define FILE_BYTES 1024
#define ENCRYPTLEN 1024

#define NUMCOMMANDS 5
char *commands[] = {"PASSWORD", "ENCRYPT", "DECRYPT", "HISTORY", "QUIT"};

inline static int checkCommand(char *str) {
	for (int i = 0; i < NUMCOMMANDS; ++i) {
		if (!strcmp(str, commands[i])) {
			return i;
		}
	}
	return -2;
}

inline static int checkNum(char *str, int len) {
	int i = 0;
	if (str[i] == '-') ++i;
	for (; i < len; ++i) {
		if (!isdigit(str[i])) {
			return 0;
		}
	}
	return 1;
}

inline static char checkAlpha(char *str, int len) {
	for (int i = 0; i < len; ++i) {
		if (!isalpha(str[i])) {
			return 0;
		}
	}
	return 1;
}

inline static void changeCase(char *str, int len) {
	for (int i = 0; i < len; ++i) {
		str[i] = toupper(str[i]);
	}
}

const char menu[] = "\nMenu:\n"
			 "-------------------------------------------\n"
			 "password - select a password for encryption\n"
			 "encrypt - select a word to encrypt\n"
			 "decrypt -  select a word to decrypt\n"
			 "history - show history up until now\n"
			 "quit - end program\n\n"
			 "type your selection: ";

inline static void printMenu(void) {
	write(STDOUT_FILENO, menu, sizeof(menu));
}

const char options[] = "\n1. use a string from history\n"
					"2. type a new string\n\n"
					"type your selection: ";

inline static void printOptions(void) {
	write(STDOUT_FILENO, options, sizeof(options));
}

const char errormsg[] = "\nselection must be a single word and cannot contain non-alphabetical characters.\n"
						"please try again.\n\n";

inline static void printError(void) {
	write(STDOUT_FILENO, errormsg, sizeof(errormsg));
}

const char prompt[] = "\nenter your word: ";

inline static void printPrompt(void) {
	write(STDOUT_FILENO, prompt, sizeof(prompt));
}

const char optionErrorMsg[] = "\nselection must be 1 or 2.\nplease try again.\n\n";

inline static void printOptionError(void) {
	write(STDOUT_FILENO, optionErrorMsg, sizeof(optionErrorMsg));
}

const char historyErrorMsg[] = "\nselection must be a valid integer in the history.\n"
							"please try again.\n\n";

inline static void printHistoryError(void) {
	write(STDOUT_FILENO, historyErrorMsg, sizeof(historyErrorMsg));
}

const char invalidMsg[] = " is an unrecognized command.\n"
						"please try again.\n\n";

inline static void printInvalidOption(char *str, int len) {
	write(STDOUT_FILENO, "\n", 1);
	write(STDOUT_FILENO, str, len);
	write(STDOUT_FILENO, invalidMsg, sizeof(invalidMsg));
}

const char missingPass[] = "\nthere is no passkey in the system.\n"
							"please enter a passkey first.\n\n";

inline static void printMissingPass(void) {
	write(STDOUT_FILENO, missingPass, sizeof(missingPass));
}

inline static void flushInput() {
	char c;
	while (read(STDIN_FILENO, &c, 1) && c != '\n');
}

inline static void readString(char *str, int len) {
	int i = 0;
	char c;
	while (read(STDIN_FILENO, &c, 1) && i < len-1 && c != '\n') {
		str[i] = c;
		++i;
	}
	str[i] = '\0';
	if (c != '\n') {
		flushInput();
	}
}

const char exitHistory[] = "\ntype -1 to exit history\n\n"
							"select any number in history: ";

inline static void printInt(int num) {
	char digits[11] = "0123456789";
	char numstr[12];
	char *c = numstr;
	if (num < 0) {
		*c++ = '-';
		num *= -1;
	}
	int shift = num;
	do {
		++c;
		shift /= 10;
	} while (shift);

	*c = '\0';
	while (num) {
		*--c = digits[num%10];
		num /= 10;
	}
	write(STDOUT_FILENO, numstr, 12);
}

int converttoInt(char *numstr, int len) {
	int num = 0;
	char i = 0;
	char sign = 1;
	if (numstr[i] == '-') {
		sign = -1;
		++i;
	}
	while (numstr[i] == '0' && i < len) { // get rid of initial zeros
		++i;
	}
	
	while(i < len) {
		num = num*10 + (numstr[i++] - '0');
	}
	return num * sign;
}

const char header[] = "\nHISTORY SO FAR\n-------------------------\n\n";

inline static void printHistory(int fd) {
	write(STDOUT_FILENO, header, sizeof(header));
	char filebuf[FILE_BYTES];
	off_t loc = lseek(fd, 0, SEEK_CUR); // get EOF location
	lseek(fd, 0, SEEK_SET); // set to beginning of file
	int num;
	char c;
	while (read(fd, &num, sizeof(int))) {
		printInt(num);
		do {
			read(fd, &c, 1);
			write(STDOUT_FILENO, &c, 1);
		} while (c != '\n');
		write(STDOUT_FILENO, "\n", 1);
	}
	write(STDOUT_FILENO, "\n", 1);
	lseek(fd, loc, SEEK_SET); // reset to EOF
}

inline static void printExitHistory(void) {
	write(STDOUT_FILENO, exitHistory, sizeof(exitHistory));
}

inline static void flushStream (int fd) {
	char c;
	while (read(fd, &c, 1) && c != '\n');
}

char word[MAXLEN+1];
char* getOption (int fd, int num) {
	memset(word, 0, MAXLEN+1);
	off_t loc = lseek(fd, 0, SEEK_CUR); // get EOF location
	lseek(fd, 0, SEEK_SET); // beginning of file
	char c;
	int i = 0;
	while (read(fd, &i, sizeof(int)) && i != num) {
		flushStream(fd);
	}
	if (i != num) {
		return NULL;
	}

	lseek(fd, 2, SEEK_CUR); // move past space and .
	i = 0;
	while (read(fd, &c, 1) && c != '\n') {
		word[i++] = c;
	}
	word[i] = '\0';
	lseek(fd, loc, SEEK_SET); // reset to EOF
	return word;
}

const char empty[] = "\nhistory is empty!\n\n";

inline static void printEmpty(void) {
	write(STDOUT_FILENO, empty, sizeof(empty));
}

static inline void writetobuf(char *buf, char *str, int offset, int len) {
	for (int i = 0; i < len; ++i) {
		buf[offset+i] = str[i];
	}
}

int main(int argc, char **argv) {
	if (argc == 1) {
		printf("usage: ./driver <file_name>\n");
		return 1;
	}

	int histfd = open(temp_file, O_CREAT|O_RDWR, S_IRWXU); // could use O_TMPFILE but it doesnt work on all
	if (histfd == -1) {
		printf("Error starting program.\n");
		return -1;
	}

	char num[MAXLEN];

	int logfd[2];
	if (pipe(logfd) == -1) {
		printf("Error opening pipe for logger.\n");
		return 2;
	}
	pid_t logger = fork();
	if (logger == -1) {
		printf("Error forking logger.\n");
		return -2;
	}
	if (!logger) { // successful fork
		if (dup2(logfd[0], STDIN_FILENO) == -1) {
			printf("Error duplicating read end of logger pipe.\n");
			return 3;
		}
		close(logfd[0]); close(logfd[1]); // file descriptors no longer needed in child
		char *args[] = {"./logger", argv[1], NULL};
		if (execvp(args[0], args) == -1) {
			printf("Error starting logger.\n");
			return -3;
		}
	}

	int encryptfd[2][2];
	if (pipe(encryptfd[0]) == -1 || pipe(encryptfd[1]) == -1) {
		printf("Error opening pipes for encryption program.\n");
		return 5;
	}
	pid_t encrypt = fork();
	if (encrypt == -1) {
		printf("Error forking encryption program.\n");
		return -4;
	}
	if (!encrypt) { // successful fork
		if (dup2(encryptfd[0][0], STDIN_FILENO) == -1) {
			printf("Error duplicating read end of encryption pipe.\n");
			return 6;
		}
		if (dup2(encryptfd[1][1], STDOUT_FILENO) == -1) {
			printf("Error duplicating write end of encryption pipe.\n");
			return 7;
		}
		close(encryptfd[0][0]); close(encryptfd[0][1]); // file descriptors no longers needed in child
		close(encryptfd[1][0]); close(encryptfd[1][1]);
		char *argsE[] = {"./encryption", NULL};
		if (execvp(argsE[0], argsE) == -1) {
			printf("Error starting encryption program.\n");
			return -5;
		}
	}

	/* close unused ends*/
	close(encryptfd[0][0]);
	close(encryptfd[1][1]);
	close(logfd[0]);

	char historyOption = 0;
	int commandSelection = -2;
	char passkey = 0;
	char buffer[MAXLEN];
	char encryptbuf[ENCRYPTLEN];
	int numWords = 0;

	while (1) {
		printMenu();
		//memset(buffer, 0, MAXLEN) this would be optional but unneeded given how i structured the readString funct
		readString(buffer, MAXLEN);
		memset(encryptbuf, 0, ENCRYPTLEN);
		int encryptoffset = 0;

		int len = strlen(buffer);
		changeCase(buffer, len);
		commandSelection = checkCommand(buffer);

		if (commandSelection == 4) {
			break;
		}

		if (commandSelection == -2) {
			write(logfd[1], &commandSelection, sizeof(int));
			printInvalidOption(buffer, len);
			continue;
		}

		int logNum = -2;
		char *wordSelection;

		if (commandSelection == 3) {
			logNum = 4;
			write(logfd[1], &logNum, sizeof(int));

			if (!numWords) {
				printEmpty();
			}
			else {
				printHistory(histfd);
			}
			continue;
		}

		if (commandSelection && !passkey) { // passkey has yet to be set and user did not type password
			logNum = (commandSelection == 1 ? 6 : 7);
			printMissingPass();
			write(logfd[1], &logNum, sizeof(int));
			continue;
		}

		printOptions();
		read(STDIN_FILENO, &historyOption, 1);
		flushInput();

		while (historyOption != '1' && historyOption != '2') {
			printOptionError();
			logNum = -2;
			write(logfd[1], &logNum, sizeof(int));

		hist:
			printOptions();
			read(STDIN_FILENO, &historyOption, 1);
			flushInput();
		}

		if (historyOption == '1' && !numWords) {
			logNum = 4;
			write(logfd[1], &logNum, sizeof(int));
			printEmpty();
			continue;
		}

		else if (historyOption == '1' && numWords) {
			logNum = 4;
			write(logfd[1], &logNum, sizeof(int));
			printHistory(histfd);
			int intSelect = -2;

			while (intSelect == -2) {
				printExitHistory();
				int numlen = 11;
				char numSelect[numlen];
				readString(numSelect, numlen);
				numlen = strnlen(numSelect, numlen);

				if (!checkNum(numSelect, numlen)) {
					printHistoryError();
					intSelect = -2;
					write(logfd[1], &intSelect, sizeof(int));
					continue;
				}

				intSelect = converttoInt(numSelect, numlen);

				if (intSelect == -1) {
					logNum = 5;
					write(logfd[1], &logNum, sizeof(int));
					goto hist;
				}

				if (intSelect < 1 || intSelect > numWords) {
					printHistoryError();
					write(logfd[1], &intSelect, sizeof(int));
					intSelect = -2;
					continue;
				}

				wordSelection = getOption(histfd, intSelect);
				if (wordSelection == NULL) {
					printHistoryError();
					intSelect = -2;
					write(logfd[1], &intSelect, sizeof(int));
					continue;
				}
				len = strlen(wordSelection);
			}
		}

		else if (historyOption == '2') {
			printPrompt();
			readString(buffer, MAXLEN);
			len = strlen(buffer);
			logNum = -2;
			while (!checkAlpha(buffer, len)) {
				printError();
				printPrompt();
				write(logfd[1], &logNum, sizeof(int));
				readString(buffer, MAXLEN);
				len = strlen(buffer);
			}
			wordSelection = buffer;
		}

		switch (commandSelection) {
			case 0 :
				passkey = 1;
				logNum = 1;
				write(logfd[1], &logNum, sizeof(int));
				write(logfd[1], wordSelection, len+1); // include '\0'

				writetobuf(encryptbuf, "PASSKEY ", encryptoffset, 8);
				encryptoffset += 8;
				writetobuf(encryptbuf, wordSelection, encryptoffset, len);
				encryptoffset += len;
				write(encryptfd[0][1], encryptbuf, encryptoffset+1); // include '\0'
				read(encryptfd[1][0], encryptbuf, ENCRYPTLEN); // clear pipe
				break;

			case 1 :
				++numWords;
				logNum = 2;
				write(logfd[1], &logNum, sizeof(int));
				write(logfd[1], wordSelection, len);

				writetobuf(encryptbuf, "ENCRYPT ", encryptoffset, 8);
				encryptoffset += 8;
				writetobuf(encryptbuf, wordSelection, encryptoffset, len);
				encryptoffset += len;
				write(encryptfd[0][1], encryptbuf, encryptoffset+1); // include null terminator

				memset(encryptbuf, 0, ENCRYPTLEN);
				read(encryptfd[1][0], encryptbuf, ENCRYPTLEN);
				char *encryptedWord = encryptbuf+8; // "RESULT \0" length is 8 bytes
				int encryptedlen = strlen(encryptedWord); // includes \n

				write(STDOUT_FILENO, "\nencrypted word: ", 17);
				write(STDOUT_FILENO, encryptedWord, encryptedlen);

				write(histfd, &numWords, sizeof(int));
				write(histfd, ". ", 2);
				write(histfd, encryptedWord, encryptedlen);

				write(logfd[1], encryptedWord, encryptedlen);
				break;

			case 2 :
				++numWords;

				logNum = 3;
				write(logfd[1], &logNum, sizeof(int));
				write(logfd[1], wordSelection, len);

				writetobuf(encryptbuf, "DECRYPT ", encryptoffset, 8);
				encryptoffset += 8;
				writetobuf(encryptbuf, wordSelection, encryptoffset, len);
				encryptoffset += len;
				write(encryptfd[0][1], encryptbuf, encryptoffset+1);

				memset(encryptbuf, 0, MAXLEN);
				read(encryptfd[1][0], encryptbuf, ENCRYPTLEN);
				char *decryptedWord = encryptbuf + 8; // "RESULT \0" length is 8 bytes
				int decryptedlen = strlen(decryptedWord); // includes \n

				write(STDOUT_FILENO, "\ndecrypted word: ", 17);
				write(STDOUT_FILENO, decryptedWord, decryptedlen);

				write(histfd, &numWords, sizeof(int));
				write(histfd, ". ", 2);
				write(histfd, decryptedWord, decryptedlen);

				write(logfd[1], decryptedWord, decryptedlen);
				break;
		}
	}

	write(encryptfd[0][1], commands[commandSelection], 5); // send quit to encrypt
	commandSelection = -1;
	write(logfd[1], &commandSelection, sizeof(int)); // send quit to logger
	close(encryptfd[0][1]); close(encryptfd[1][0]);
	close(logfd[1]);
	return unlink(temp_file); // delete temp history file using linux syscall
}
