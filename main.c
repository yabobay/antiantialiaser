#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <magick/api.h>

#include "parsem.h"
#include "filenames.h"

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
    Image *img = NULL;
    Image *out = NULL;
    ImageInfo *imgInfo;
    ExceptionInfo *e;
    GetExceptionInfo(e);
        
    for (int i = 0; i < args.infilec; i++) {
      imgInfo = CloneImageInfo(0); // initialize empty image
      // read the image from disk
      strcpy(imgInfo->filename, args.infiles[i]);
      img = ReadImage(imgInfo, e);

      if (e->severity)
        CatchException(e);
      else {
        if (args.replace)
          outfile = imgInfo->filename;
        else
          outfile = args.outfile ?
            args.outfile : duplicateFilename(args.infiles[i]);

        if (args.verbose)
          printf("%s -> %s\n", args.infiles[i], outfile);

        if (img != NULL) {
          unsigned int x = img->columns;
          unsigned int y = img->rows;
          unsigned int ty = args.resolution; // t is for target
          double multiplier = (double) args.resolution / (double) img->rows;
          unsigned int tx = (int) ((double) x * multiplier);
          if (ty <= y)
            fprintf(stderr, "WARNING: %s is bigger than %dp. Won't resize.\n",
                    args.infiles[i], ty);
          else {
            // TODO: ask about existing filename.
            out = ScaleImage(img, tx, ty, e); // resize the image
            CatchException(e);
            // write new image to disk
            strcpy(out->filename, outfile);
            if (!WriteImage(imgInfo, out))
              CatchException(&out->exception);
          }
        }

        if (!args.replace)
          free(outfile);
      }

    }

    DestroyImage(img);
    DestroyImage(out);
    DestroyImageInfo(imgInfo);
    DestroyExceptionInfo(e);
    DestroyMagick();

  }
  
  free(args.infiles);

  return returnvalue;

}
