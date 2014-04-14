#include "BiomarkerImageProcessor.h"

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

BiomarkerImageProcessor::BiomarkerImageProcessor() :
  circleCenterX(DEFAULT_CIRCLE_CENTER_X),
  circleCenterY(DEFAULT_CIRCLE_CENTER_Y),
  circleRadius(DEFAULT_CIRCLE_RADIUS),
  circleDetectionEnabled(false) {
}

void BiomarkerImageProcessor::reset() {
  timer = boost::timer::cpu_timer();
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
  cv::circle(frame, cv::Point(sampleCircle[0], sampleCircle[1]), sampleCircle[2], cv::Scalar(255, 0, 0), 1, 8, 0);
#endif

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


