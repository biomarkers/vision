#include "vision/BiomarkerImageProcessor.h"

#include <iostream>
#include <boost/timer/timer.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#define DEBUG_MODE

#define HOUGH_HIGH 218
#define HOUGH_LOW  100

/* Predefined circle locations to use when circle detection is disabled.
 */
#define DEFAULT_CIRCLE_CENTER_X      317.5
#define DEFAULT_CIRCLE_CENTER_Y      237.5
#define DEFAULT_CIRCLE_RADIUS        30.0
#define CIRCLE_SHRINK_CONST          15.0

BiomarkerImageProcessor::BiomarkerImageProcessor() :
  circleCenterX(DEFAULT_CIRCLE_CENTER_X),
  circleCenterY(DEFAULT_CIRCLE_CENTER_Y),
  circleRadius(DEFAULT_CIRCLE_RADIUS),
  circleDetectionEnabled(true) {
}

void BiomarkerImageProcessor::reset() {
  // Timer starts here
  timer = boost::timer::cpu_timer();

  // Clear any samples from previous runs
  samples.clear();
}

cv::SerializableScalar BiomarkerImageProcessor::process(cv::Mat frame) {
  cv::Vec3f sampleCircle;
  if(isCircleDetectionEnabled()) {
    sampleCircle = this->findSampleCircle(frame);
  } else {
    sampleCircle = cv::Vec3f(circleCenterX, circleCenterY, circleRadius);
  }

#ifdef DEBUG_MODE
  // Draw the circle to the image
  cv::circle(frame, cv::Point(sampleCircle[0], sampleCircle[1]), sampleCircle[2], cv::Scalar(255, 0, 0), 3, 8, 0);
#endif

  // Sample is a 4-vec of [R, G, B, time]
  cv::Scalar sample = this->sampleSlide(frame, sampleCircle);

  boost::timer::nanosecond_type const one_second(1 * 1000000000LL);
  sample[3] = (float) timer.elapsed().wall / (float) one_second;

  cv::SerializableScalar ser(sample);
  this->samples.push_back(ser);

  return ser;
}

/*
 * Find the circle containing the assay.
 */
cv::Vec3f BiomarkerImageProcessor::findSampleCircle(cv::Mat frame) {
  // Convert the image to grayscale
  cv::Mat frame_gray(frame);
  // cv::cvtColor(frame, frame_gray, CV_BGR2GRAY);
  cv::cvtColor(frame, frame_gray, CV_BGR2HSV);

  std::vector<cv::Mat> channels;
  cv::split(frame_gray, channels);
  frame_gray = channels[2];


  // Blur to remove extraneous detail before edge detection
  // cv::medianBlur(frame_gray, frame_gray, 9);
  // cv::blur(frame_gray, frame_gray, cv::Size(3, 3));
  cv::GaussianBlur(frame_gray, frame_gray, cv::Size(9, 9), 2, 2);

  cv::imshow("blur_win", frame_gray);

  // Edge detection
  // cv::adaptiveThreshold(frame_gray, frame_gray, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 11, 1);
  cv::Mat frame_canny;
  cv::Canny(frame_gray, frame_canny, 5, 10);

  // int erosion_size = 2;
  // cv::Mat element = getStructuringElement(cv::MORPH_ELLIPSE,
  //     cv::Size( 2*erosion_size + 1, 2*erosion_size+1),
  //     cv::Point( erosion_size, erosion_size ));
  // cv::dilate(frame_gray, frame_gray, element );
  // cv::erode(frame_gray, frame_gray, element );

  cv::imshow("canny_win", frame_canny);

  // Extract circle features
  std::vector<cv::Vec3f> circles;
  // HoughCircles(frame_gray, circles, CV_HOUGH_GRADIENT, 1, 50, 50, 40, 0, 0);
  HoughCircles(frame_gray, circles, CV_HOUGH_GRADIENT, 1, 50, 50, 50, 0, 0);

  // Of the circles found, pick the one closest to the center of the frame
  // TODO: This is not the best way to do this. Research probabilistic methods?
  cv::Point frame_center(frame_gray.cols / 2, frame_gray.rows / 2);

  float closest_dist = 10000;
  cv::Vec3f most_center;
  for(size_t i = 0; i < circles.size(); i++) {
    cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));

#ifdef DEBUG_MODE
    // Draw the circle to the image
    cv::circle(frame, cv::Point(circles[i][0], circles[i][1]), circles[i][2], cv::Scalar(0, 0, 255), 1, 8, 0);
#endif

    float dist = pow(center.x - frame_center.x, 2) + pow(center.y - frame_center.y, 2);
    if(dist < closest_dist) {
      most_center = circles[i];
      closest_dist = dist;

      // Shrink detected circle to avoid sampling slide fringe
      most_center[2] -= CIRCLE_SHRINK_CONST;
    }
  }

  return most_center;
}

/*
 * Take a color average of a slide, masking out the area we are not interested
 * in.
 */
cv::Scalar BiomarkerImageProcessor::sampleSlide(cv::Mat frame, cv::Vec3f sampleCircle) {
  // Build our mask of the desired circle space
  cv::Mat roi(frame.size(), CV_8U, cv::Scalar(0));
  cv::Point center(cvRound(sampleCircle[0]), cvRound(sampleCircle[1]));

  int radius = cvRound(sampleCircle[2]);
  cv::circle(roi, center, radius, cv::Scalar(1), -1, 8, 0);

  // Find the average of all pixels within that mask
  cv::Scalar avg = cv::mean(frame, roi);

  return avg;
}


