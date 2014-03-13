#include <math.h>
#include <iostream>
#include "../include/PointAnalysis.h"
#include "../include/SerializableScalar.h"

//BOOST_CLASS_EXPORT_IMPLEMENT(PointAnalysis);

PointAnalysis::PointAnalysis(float begin, float end, ModelComponent::VariableType variable) :
    ModelComponent(begin, end, variable),
    mAvg(0)
{
    //no construction
}

void PointAnalysis::evaluate(cv::Mat x)
{
    x = cutToSize(x);
    int height = x.size().height;

    //r2 value will instead be standard dev, sigma
    float sem = getSquaredFromMean(x);
    sem /= height;
    sem = sqrt(sem);
    mR2 = (sem);

    mAvg = 0;
    for(int c = 0; c < height; c++)
    {
        mAvg += (x.row(c).at<float>(0) / height);
    }
}

float PointAnalysis::getWeight()
{
    return mAvg;
}

std::string PointAnalysis::getStatString()
{
    std::ostringstream data;
    data << "Point Analysis Standard Deviation = " << mR2 << "\n";
    return data.str();
}

float PointAnalysis::graphPoint(int second)
{
    return mAvg + 0 * second;
}

ModelComponent::ModelType PointAnalysis::getModelType()
{
    return ModelComponent::POINT;
}

