//testKernelIO.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
   char c;
  // have kernel open two connection to file alphabet.txt
  int fd1;
  int fd2 = open("alphabet.txt",O_RDONLY);
  // read a char & write it alternatingly from connections fs1 & fd2
  int flag = 1;
  int pid;
  
  if ((pid = fork()) == -1)
  {
    perror("fork Error\n");
    return 0;
  }
    
  if (pid == 0)
      fd1 = open("alphabet.txt",O_RDONLY);

  while(flag)
  {
    if (pid == 0) //child process
    {
      if (read(fd1,&c,1) == 1)  //-1 if fails to read
        write(1,&c,1);
      else 
        flag = 0;
      printf("Child process = %d\n", getpid());
    }

    else
    {
      if (read(fd2,&c,1) == 1)
        write(1,&c,1);
      else
        flag = 0;
      printf("Parent process = %d\n", getpid());
    }
  }
  
  return 0;
}
/* переписать код без использования break!*/
