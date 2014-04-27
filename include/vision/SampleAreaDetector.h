#ifndef __SAMPLE_AREA_DETECTOR_H
#define __SAMPLE_AREA_DETECTOR_H

#include <vector>

#include <opencv2/imgproc/imgproc.hpp>

template <class T>
class SampleAreaDetector {
public:
  virtual std::vector<T> detect(cv::Mat frame) = 0;
};

#endif
