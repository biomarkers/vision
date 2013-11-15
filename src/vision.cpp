#include <cstdio>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#define DEBUG_MODE
#define FRAME_SKIP 10

/*
 * Skip n frames returning the n+1 frame. Returns NULL if no frames are
 * available.
 */
bool skipNFrames(cv::VideoCapture capture, int n, cv::Mat *image) {
  for(int i = 0; i < n + 1; i++) {
    if(!capture.read(*image)) {
      return false;
    }
  }

  return true;
}

/*
 * Take a color average of a slide, masking out the area we are not interested
 * in.
 */
cv::Scalar sampleSlide(cv::Mat frame) {
  cv::Mat roi(frame.size(), CV_8U);
  cv::circle(roi, cv::Point(680, 370), 70, cv::Scalar(255), -1, 8, 0);

  cv::Scalar avg = cv::mean(frame, roi);

#ifdef DEBUG_MODE
  cv::circle(frame, cv::Point(680, 370), 70, cv::Scalar(255, 0, 0), 1, 8, 0);
#endif

  return avg;
}

int main(int argc, char **argv) {
  if(argc < 2) {
    std::cerr << "Usage: " << argv[0] << " [file_name]" << std::endl;
    return 0;
  }

  cv::namedWindow("img_win", CV_WINDOW_AUTOSIZE);

  cv::VideoCapture cap(argv[1]);

  std::vector<cv::Scalar> avgs;

  cv::Mat frame;
  while(true) {
    bool got_frame = skipNFrames(cap, FRAME_SKIP, &frame);

    if(got_frame) {
      cv::Scalar avg = sampleSlide(frame);
      avgs.push_back(avg);

#ifdef DEBUG_MODE
      double sec = cap.get(CV_CAP_PROP_POS_MSEC) / 1000;
      printf("%f,%f,%f,%f\n", sec, avg.val[0], avg.val[1], avg.val[2]);
#endif

      cv::imshow("img_win", frame);
    } else {
      fprintf(stderr, "No frame...\n");
      break;
    }

    char key = cvWaitKey(1);
    if(key == 27) {
      break;
    }
  }

  return 0;
}
