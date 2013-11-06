#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <cv.h>
#include <highgui.h>

#define DEBUG_MODE true
#define FRAME_SKIP 10

/*
 * Skip n frames returning the n+1 frame. Returns NULL if no frames are
 * available.
 */
CvArr *skip_n_frames(CvCapture *capture, int n) {
  for(int i = 0; i < n; i++) {
    if(cvQueryFrame(capture) == NULL) {
      return NULL;
    }
  }

  return cvQueryFrame(capture);
}

/*
 * Take a color average of a slide, masking out the area we are not interested
 * in.
 */
CvScalar sample_slide(CvArr *frame) {
  cvSetImageROI(frame, cvRect(625, 325, 100, 100));
  CvScalar avg = cvAvg(frame, NULL);
  cvResetImageROI(frame);

#ifdef DEBUG_MODE
  cvRectangle(frame, cvPoint(625, 325), cvPoint(725, 425), CV_RGB(0, 255, 0), 2, 8, 0);
#endif

  return avg;
}

int main(int argc, char **argv) {
  if(argc < 2) {
    fprintf(stderr, "Usage: %s [file_name]\n", argv[0]);
    return 0;
  }

  cvNamedWindow("img_win", CV_WINDOW_AUTOSIZE);

  CvCapture* cap = cvCaptureFromFile(argv[1]);
  CvArr *frame;
  while(true) {
    frame = skip_n_frames(cap, FRAME_SKIP);

    if(frame != NULL) {
      double sec = cvGetCaptureProperty(cap, CV_CAP_PROP_POS_MSEC) / 1000;
      CvScalar avg = sample_slide(frame);

#ifdef DEBUG_MODE
      printf("%f,%f,%f,%f\n", sec,avg.val[0], avg.val[1], avg.val[2]);
#endif

      cvShowImage("img_win", frame);

      char key = cvWaitKey(1);
      if(key == 27) {
        break;
      }
    } else {
      fprintf(stderr, "No frame...\n");
      break;
    }
  }

  cvReleaseCapture(&cap);
  cvDestroyWindow("img_win");

  return 0;
}
