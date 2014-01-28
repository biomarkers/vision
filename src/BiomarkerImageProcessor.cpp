#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "BiomarkerImageProcessor.h"

#define DEBUG_MODE
#define FRAME_SKIP 0

#define HOUGH_HIGH 218
#define HOUGH_LOW  100

/* Predefined circle locations to use when circle detection is disabled.
 */
#define CIRCLE_CENTER_X      317.5
#define CIRCLE_CENTER_Y      237.5
#define CIRCLE_CENTER_RADIUS 30.0

/* Enable or disable circle detection. Will use predefined values above if
 * disabled.
 */
// #define CIRCLE_DETECTION

BiomarkerImageProcessor::BiomarkerImageProcessor() : frameCounter(0) {
}

cv::Scalar BiomarkerImageProcessor::process(cv::Mat frame) {
  cv::cvtColor(frame, frame, CV_BGR2HLS);

#ifdef CIRCLE_DETECTION
  cv::Vec3f sampleCircle = this->findSampleCircle(frame);
#else
  cv::Vec3f sampleCircle = cv::Vec3f(CIRCLE_CENTER_X, CIRCLE_CENTER_Y, CIRCLE_CENTER_RADIUS);
#endif

#ifdef DEBUG_MODE
  cv::circle(frame, cv::Point(sampleCircle[0], sampleCircle[1]), sampleCircle[2], cv::Scalar(255, 0, 0), 1, 8, 0);
#endif

  cv::Scalar sample = this->sampleSlide(frame, sampleCircle);
  sample[3] = (float) frameCounter++;

  samples.push_back(sample);

  return sample;
}

/*
 * Find the circle containing the assay.
 */
cv::Vec3f BiomarkerImageProcessor::findSampleCircle(cv::Mat frame) {
  cv::Mat frame_gray;
  cv::cvtColor(frame, frame_gray, CV_BGR2GRAY);

  // GaussianBlur(frame_gray, frame_gray, cv::Size(3, 3), 2, 2);

  std::vector<cv::Vec3f> circles;
  HoughCircles(frame_gray, circles, CV_HOUGH_GRADIENT, 2, frame_gray.rows / 4, HOUGH_HIGH, HOUGH_LOW, 0, 0);

  cv::Point frame_center(frame.cols / 2, frame.rows / 2);

  float closest_dist = 10000;
  cv::Vec3f most_center;
  for(size_t i = 0; i < circles.size(); i++) {
    cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));

    float dist = pow(center.x - frame_center.x, 2) + pow(center.y - frame_center.y, 2);
    if(dist < closest_dist) {
      most_center = circles[i];
    }
  }

  return most_center;
}

/*
 * Take a color average of a slide, masking out the area we are not interested
 * in.
 */
cv::Scalar BiomarkerImageProcessor::sampleSlide(cv::Mat frame, cv::Vec3f sampleCircle) {
  cv::Mat roi(frame.size(), CV_8U, cv::Scalar(0));
  cv::Point center(cvRound(sampleCircle[0]), cvRound(sampleCircle[1]));

  int radius = cvRound(sampleCircle[2]);
  cv::circle(roi, center, radius, cv::Scalar(1), -1, 8, 0);

  cv::Scalar avg = cv::mean(frame, roi);

  return avg;
}

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

int main(int argc, char **argv) {
  if(argc < 2) {
    std::cerr << "Usage: " << argv[0] << " [file_name]" << std::endl;
    return 0;
  }

  BiomarkerImageProcessor processor;

  cv::namedWindow("img_win", CV_WINDOW_AUTOSIZE);
  cv::VideoCapture cap(argv[1]);

  cv::Mat frame;
  // skipNFrames(cap, 8730, &frame);
  while(true) {
    bool got_frame = skipNFrames(cap, FRAME_SKIP, &frame);

    if(got_frame) {
      cv::Scalar sample = processor.process(frame);
      std::cout << sample[0] << "," << sample[1] << "," << sample[2] << "," << sample[3] << std::endl;

      cv::imshow("img_win", frame);
    } else {
      std::cerr << "No frame..." << std::endl;
      break;
    }

    char key = cvWaitKey(1);
    if(key == 27) {
      break;
    }
  }

  return 0;
}
