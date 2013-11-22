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
	printf("loop\n");
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
  printf("kkkk\n");
  cvSetImageROI(frame, cvRect(625, 325, 100, 100));
  printf("koalas\n");
  CvScalar avg = cvAvg(frame, NULL);
  printf("waaaaaaaaatk\n");
  cvResetImageROI(frame);
  printf("tie dye\n");
#ifdef DEBUG_MODE
  cvRectangle(frame, cvPoint(625, 325), cvPoint(725, 425), CV_RGB(0, 255, 0), 2, 8, 0);
#endif
  printf("dyed tie\n");
  return avg;
}

int main(int argc, char **argv) {
  if(argc < 2) {
    fprintf(stderr, "Usage: %s [file_name]\n", argv[0]);
    //return 0;
  }

  cvNamedWindow("img_win", CV_WINDOW_AUTOSIZE);

  CvCapture* cap = cvCaptureFromAVI("a.avi");
  if(cap == NULL)
  {
	printf("WHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT THE HELL\n\n");
  }
  
  CvArr *frame;
  while(true) {
	printf("grabbing a frame\n");
    frame = skip_n_frames(cap, FRAME_SKIP);

    if(frame != NULL) {
	  printf("wat\n");
      double sec = cvGetCaptureProperty(cap, CV_CAP_PROP_POS_MSEC) / 1000;
	  printf("samplin\n");
      //CvScalar avg = sample_slide(frame);
	  
	  
	  printf("kkkk\n");
	  cvSetImageROI(frame, cvRect(125, 125, 100, 100));
	  printf("koalas\n");
	  CvScalar avg = cvAvg(frame, NULL);
	  printf("waaaaaaaaatk\n");
	  //cvResetImageROI(frame);
	  printf("tie dye\n");
	#ifdef DEBUG_MODE
	  //cvRectangle(frame, cvPoint(625, 325), cvPoint(725, 425), CV_RGB(0, 255, 0), 2, 8, 0);
	#endif
	  printf("dyed tie\n");
	  
	  
	  printf("k\n");
#ifdef DEBUG_MODE
      printf("%f,%f,%f,%f\n", sec,avg.val[0], avg.val[1], avg.val[2]);
#endif
      printf("showing...\n");
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
