#include <stdio.h>
#include <libunwind.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <stdint.h>
#include <malloc.h>
#define MAX_FUNC_NUM 100

int sample_time;
struct itimerval timer;
struct timeval time1, time2;
struct ftime{
    char fname[64];  //function name
    int  cnt;        //count of function is detecded 
};
struct ftime record[MAX_FUNC_NUM];
int func_cnt=0;

void record_count(char* name)
{
    int i = 0;
    for (i = 0;i < func_cnt; i++)
    {
        if(strcmp(name, record[i].fname) == 0)
        {
            record[i].cnt++;
            break;
        }
    }
    if (i == func_cnt)
    {
        strcpy(record[func_cnt].fname, name);
        record[func_cnt].cnt = 1;
        func_cnt ++;
    }
}

void bubbleSort(struct ftime a[], int n) 
{
    int i, j;
    for (i = 1; i< n; i++) 
    {
        for (j = 0; j < n - i; j++) 
        {
            if (a[j].cnt < a[j + 1].cnt)
            {
                struct ftime tmp = a[j]; 
                a[j] = a[j + 1]; 
                a[j + 1] = tmp;
            }
        }
    }
}

void print_count()
{
    int i;
    int total=0;
    float time;
    gettimeofday(&time2, NULL);
    time = 1000.0 * (time2.tv_sec - time1.tv_sec) + (time2.tv_usec - time1.tv_usec)/1000.0;
    printf("sample frequency: %d Hz\n", 1000000/sample_time);
    printf("running time：%.2f ms\n", time);
    bubbleSort(record, func_cnt);
    for (i=0;i<func_cnt;i++)
      total += record[i].cnt;
    printf("rtime(%%)\t atime(ms)\t name\n");
    for (i=0;i<func_cnt;i++)
      printf("  %.2f  \t  %.2f  \t  %s  \n", 1.0*record[i].cnt/total, time*record[i].cnt/total, record[i].fname);
}

void do_unwind_backtrace()
{
	  int i;
    unw_cursor_t  cursor;
    unw_context_t context;
    unw_word_t offset, pc;
    char fname[64];

    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    for (i = 0; i < 2; i++) //event_handler -> killpg -> test program
		  unw_step(&cursor); 
    if (unw_step(&cursor) > 0)
    {
      unw_get_reg(&cursor, UNW_REG_IP, &pc);
      fname[0] = '\0';
      (void) unw_get_proc_name(&cursor, fname, sizeof(fname), &offset);
      record_count(fname);
    }
}

void event_handler (int signum)
{
  	do_unwind_backtrace();
  	setitimer (ITIMER_REAL, &timer, NULL);
}

void hotspots_analysis(int t_us)
{
  struct sigaction sa;
  sample_time = t_us;
  gettimeofday(&time1, NULL);
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler = &event_handler;
  sigaction(SIGALRM, &sa, NULL);
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = t_us;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = t_us;
  setitimer (ITIMER_REAL, &timer, NULL);
}