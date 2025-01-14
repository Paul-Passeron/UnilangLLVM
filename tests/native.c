#include <sys/syscall.h>
#include <unistd.h>

void put_character(char c) { write(1, &c, 1); }
void put_string(char *s) {
  size_t len = 0;
  while (s[len++]) {
  }
  write(1, s, len);
}
