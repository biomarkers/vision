#include "vision/BiomarkerImageProcessor.h"

#include <iostream>
#include <boost/timer/timer.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "vision/CircularSampleAreaDetector.h"

#define DEBUG_MODE

/* Predefined circle locations to use when circle detection is disabled.
 */
#define DEFAULT_CIRCLE_CENTER_X      317.5
#define DEFAULT_CIRCLE_CENTER_Y      237.5
#define DEFAULT_CIRCLE_RADIUS        30.0

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
  averageStdDev = cv::Scalar::all(0);
  frameCount = 0;
}

std::vector<cv::SerializableScalar> BiomarkerImageProcessor::process(cv::Mat frame, std::vector<cv::Vec3f> circles) {
  if(this->samples.size() < circles.size()) {
    this->samples.push_back(std::vector<cv::SerializableScalar>());
  }

  std::vector<cv::SerializableScalar> frameSamples;
  for(size_t i = 0; i < circles.size(); i++) {
    // Sample is a 4-vec of [R, G, B, time]
    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);
    cv::circle(mask, cv::Point(circles[i][0], circles[i][1]), circles[i][2], cv::Scalar(255), -1);

    cv::Scalar sample = this->sampleSlide(frame, mask);

#ifdef DEBUG_MODE
    // Draw the circle to the image
    cv::circle(frame, cv::Point(circles[i][0], circles[i][1]), circles[i][2], sample, 5);
#endif

    boost::timer::nanosecond_type const one_second(1 * 1000000000LL);
    sample[3] = (float) timer.elapsed().wall / (float) one_second;

    cv::SerializableScalar ser(sample);
    frameSamples.push_back(sample);

    this->samples[i].push_back(ser);
  }

  return frameSamples;
}

/*
 * Take a color average of a slide, masking out the area we are not interested
 * in.
 */
cv::Scalar BiomarkerImageProcessor::sampleSlide(cv::Mat frame, cv::Mat roi) {
  // Find the average of all pixels within that mask
  cv::Scalar avg;
  cv::Scalar stddev;

  cv::meanStdDev(frame, avg, stddev, roi);

  frameCount++;
  averageStdDev += ((stddev - averageStdDev) / frameCount);

  return avg;
}


