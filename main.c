#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <magick/api.h>

#include "parsem.h"
#include "filenames.h"

void upscaleImage(const char *infile, const char *outfile, int res, ExceptionInfo *e);

int main(int argc, char **argv) {

  int returnvalue = 0;

  if (argc < 2)
    argv[argc++] = "--help"; // help 🥲

  struct arguments args = parsem(argc, argv);

  returnvalue = 1;
  if (args.outfile && args.infilec > 1)
    puts("Error: can't process multiple files with the -o option");
  else if (args.replace && (args.outfile || args.directory))
    puts("Error: can't use the -r option with -o or -d");
  // ↓ closest possible approximate to "if --help was not called"
  else if (returnvalue = 0, args.infilec) {
    char *outfile = NULL;
    InitializeMagick(NULL);
    ExceptionInfo *e;
    GetExceptionInfo(e);

    for (int i = 0; i < args.infilec; i++) {
      if (e->severity)
        CatchException(e);
      else {
        if (args.replace)
          outfile = args.infiles[i];
        else if (args.outfile)
          outfile = args.outfile;
        else
          // TODO: check if file exists in same directory before doing this
          outfile = duplicateFilename(args.infiles[i]);

        if (args.verbose)
          printf("%s -> %s\n", args.infiles[i], outfile);

        upscaleImage(args.infiles[i], outfile, args.resolution, e);
      }
    }

    if (!args.replace)
      free(outfile);
    DestroyExceptionInfo(e);
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
  Image *out;
  CatchException(e);
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
