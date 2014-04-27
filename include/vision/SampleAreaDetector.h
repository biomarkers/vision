#include <vector>

#include <opencv2/imgproc/imgproc.hpp>

template <class T>
class SampleAreaDetector {
public:
  virtual std::vector<T> detect(cv::Mat frame) = 0;
};
