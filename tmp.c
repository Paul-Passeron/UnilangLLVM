#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>



int main(void){
  dev_t d;
  struct stat s;
  
  printf("Sizeof stat: %ld\n", 8 * sizeof( struct stat));
  return 0;
}