/*
 * =====================================================================================
 *
 *       Filename:  total.c
 *
 *    Description:  Skriv ett program som läser innehållet i en mapp rekursivt och räknar
 *                  antalet filer, mjuka länkar och mappar.
 *
 *        Version:  1.0
 *        Created:  2017-03-03 17:26:31
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
  unsigned int files;
  unsigned int sym_links;
  unsigned int folders;
} Counter;

unsigned long count(char *path, Counter *counter) {
  DIR *dirp = opendir(path);

  // Check if we have access to the directory
  if (dirp == NULL) {
    printf("not able to open %s\n", path);
    return 0;
  }

  struct dirent *entry;
  char subdir[1024];
  while ((entry = readdir(dirp)) != NULL) {
    switch (entry->d_type) {
      case DT_REG:       // This is a regular file.
        counter->files++;
        break;
      case DT_LNK:       // This is a symbolic link.
        counter->sym_links;
        break;
      case DT_DIR:       // This is a directory.
        if ((strcmp(entry->d_name, ".") == 0) | (strcmp(entry->d_name, "..") == 0)) {
          break;
        }
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

  printf("Files:\t\t%d\n", counter.files);
  printf("Symbolic links:\t%d\n", counter.sym_links);
  printf("Folders:\t%d\n", counter.folders);

  return 0;
}
