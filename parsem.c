#include <wctype.h>
#include <stdlib.h>
#include <stdbool.h>

#include <argp.h>

#include "parsem.h"

#define DEFAULTRES 720

const char *argp_program_version = "AntiAntiAliaser alpha";

error_t parse_opt (int key, char *arg, struct argp_state *state);

struct argp_option options[] = {
  {"output", 'o', "FILENAME", 0, "File to output to (optional)"},
  {"filetype", 'f', "EXTENSION", 0, "Filetype to convert to (optional)"},
  {"dir", 'd', "DIRECTORY", 0, "Directory to dump file(s) into"},
  {"size", 's', "N", 0, "Resolution to output (default: 720)"},
  {"replace", 'r', 0, 0, "Replace original files. (default: no)"},
  {"verbose", 'v', 0, 0, "Enable verbose mode"},
  {"pretend", 'p', 0, OPTION_HIDDEN, 0, 0}, // AKA don't do anything
  {} // for some reason this makes unnamed arguments work properly
};

struct argp argp = {
  options, parse_opt, "FILENAMES...", 0
};

error_t parse_opt (int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;
  if (iswprint(key))
    switch (key) {
    case 'v':
      arguments->verbose = true;
      break;
    case 's':
      arguments->resolution = atoi(arg);
      break;
    case 'o':
      arguments->outfile = arg;
      break;
    case 'd':
      arguments->directory = arg;
      break;
    case 'r':
      arguments->replace = true;
      break;
    case 'p':
      arguments->pretend = true;
      arguments->verbose = true;
      break;
    case 'f':
      arguments->filetype = arg;
      break;
    }
  else if (arg) { // argument is anonymous
    ++arguments->infilec;
    arguments->infiles =
      realloc(arguments->infiles, sizeof(char*) * arguments->infilec);
    arguments->infiles[arguments->infilec - 1] = arg;
  }
  return 0;
}

struct arguments parsem(int argc, char **argv) {
  struct arguments arguments;

  // set default values
  arguments.verbose = false;
  arguments.outfile = NULL;
  arguments.resolution = DEFAULTRES;
  arguments.infiles = malloc(sizeof(char*));
  arguments.infilec = 0;
  arguments.directory = NULL;
  arguments.replace = false;
  arguments.pretend = false;
 
  // we use ARGP_NO_EXIT so that main can free arguments.infiles later
  arguments.e = argp_parse(&argp, argc, argv, ARGP_NO_EXIT, 0, &arguments);

  return arguments;
}
