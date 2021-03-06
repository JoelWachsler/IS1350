/*
 * =====================================================================================
 *
 *       Filename:  freq_doubles.c
 *
 *    Description:  Samma som freq.c men räknar med dubletter
 *
 *        Version:  1.0
 *        Created:  2017-03-03 18:38:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joel Wachsler
 *   Organization:  KTH
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

typedef struct {
  unsigned long files;
  unsigned long sym_links;
  unsigned long folders;
} Counter;

#define FREQ_MAX 32

unsigned long freq[FREQ_MAX];

void add_to_freq(unsigned long size, unsigned long inode, Counter *counter) {
  if (size != 0) {
    int j = 0;
    int n = 2;
    while (size / n != 0 & j < FREQ_MAX) {
      n = 2*n;
      j++;
    }

    freq[j]++;
    counter->files++;
  }
}

void count(char *path, Counter *counter) {
  DIR *dirp = opendir(path);

  // Check if we have access to the directory
  if (dirp == NULL) {
    printf("not able to open %s\n", path);
    return;
  }

  char subdir[2048];
  struct dirent *entry;
  struct stat file_st;

  while ((entry = readdir(dirp)) != NULL) {
    switch (entry->d_type) {
      case DT_REG:       // This is a regular file.
        if (fstatat(dirfd(dirp), entry->d_name, &file_st, 0) == 0)
          add_to_freq(file_st.st_size, file_st.st_ino, counter);
        break;
      case DT_LNK:       // This is a symbolic link.
        counter->sym_links++;
        break;
      case DT_DIR:       // This is a directory.
        if ((strcmp(entry->d_name, ".") == 0) | (strcmp(entry->d_name, "..") == 0))
          break;
        counter->folders++;
        sprintf(subdir, "%s/%s", path, entry->d_name);
        count(subdir, counter);
        break;
      default:
        break;
    }
  }

  closedir(dirp);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("usage: myls <dir>\n");
    return -1;
  }

  char *path = argv[1];
  Counter counter = {0,0,0};
  count(path, &counter);

  printf("Files:\t\t%lu\n", counter.files);
  printf("Symbolic links:\t%lu\n", counter.sym_links);
  printf("Folders:\t%lu\n", counter.folders);

  printf("The directory %s contains %lu files\n", path, counter.files + counter.sym_links + counter.folders);
  printf("#The directory %s: number of files smaller than 2^k:\n", path);
  printf("#k\tnumber\n");

  for(int j = 0; j < FREQ_MAX; j++)
    printf("%d\t%lu\n", (j+1), freq[j]);

  return 0;
}

