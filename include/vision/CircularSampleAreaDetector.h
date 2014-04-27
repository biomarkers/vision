#include "vision/SampleAreaDetector.h"

#define CIRCLE_SHRINK_FACTOR 0.85

class CircularSampleAreaDetector : SampleAreaDetector<cv::Vec3f> {
public:
  std::vector<cv::Vec3f> detect(cv::Mat frame);
};
