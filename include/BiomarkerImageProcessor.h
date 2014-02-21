#ifndef __BIOMARKER_IMAGE_PROCESOR_H
#define __BIOMARKER_IMAGE_PROCESOR_H

#include <boost/timer/timer.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "SerializableScalar.h"

class BiomarkerImageProcessor {
public:
  BiomarkerImageProcessor();

  /* Reset the internal timer to prepare for a new test run. */
  void reset();

  /* Process a single input frame and store the result. */
  cv::SerializableScalar process(cv::Mat frame);

  /* Return the list of samples for the current test. */
  std::vector<cv::SerializableScalar> getSamples() { return samples; }

  /* Options */
  bool isCircleDetectionEnabled() { return this->circleDetectionEnabled; }
  void setCircleDetectionEnabled(bool c) { this->circleDetectionEnabled = c; }
  float getCircleCenterX() { return this->circleCenterX; }
  float getCircleCenterY() { return this->circleCenterY; }
  void setCircleCenterX(float x) { this->circleCenterX = x; }
  void setCircleCenterY(float y) { this->circleCenterY = y; }

private:
  boost::timer::cpu_timer timer;
  std::vector<cv::SerializableScalar> samples;
  bool circleDetectionEnabled;
  float circleCenterX, circleCenterY;
  float circleRadius;

  cv::Vec3f findSampleCircle(cv::Mat frame);
  cv::Scalar sampleSlide(cv::Mat frame, cv::Vec3f sampleCircle);
};

#endif
