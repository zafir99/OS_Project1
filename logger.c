#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define MAXLEN 100
#define time_char 17

static inline void formatString(char *buf) {
	fgets(buf, MAXLEN, stdin);
	buf[strcspn(buf, "\n")] = '\0';
}

int main(void) {
	char filename[MAXLEN];
	char *action[5] = {"START", "PASS", "ENCRYPT", "DECRYPT", "QUIT"};
	char timebuf[time_char];
	char buf[MAXLEN];

	fgets(filename, MAXLEN, stdin);
	filename[strcspn(filename, "\n")] = '\0'; // trim off newline character
	int fd = open(filename, O_CREAT|O_RDWR, S_IRWXU);
	if (fd == -1) {
		printf("Error opening file %s.\n", filename);
		return -1;
	}
	off_t start = lseek(fd, 0, SEEK_END);	// get current logging location
											// so as to not include any info from previous logging sessions
	time_t now = time(NULL);
	strftime(timebuf, time_char, "%Y-%m-%d %H:%M", localtime(&now));
	dprintf(fd, "%s [%s] Logging Started.\n\n", timebuf, action[0]);

	int action_type;
	scanf("%d", &action_type);
	while (action_type != -1) {
		getchar(); // consume newline character from scanf
		now = time(NULL);
		strftime(timebuf, time_char, "%Y-%m-%d %H:%M", localtime(&now));
		formatString(buf);
		switch (action_type) {
			case 1 :
				dprintf(fd, "%s [%s] Passkey set to %s.\n\n", timebuf, action[action_type], buf);
				break;
			
			case 2 :
				dprintf(fd, "%s [%s] String \"%s\" encrypted to ", timebuf, action[action_type], buf);
				formatString(buf);
				dprintf(fd, "\"%s\".\n\n", buf);
				break;

			case 3 :
				dprintf(fd, "%s [%s] String \"%s\" decrypted to ", timebuf, action[action_type], buf);
				formatString(buf);
				dprintf(fd, "\"%s\".\n\n", buf);
				break;

			default :
				dprintf(fd, "%s [Unknown Command] Invalid command entered.\n\n", timebuf);
				break;
		}
		scanf("%d", &action_type);
	}
	
	now = time(NULL);
	strftime(timebuf, time_char, "%Y-%m-%d %H:%M", localtime(&now));
	dprintf(fd, "%s [%s] Logging Ended.\n\n", timebuf, action[4]);
	close(fd);
	return 0;
}
