#include <dirent.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ansi-colour.h"
#include "file.h"

/*
ls - print all files and directories in working directory
Available arguments:
        --help: show this help message
        --version: show the version of the program (WIP)
*/

/*static int printBool(bool cond) {
        printf("%s\n", cond ? "true" : "false");
        return 0;
}*/

static bool startsWithChar(const char *pre, const char str) {
  char *e;
  int index;
  e = strchr(pre, str);
  index = (int)(e - pre);
  return index == 0;
}

static int cmp(const void *a, const void *b) {
  return strcmp(*(char **)a, *(char **)b);
}

// lots o' code ~~stolen~~ borrowed from
// https://pubs.opengroup.org/onlinepubs/9699919799/functions/readdir.html
int main(int argc, char **argv) {
  // define default options
  bool colour = false;
  bool showdot = false;
  bool specpath = false;
  int maxLen = 180;
  char *thatpath;
  for (int i = 1; i < argc; i++) {
    char *arg = argv[i];
    /*printf(arg);
    printf(" ");*/
    if (!strcmp(arg, "--help")) {
      char *help =
          "Drake's Epic Coreutils (working title) " DRAKECU_VERSION "\n"
          "ls - print all files and directories in working directory\n"
          "Available arguments:\n"
          "	--help:        show this help message\n"
          "	--version:     show the version of the program\n"
          "	--color:       colour the output depending on whether there is "
          "a file or folder\n"
          "	--colour:      same as --color, but for our Bri'ish folks\n"
          "	-C, --columns: print every entry on a seperate line";
      printf("%s\n", help);
      return 0;
    } else if (!strcmp(arg, "--version")) {
      printf("%s\n", DRAKECU_VERSION);
      return 0;
    } else if (!strcmp(arg, "--color") || !strcmp(arg, "--colour")) {
      colour = true;
    } else if (!strcmp(arg, "-a") || !strcmp(arg, "--all")) {
      showdot = true;
    } else if (!strcmp(arg, "-C") || !strcmp(arg, "--columns")) {
      maxLen = 0;
    } else {
      // TODO: interpret absolute *and* relative paths. can't be that hard,
      // right?
      specpath = true;
      thatpath = arg;
    }
  }
  char wd[PATH_MAX];
  if (specpath == false) {
    getcwd(wd, sizeof(wd));
  } else if (specpath == true) {
    char *e = realpath(thatpath, NULL);
    strcpy(wd, e);
    free(e);
  }
  // printf(wd);
  DIR *dirp;
  struct dirent *dp;
  dirp = opendir(wd);
  if (dirp == NULL) {
    printf("couldn't open '%s'\n", wd);
    return 1;
  }
  int len = sizeof(char);
  char *out = malloc(len);
  out = NULL;
  dp = readdir(dirp);
  do {
    char *dirname = dp->d_name;
    if (!startsWithChar(dirname, '.') || showdot == true) {
      len += 1 + strlen(dirname) + strlen("§");
      out = realloc(out, len);
      strcat(out, dirname);
      strcat(out, "§");
    }
  } while ((dp = readdir(dirp)) != NULL);
  if (out == NULL || out == "") {
  	// printf(" \n");
  	// imo this shouldn't be commented out for consistency with the rest of the program, but gnu does it so so do we (english:tm:)
  	return 0;
  }
  free(dirp);
  char *word, *words[strlen(out) / 2 + 1];
  int i, n;
  i = 0;
  word = strtok(out, "§");
  while (word != NULL) {
    words[i++] = word;
    word = strtok(NULL, "§");
  }
  n = i;
  qsort(words, n, sizeof(*words), cmp);
  char oldwd[PATH_MAX];
  strcpy(oldwd, wd);
  int currLen = 0;
  for (i = 0; i < n; i++) {
    if (colour == false) {
      printf("%s  ", words[i]);
    } else {
      strcpy(wd, oldwd);
      strcat(wd, "/");
      strcat(wd, words[i]);
      // printf(wd);
      if (isSymlink(wd) == true) {
        printf(ANSI_GREEN);
        printf("%s  ", words[i]);
        printf(ANSI_RESET);
      } else if (isDirectory(wd) == true) {
        printf(ANSI_BLUE);
        printf("%s  ", words[i]);
        printf(ANSI_RESET);
      } else {
        printf("%s  ", words[i]);
      }
    }
    currLen += strlen(words[i]);
    if (currLen >= maxLen) {
      printf("\n");
      currLen = 0;
    }
  }
  free(out);
  printf("\n");
  return 0;
}
