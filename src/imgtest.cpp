#include <iostream>
#include <opencv2/highgui/highgui.hpp>

#include "BiomarkerImageProcessor.h"
#include "DataPersistence.h"

#define FRAME_SKIP 0

/*
 * Skip n frames returning the n+1 frame. Returns NULL if no frames are
 * available.
 */
bool skipNFrames(cv::VideoCapture capture, int n, cv::Mat *image) {
  for(int i = 0; i < n + 1; i++) {
    if(!capture.read(*image)) {
      return false;
    }
  }

  return true;
}

int main(int argc, char **argv) {
  if(argc < 2) {
    std::cerr << "Usage: " << argv[0] << " [file_name]" << std::endl;
    return 0;
  }

  BiomarkerImageProcessor processor;

  cv::namedWindow("img_win", CV_WINDOW_AUTOSIZE);
  cv::VideoCapture cap(argv[1]);

  DataStore p = DataStore::open();
  p.createTables();
  std::vector<ModelEntry> entries = p.findAllModelEntries();
  for(int i = 0; i < entries.size(); i++) {
    std::cout << entries[i].data << std::endl;
  }
  p.close();

  /*
  cv::Mat frame;
  // skipNFrames(cap, 8730, &frame);
  while(true) {
    bool got_frame = skipNFrames(cap, FRAME_SKIP, &frame);

    if(got_frame) {
      cv::Scalar sample = processor.process(frame);
      std::cout << sample[0] << "," << sample[1] << "," << sample[2] << "," << sample[3] << std::endl;

      cv::imshow("img_win", frame);
    } else {
      break;
    }

    char key = cvWaitKey(1);
    if(key == 27) {
      break;
    }
  }
  */

  return 0;
}
