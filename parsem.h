#ifndef ARGPARSING_H
#define ARGPARSING_H

#include <argp.h>

struct arguments {
  bool verbose;
  int resolution;
  char **infiles;
  int infilec;
  char *outfile;
  char *directory;
  bool replace;
  bool pretend;
  error_t e; // basically just used in case of anything;
};

struct arguments parsem(int argc, char **argv);

#endif
