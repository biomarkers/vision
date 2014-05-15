#include <math.h>
#include <iostream>
#include "regression/PointAnalysis.h"
#include "serialization/SerializableScalar.h"

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
    mMSE = sem / height;
    mR2 = sqrt(mMSE);

    mAvg = 0;
    for(int c = 0; c < height; c++)
    {
        mAvg += (x.row(c).at<float>(0) / height);
    }


}

float PointAnalysis::getEstimation(cv::Mat x)
{
    (void) x;
    return mAvg;
}

float PointAnalysis::getWeight()
{
    return mAvg;
}

std::string PointAnalysis::getStatString(bool printBounds)
{
    std::ostringstream data;
    data << "Point Analysis\n";
    if(printBounds)
    {
        insertVar(&data);
        data << "channel from " << mBegin << "s to " << mEnd << "s\n";
    }
    data << "y = " << mAvg << "\n";
    data << "Standard Deviation = " << mR2 << "\n";
    data << "Mean Squared Error = " << mMSE << "\n";

    return data.str();
}

float PointAnalysis::graphPoint(int second)
{
    if(second < mBegin || second > mEnd)
        return 0;
    return mAvg + 0 * second;
}

ModelComponent::ModelType PointAnalysis::getModelType()
{
    return ModelComponent::POINT;
}

