#include <opencv2/core/core.hpp>
#include "serialization/SerializableScalar.h"

void cv::SerializableScalar::putScalar(cv::Scalar scalar)
{
    (*this).val[0] = scalar(0);
    (*this).val[1] = scalar(1);
    (*this).val[2] = scalar(2);
    (*this).val[3] = scalar(3);
}

cv::SerializableScalar::SerializableScalar(Scalar scalar)
{
    putScalar(scalar);
}

cv::SerializableScalar::SerializableScalar() : Scalar()
{}
