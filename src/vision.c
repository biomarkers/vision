#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <cv.h>
#include <highgui.h>

int main(int argc, char **argv) {
  if(argc < 2) {
    fprintf(stderr, "Usage: %s [file_name]\n", argv[0]);
    return 0;
  }

  cvNamedWindow("img_win", CV_WINDOW_AUTOSIZE);

  CvCapture* cap = cvCaptureFromFile(argv[1]);
  while(true) {
    CvArr *frame = cvQueryFrame(cap);

    if(frame != NULL) {
      cvShowImage("img_win", frame);
    } else {
      fprintf(stderr, "No frame...\n");
    }

    char key = cvWaitKey(30);
    if(key == 27) {
      break;
    }
  }

  cvReleaseCapture(&cap);

  return 0;
}
