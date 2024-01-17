#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *filenameCoda(const char *filename) {
  char *dot = strrchr(filename, '.');
  if (!dot)
    return "";
  return strdup(dot + 1);
}

char *filenameOnset(const char *filename) {
  /* slightly wasteful but i literally don't care. feel free to fix it
     if you want to */
  char *coda = filenameCoda(filename);
  int l = strlen(coda) + 1;
  free(coda);
  return strndup(filename, (strlen(filename) - l) * sizeof(char));
}

#define FILENAME_INDICATION " (upscaled)."

char *duplicateFilename(const char *filename) {
  char *x = malloc(sizeof(char) *
                   (strlen(filename) + strlen(FILENAME_INDICATION) - 1));
  char *onset = filenameOnset(filename);
  char *coda = filenameCoda(filename);
  sprintf(x, "%s%s%s", onset, FILENAME_INDICATION, coda);
  free(onset); free(coda);
  return x;
}
