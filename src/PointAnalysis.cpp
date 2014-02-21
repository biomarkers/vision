#include <math.h>
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
    mAvg = 0;
    for(int c = 0; c < height; c++)
    {
        mAvg += x.row(c).at<float>(0);
    }
    mAvg /= (float)(height);
}

float PointAnalysis::getWeight()
{
    return mAvg;
}

ModelComponent::ModelType PointAnalysis::getModelType()
{
    return ModelComponent::POINT;
}

