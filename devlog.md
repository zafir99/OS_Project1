14-October-2025 3:47PM
For this brief session, I plan on completing the encryption program since the Vigenère cipher doesn't seem 
particularly difficult to implement. 
I will do the driver and logger programs maybe later today but probably over the next few days.

18-October-2025 2:13PM
I did quite a few things without properly reflecting so in this devlog, so I will do so now.
Firstly, my initial version of the Vigenère cipher was a version that worked from the command line—it did not
go through standard input, so I made a new branch that did go through standard input. I also ended up learning
about how fgets processes up until either the character limit is reached OR a newline character is encountered.
Because of that I learned about the strcspn standard library function for efficient processing of the input strings.

Secondly, I did a little experimenting with fork() and pipe(); I had no idea that the forked process kept a file
descriptor table with the same entries as the parent process. I ended up learning how both functions actually work.

For this session, I plan on completing the logger program and driver program. I will split it up into a function
that prints the menu, a while loop that forks into a child process for encryption/decryption, and I think I'll have
a child process outside the loop that consists of the logger. I'm not entirely sure if that's how it'll all work 
properly, but that is the current outline. Ideally, I will at least have a rough draft of the entire application by
the end of this session.

18-October-2025 7:00PM
I was able to finish my logger; I came across troubles with the mixing of scanf and fgets. I had to use getchar
to get rid of the extra newline character that remains in the buffer from scanf, but after all that, the logger
appears to working correctly.
Unfortunately, I just realized that even my current stdin version of the encryption program is still incorrect. I
just checked the project doc and realized that it's supposed to be a one line string input separated by a space, so
now, I will rewrite it to use a single buffer and strtok.

19-October-2025 12:32AM
I finished the revised cypher. It works properly with decent error handling. Now all that is left is the driver
program. I plan on working on the error handling first, and then moving on to the critical logic.

19-October-2025 1:49AM
I quickly switched back to my logging development branch because I realized the logger was supposed to accept
the filename as a command line argument and not through stdin. It was fairly trivial to implement, and I just
merged back the results with my main branch.
