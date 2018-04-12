/*
 compile: gcc *.c -o unw -lunwind -lunwind-x86_64
 */
#include <stdio.h>

extern void hotspots_analysis(int t_us);
extern void print_count();

void a()
{
   int i;
   for (i = 0; i < 60000000; i++);
}

void b()
{
   int i;
   for (i = 0; i < 20000000; i++);
}

void c()
{
   int i;
   for (i = 0; i < 80000000; i++);
}

int main (int argc, char **argv)
{
	int i;
	hotspots_analysis(1000);

	for (i = 0; i < 10;i++)
	{
		a();
		b();
		c();
	}
	for (i = 0; i < 240000000; i++);

	print_count();
}