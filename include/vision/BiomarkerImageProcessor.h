#ifndef __BIOMARKER_IMAGE_PROCESOR_H
#define __BIOMARKER_IMAGE_PROCESOR_H

#include <boost/timer/timer.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "serialization/SerializableScalar.h"

class BiomarkerImageProcessor {
public:
  BiomarkerImageProcessor();

  /* Reset the internal timer to prepare for a new test run. */
  void reset();

  /* Process a single input frame and store the result. */
  std::vector<cv::SerializableScalar> process(cv::Mat frame, std::vector<cv::Vec3f> regions);

  /* Return the list of samples for the current test. */
  std::vector<std::vector<cv::SerializableScalar>> getSamples() { return samples; }

  /* Options */
  bool isCircleDetectionEnabled() { return this->circleDetectionEnabled; }
  void setCircleDetectionEnabled(bool c) { this->circleDetectionEnabled = c; }
  float getCircleCenterX() { return this->circleCenterX; }
  float getCircleCenterY() { return this->circleCenterY; }
  float getCircleRadius() { return this->circleRadius; }
  void setCircleCenterX(float x) { this->circleCenterX = x; }
  void setCircleCenterY(float y) { this->circleCenterY = y; }
  void setCircleRadius(float r) { this->circleRadius = r; }
  cv::Scalar getAverageStdDev() { return this->averageStdDev; }

private:
  boost::timer::cpu_timer timer;
  std::vector<std::vector<cv::SerializableScalar>> samples;
  float circleCenterX, circleCenterY;
  float circleRadius;
  bool circleDetectionEnabled;

  cv::Scalar averageStdDev;
  int frameCount = 0;

  cv::Scalar sampleSlide(cv::Mat frame, cv::Mat roi);
};

#endif
