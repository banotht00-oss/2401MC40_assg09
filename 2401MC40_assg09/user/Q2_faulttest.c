#include "types.h"
#include "user.h"

int
main(void)
{
  int i, pid, fds[2];
  char *mem;
  char signal;

  mem = sbrk(32 * 4096);
  if(mem == (char*)-1){
    printf(1, "faulttest: sbrk failed\n");
    exit();
  }

  for(i = 0; i < 5; i++)
    mem[i * 4096] = i;

  printf(1, "lazy page faults: %d (expected 5)\n", getfaults());

  if(pipe(fds) < 0){
    printf(1, "faulttest: pipe failed\n");
    exit();
  }
  pid = fork();
  if(pid < 0){
    printf(1, "faulttest: fork failed\n");
    exit();
  }
  if(pid == 0){
    close(fds[0]);
    mem[32 * 4096] = 1;
    write(fds[1], "x", 1);
    exit();
  }

  close(fds[1]);
  if(read(fds[0], &signal, 1) == 0)
    printf(1, "out-of-bounds access killed the child (expected)\n");
  else
    printf(1, "faulttest: out-of-bounds access was not rejected\n");
  close(fds[0]);
  wait();
  exit();
}
