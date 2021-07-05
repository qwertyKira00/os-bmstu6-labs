//testKernelIO.c
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h> //System calls
#include <stdlib.h>
#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD   -12

typedef struct file_descr
{
  int fd;
} file_descr_t;

void *read_from_file(void *args)
{
  file_descr_t *file_descriptor = (file_descr_t*) args;
  char c;

  while(read(file_descriptor->fd, &c, 1) == 1)
    write(1, &c, 1);
  
  write(1, " ", 1);
}

int main()
{
  char c;
  int status, status_addr;
  // have kernel open two connection to file alphabet.txt
  int fd1 = open("alphabet.txt",O_RDONLY);
  int fd2 = open("alphabet.txt",O_RDONLY);

  pthread_t thread;
 
  // read a char & write it alternatingly from connections fs1 & fd2
  status = pthread_create(&thread, NULL, read_from_file, &fd1);
  if (status != 0)
  { 
    printf("Error: couldn't create a thread. Status = %d\n", status);
    exit(ERROR_CREATE_THREAD);
  }

  read_from_file(&fd2);
   
  status = pthread_join(thread, (void**)&status_addr);
  if (status != 0) 
  {
        printf("Error: couldn't join thread. Status = %d\n", status);
        exit(ERROR_JOIN_THREAD);
    }
  
  return 0;
}