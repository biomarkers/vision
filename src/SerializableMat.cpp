#include <opencv2/core/core.hpp>
#include "../include/SerializableMat.h"
#include <iostream>

void cv::SerializableMat::putMatrix(cv::Mat mat)
{
    int height;
    height = mat.size().height;
    release();
    for(int c = 0; c < height; c++)
    {
        push_back(mat.row(c));
    }
}

cv::SerializableMat::SerializableMat(cv::Mat mat)
{
    putMatrix(mat);
}

cv::SerializableMat::SerializableMat() : Mat()
{

}
