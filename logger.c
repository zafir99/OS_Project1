#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define MAXLEN 100
#define time_char 17
#define NUMACTIONS 7

static inline void formatString(char *buf) {
	read(STDIN_FILENO, buf, MAXLEN);
	buf[strcspn(buf, "\n")] = '\0';
}

int main(int argc, char **argv) {
	if (argc == 1) {
		printf("usage: ./logger <file_name>\n");
		return 1;
	}

	char *action[NUMACTIONS] = {"START", "PASS", "ENCRYPT", "DECRYPT", "HISTORY", "EXIT HISTORY", "QUIT"};
	char timebuf[time_char];
	char buf[MAXLEN];

	int fd = open(argv[1], O_CREAT|O_RDWR, S_IRWXU);
	if (fd == -1) {
		printf("Error opening file %s.\n", argv[1]);
		return -1;
	}
	off_t start = lseek(fd, 0, SEEK_END);	// get current logging location
											// so as to not include or write on info from previous logging sessions
	time_t now = time(NULL);
	strftime(timebuf, time_char, "%Y-%m-%d %H:%M", localtime(&now));
	dprintf(fd, "%s [%s] Logging Started.\n\n", timebuf, action[0]);

	int action_type;
	while (1) {
		read(STDIN_FILENO, &action_type, sizeof(int));
	//	scanf("%d", &action_type);
		if (action_type == -1) {
			break;
		}
		now = time(NULL);
		strftime(timebuf, time_char, "%Y-%m-%d %H:%M", localtime(&now));
		switch (action_type) {
			case 1 :
				formatString(buf);
				dprintf(fd, "%s [%s] Passkey set by user as \"%s\".\n\n", timebuf, action[action_type], buf);
				break;
			
			case 2 :
				formatString(buf);
				dprintf(fd, "%s [%s] String \"%s\" encrypted to ", timebuf, action[action_type], buf);
				formatString(buf);
				dprintf(fd, "\"%s\".\n\n", buf);
				break;

			case 3 :
				formatString(buf);
				dprintf(fd, "%s [%s] String \"%s\" decrypted to ", timebuf, action[action_type], buf);
				formatString(buf);
				dprintf(fd, "\"%s\".\n\n", buf);
				break;

			case 4 :
				dprintf(fd, "%s [%s] History shown to user.\n\n", timebuf, action[action_type]);
				break;

			case 5 :
				dprintf(fd, "%s [%s] User exited history.\n\n", timebuf, action[action_type]);
				break;

			case 6 :
				dprintf(fd, "%s [%s] User attempted encryption with no passkey.\n\n", timebuf, action[2]);
				break;

			case 7 :
				dprintf(fd, "%s [%s] User attempted decryption with no passkey.\n\n", timebuf, action[3]);
				break;

			default :
				dprintf(fd, "%s [UNKNOWN] Invalid command entered.\n\n", timebuf);
				break;
		}
	}
	
	now = time(NULL);
	strftime(timebuf, time_char, "%Y-%m-%d %H:%M", localtime(&now));
	dprintf(fd, "%s [%s] Logging Ended.\n\n", timebuf, action[6]);
	close(fd);
	return 0;
}
