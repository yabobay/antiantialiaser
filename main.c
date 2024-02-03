#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <magick/api.h>

#include "parsem.h"
#include "filenames.h"

void upscaleImage(const char *infile, const char *outfile, int res, ExceptionInfo *e);

int main(int argc, char *argv[]) {
  int returnvalue = 1;

  if (argc < 2)
    argv[argc++] = "--help"; // help 🥲
  struct arguments args = parsem(argc, argv);

  if (args.outfile && args.infilec > 1)
    puts("Error: can't process multiple files with the -o option.\nTry -d instead to specify a directory.");
  else if (args.replace && (args.outfile || args.directory || args.filetype))
    puts("Error: can't use the -r option with any of: -o, -d, -f");
  else if (args.outfile && args.directory)
    puts("Error: can't use the -o and -d flags together");
  // ↓ closest possible approximate to "if --help was not called"
  else if (returnvalue = 0, args.infilec) {
    char *outfile = NULL;
    InitializeMagick(NULL);
    ExceptionInfo e;
    GetExceptionInfo(&e);

    for (int i = 0; i < args.infilec; i++) {
      CatchException(&e);

      if (args.directory) {
        char *filename = removePath(args.infiles[i]);
        int length = strlen(args.directory) + 1 + strlen(filename);
        outfile = malloc(sizeof(char) * (length + 1));
        sprintf(outfile, "%s/%s", args.directory, filename);
        free(filename);
      }
      else if (args.outfile)
        outfile = strdup(args.outfile);
      else
        outfile = strdup(args.infiles[i]);

      if (args.filetype) {
        char *tmp = outfile;
        outfile = changeCoda(tmp, args.filetype);
        free(tmp);
      }

      if (!args.replace)
        while (fileExists(outfile)) {
          char *tmp = outfile;
          outfile = duplicateFilename(tmp);
          free(tmp);
        }

      if (args.verbose)
        printf("%s -> %s\n", args.infiles[i], outfile);
      if (!args.pretend)
        upscaleImage(args.infiles[i], outfile, args.resolution, &e);
    }

    free(outfile);
    DestroyExceptionInfo(&e);
    DestroyMagick();
  }
  
  free(args.infiles);
  return returnvalue;
}

void upscaleImage(const char *infile, const char *outfile, int res, ExceptionInfo *e) {
  // read the image from disk
  ImageInfo *imgInfo = CloneImageInfo(0); // initialize empty image
  strcpy(imgInfo->filename, infile);
  Image *img = ReadImage(imgInfo, e);
  CatchException(e);
  Image *out;
  if (img != NULL) {
    unsigned int x = img->columns;
    unsigned int y = img->rows;
    unsigned int ty = res; // t is for target
    double multiplier = (double) res / (double) img->rows;
    unsigned int tx = (int) ((double) x * multiplier);
    if (ty <= y) {
      fprintf(stderr, "WARNING: %s is bigger than %dp. Won't resize.\n",
              infile, ty);
      tx = x; ty = y;
    }
    out = ScaleImage(img, tx, ty, e); // resize the image
    CatchException(e);
    // write new image to disk
    strcpy(out->filename, outfile);
    if (!WriteImage(imgInfo, out))
      CatchException(&out->exception);
  }
  DestroyImage(img);
  DestroyImage(out);
  DestroyImageInfo(imgInfo);
}
