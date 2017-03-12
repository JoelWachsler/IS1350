/*
 * =====================================================================================
 *
 *       Filename:  total.c
 *
 *    Description:  Counts the total number of files in the directory.
 *
 *        Version:  1.0
 *        Created:  2017-02-28 10:40:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joel Wachsler
 *   Organization:  KTH
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

unsigned long count(char *path) {
  unsigned long total = 0;
  DIR *dirp = opendir(path);

  // Check if we have access to the directory
  if (dirp == NULL) {
    printf("not able to open %s\n", path);
    return 0;
  }

  char subdir[1024];
  struct dirent *entry;
  while ((entry = readdir(dirp)) != NULL) {
    switch (entry->d_type) {
      case DT_DIR:       // This is a directory.
        if ((strcmp(entry->d_name, ".") == 0) | (strcmp(entry->d_name, "..") == 0)) {
          break;
        }
        sprintf(subdir, "%s/%s", path, entry->d_name);
        total += count(subdir);
        break;
      case DT_REG:       // This is a regular file.
        total++;
        break;
      default:
        break;
    }
  }
  closedir(dirp);
  return total;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    perror("usage: total <dir>");
    return -1;
  }

  char *path = argv[1];
  unsigned long total = count(path);
  printf("The directory %s contains %lu files\n", path, total);

  return 0;
}
