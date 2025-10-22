IMPORTANT REMARK:

You will NOT be able to independently test the logger program. It is designed to read ***RAW INTEGER DATA*** from
standard input by using the read syscall. If you want to test its functionality independent of the driver program, you will have to code a simple program that pipes integers to the logger.


Instructions For Running
---------------------------

1. Compile all programs with gcc -o

gcc -o encryption encryption.c
gcc -o logger logger.c
gcc -o driver driver.c


2. Run driver with arbitrary filename

./driver <filename>

3. Done!
