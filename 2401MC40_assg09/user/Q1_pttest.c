#include "types.h"
#include "user.h"

int
main(void)
{
  int i;
  char *mem;

  printf(1, "Page tables before sbrk:\n");
  vmprint();

  mem = sbrk(4 * 4096);
  if(mem == (char*)-1){
    printf(1, "pttest: sbrk failed\n");
    exit();
  }

  // Touch each lazy page so the post-sbrk page table contains new mappings.
  for(i = 0; i < 4; i++)
    mem[i * 4096] = 0;

  printf(1, "Page tables after sbrk and first access to each page:\n");
  vmprint();
  exit();
}
