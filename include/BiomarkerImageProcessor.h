#ifndef __BIOMARKER_IMAGE_PROCESOR_H
#define __BIOMARKER_IMAGE_PROCESOR_H

#include <boost/timer/timer.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class BiomarkerImageProcessor {
public:
  BiomarkerImageProcessor();
  void reset();
  void process();
  cv::Scalar process(cv::Mat frame);
  std::vector<cv::Scalar> getSamples() { return samples; }
private:
  boost::timer::cpu_timer timer;
  std::vector<cv::Scalar> samples;

  cv::Vec3f findSampleCircle(cv::Mat frame);
  cv::Scalar sampleSlide(cv::Mat frame, cv::Vec3f sampleCircle);
};

#endif
