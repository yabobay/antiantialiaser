#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define FILENAME_INDICATION " (copy)."

bool fileExists(const char* filename) {
  return access(filename, F_OK) == 0;
}

// includes the character
char *chopUntilChar(const char *str, char c) {
  char *chr = strrchr(str, c);
  if (!chr)
    return strdup(str);
  return strdup(chr + 1);
}

char *filenameCoda(const char *filename) {
  return chopUntilChar(filename, '.');
}

char *filenameOnset(const char *filename) {
  /* slightly wasteful but i literally don't care. feel free to fix it
     if you want to */
  char *coda = filenameCoda(filename);
  int l = strlen(coda) + 1;
  free(coda);
  return strndup(filename, (strlen(filename) - l) * sizeof(char));
}

char *changeCoda(const char *filename, const char *coda) {
  char *onset = filenameOnset(filename);
  char *newFilename = malloc((strlen(onset) + strlen(coda) + 2) * sizeof(char));
  sprintf(newFilename, "%s.%s", onset, coda);
  free(onset);
  return newFilename;
}

char *duplicateFilename(const char *filename) {
  char *x = malloc(sizeof(char) *
                   (strlen(filename) + strlen(FILENAME_INDICATION) - 1));
  char *onset = filenameOnset(filename);
  char *coda = filenameCoda(filename);
  sprintf(x, "%s%s%s", onset, FILENAME_INDICATION, coda);
  free(onset); free(coda);
  return x;
}

char *removePath(const char *filename) {
  return chopUntilChar(filename, '/');
}
