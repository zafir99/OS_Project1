#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define MAXLEN 100
#define time_char 17
#define NUMACTIONS 6

static inline void formatString(char *buf) {
	fgets(buf, MAXLEN, stdin);
	buf[strcspn(buf, "\n")] = '\0';
}

int main(int argc, char **argv) {
	char *action[NUMACTIONS] = {"START", "PASS", "ENCRYPT", "DECRYPT", "HISTORY", "QUIT"};
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
	scanf("%d", &action_type);
	while (action_type != -1) {
		getchar(); // consume newline character from scanf
		now = time(NULL);
		strftime(timebuf, time_char, "%Y-%m-%d %H:%M", localtime(&now));
		switch (action_type) {
			case 1 :
				dprintf(fd, "%s [%s] Passkey set by user.\n\n", timebuf, action[action_type]);
				break;
			
			case 2 :
				formatString(buf);
				dprintf(fd, "%s [%s] String encrypted to ", timebuf, action[action_type]);
				dprintf(fd, "\"%s\".\n\n", buf);
				break;

			case 3 :
				formatString(buf);
				dprintf(fd, "%s [%s] String decrypted to ", timebuf, action[action_type]);
				dprintf(fd, "\"%s\".\n\n", buf);
				break;

			case 4 :
				dprintf(fd, "%s [%s] History shown to user.\n\n", timebuf, action[action_type]);
				break;

			default :
				dprintf(fd, "%s [Unknown Command] Invalid command entered.\n\n", timebuf);
				break;
		}
		scanf("%d", &action_type);
	}
	
	now = time(NULL);
	strftime(timebuf, time_char, "%Y-%m-%d %H:%M", localtime(&now));
	dprintf(fd, "%s [%s] Logging Ended.\n\n", timebuf, action[5]);
	close(fd);
	return 0;
}
