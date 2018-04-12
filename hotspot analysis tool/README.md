 a tool based on libunwind for analysing hotspot function

usage:
add "hotspots_analysis()" before your program in main(), and add "print_count()" after your program in main(), then move the hotspot.c and the .c files which to be tested to the same folder, and compile them.
for example, we need to test main.c:
$ gcc *.c -o hotspottest -lunwind -lunwind-x86_64
