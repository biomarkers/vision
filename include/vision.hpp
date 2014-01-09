class BiomarkerImageProcessor {
  std::vector<cv::Scalar> samples;
public:
  cv::Scalar process(cv::Mat frame);
  std::vector<cv::Scalar> getSamples() { return samples; }
private:
  cv::Vec3f findSampleCircle(cv::Mat frame);
  cv::Scalar sampleSlide(cv::Mat frame, cv::Vec3f sampleCircle);
};

