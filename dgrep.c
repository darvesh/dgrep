#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "usage: ./%s <search_term> <file_name>\n", argv[0]);
    exit(1);
  }
  size_t search_len = strlen(argv[1]);
  int fd = open(argv[2], O_RDONLY);
  struct stat st;
  fstat(fd, &st);
  void *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);
  if (map == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
  char *data = (char *)map;
  off_t line_start = 0;
  for (off_t i = 0; i < st.st_size; i++) {
    if (i + search_len <= st.st_size) {
      if (memcmp(data + i, argv[1], search_len) == 0) {
        off_t line_end = i;
        while (line_end < st.st_size && data[line_end] != '\n') {
          ++line_end;
        }
        fwrite(data + line_start, 1, line_end - line_start, stdout);
        putchar('\n');
      }
    }
    if (data[i] == '\n') {
      line_start = i + 1;
    }
  }
}