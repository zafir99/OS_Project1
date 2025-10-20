#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#define temp_file "temp_hist.txt"
#define MAXLEN 100
#define FILE_BYTES 1024

#define NUMCOMMANDS 5
char *commands[] = {"PASSWORD", "ENCRYPT", "DECRYPT", "HISTORY", "QUIT"};

inline static char checkCommand(char *str) {
	for (int i = 0; i < NUMCOMMANDS; ++i) {
		if (!strcmp(str, commands[i])) {
			return i;
		}
	}
	return -1;
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
			 "decrypt -  select a word to decrypt\n"
			 "history - show history up until now\n"
			 "quit - end program\n\n"
			 "type your selection: ";

inline static void printMenu(void) {
	write(STDOUT_FILENO, menu, sizeof(menu));
}

const char options[] = "1. use a string from history\n"
					"2. type a new string\n\n"
					"type your selection: ";

inline static void printOptions(void) {
	write(STDOUT_FILENO, options, sizeof(options));
}

const char errormsg[] = "selection must be a single word and cannot contain non-alphabetical characters.\n"
						"\tplease try again.\n\n";

inline static void printError(void) {
	write(STDOUT_FILENO, errormsg, sizeof(errormsg));
}

const char optionErrorMsg[] = "selection must be 1 or 2.\n\tplease try again.\n\n";

inline static void printOptionError(void) {
	write(STDOUT_FILENO, optionErrorMsg, sizeof(optionErrorMsg));
}

const char historyErrorMsg[] = "selection must be a valid integer in the history.\n"
							"\tplease try again.\n\n";

inline static void printHistoryError(void) {
	write(STDOUT_FILENO, historyErrorMsg, sizeof(historyErrorMsg));
}

const char invalidMsg[] = " is an unrecognized command.\n"
						"\tplease try again.\n\n";

inline static void printInvalidOption(char *str, int len) {
	write(STDOUT_FILENO, str, len);
	write(STDOUT_FILENO, invalidMsg, sizeof(invalidMsg));
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

char filebuf[FILE_BYTES];

inline static void printHistory(int fd) {
	int bytesRead;
	while (bytesRead = read(fd, filebuf, FILE_BYTES)) {
		write(STDOUT_FILENO, filebuf, bytesRead);
	}
	write(STDOUT_FILENO, "\n\n", 2);
}

#define HEADER "HISTORY SO FAR\n-------------------------\n\n"

int main(int argc, char **argv) {
	if (argc == 1) {
		printf("usage: ./driver <file_name>\n");
		return 1;
	}

	int histfd = open(temp_file, O_CREAT|O_APPEND|O_RDWR, S_IRWXU); // could use O_TMPFILE but it doesnt work on all
	if (histfd == -1) {
		printf("Error starting program.\n");
		return -1;
	}
	write(histfd, HEADER, sizeof(HEADER));

	pid_t logger = fork();
	if (logger == -1) {
		printf("Error forking logger.\n");
		return -2;
	}
	int logfd[2][2];
	if (pipe(logfd[0]) == -1 || pipe(logfd[1]) == -1) {
		printf("Error opening pipe for logger.\n");
		return 2;
	}
	if (!logger) { // successful fork
		if (dup2(logfd[0][0], STDIN_FILENO) == -1) {
			printf("Error duplicating read end of logger pipe.\n");
			return 3;
		}
		if (dup2(logfd[1][1], STDOUT_FILENO) == -1) {
			printf("Error duplicating write end of logger pipe.\n");
			return 4;
		}
		close(logfd[0][0]); close(logfd[0][1]); // file descriptors no longer needed in child
		close(logfd[1][0]); close(logfd[1][1]);
		char *args[] = {"./logger", argv[1], NULL};
		if (execvp(args[0], args) == -1) {
			printf("Error starting logger.\n");
			return -3;
		}
	}

	pid_t encrypt = fork();
	if (encrypt == -1) {
		printf("Error forking encryption program.\n");
		return -4;
	}
	int encryptfd[2][2];
	if (pipe(encryptfd[0]) == -1 || pipe(encryptfd[1]) == -1) {
		printf("Error opening pipe for encryption program.\n");
		return 5;
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

	char historyOption = 0;
	char commandSelection = 0;
	char buffer[MAXLEN];
	char filebuf[FILE_BYTES];
	while (commandSelection != 4) { // commands[4] == "QUIT"
		printMenu();
		readString(buffer, MAXLEN);
		if (!checkAlpha) { // input has non-alpha characters or > 1 word
			printError();
			continue;
		}

		int len = strlen(buffer);
		changeCase(buffer, len);
		commandSelection = checkCommand(buffer);
		if (commandSelection == -1) {
			printInvalidOption(buffer, len);
			continue;
		}

		write(logfd[1], &commandSelection, 1);
		write(logfd[1], "\n", 1);
		switch (commandSelection) {
			case 0 :
				while (historyOption != '1' || historyOption != '2') {
					printOptions();
					read(STDIN_FILENO, &historyOption, 1);
					printf("%c\n", historyOption);
					char flush = 0;
					read(STDIN_FILENO, &flush, 1);
					if (flush != '\n') {
						flushInput();
					}
					if (historyOption != '1' || historyOption != '2') {
						printOptionError();
					}
				}
				if (historyOption == '1') {
					printHistory(histfd);
					read(STDIN_FILENO, &historyOption, 1);
					
				}
				if (historyOption == 2) {

				}
				break;

			case 1 :

				break;

			case 2 :

				break;

			case 3 :
				printHistory(histfd);
				break;
		}
	}

	close(encryptfd[0]); close(encryptfd[1]);
	close(logfd[0]); close(logfd[1]);
	return unlink(temp_file); // delete temp history file using linux syscall
}
