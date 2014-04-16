#include "regression/ModelComponent.h"
#include "serialization/SerializableScalar.h"
#include <math.h>


ModelComponent::ModelComponent(float begin, float end, VariableType variable) :
    mBegin(begin), mEnd(end), mWeights(), mR2(), mVar(variable)
{
    //no construction
}

ModelComponent::~ModelComponent()
{
}

float ModelComponent::getBegin()
{
    return mBegin;
}

float ModelComponent::getEnd()
{
    return mEnd;
}

ModelComponent::VariableType ModelComponent::getVarType()
{
    return mVar;
}

//assume rows are in the form [y, x]
cv::Mat ModelComponent::cutToSize(cv::Mat x)
{
    cv::Mat out;
    int height = x.size().height;

    for(int c = 0; c < height; c++)
    {
        int val = x.row(c).at<float>(1);
        if(mBegin <= val && val <= mEnd)
        {
            out.push_back(x.row(c));
        }
    }
    return out;
}

float ModelComponent::getSquaredFromMean(cv::Mat x)
{
    float mean = 0;
    for(int c = 0; c < x.size().height; c++)
    {
        mean += (x.row(c).at<float>(0) / (float)x.size().height);
    }

    //std::cout << "MEAN: " << mean << "\n";

    float se = 0;
    for(int c = 0; c < x.size().height; c++)
    {
        se += (pow(mean - x.row(c).at<float>(0), 2.f));
    }

    //std::cout << "ERR FROM MEAN ^2: " << se << "\n";
    return se;
}

void ModelComponent::insertVar(std::ostringstream *stream)
{
    switch(mVar)
    {
    case RED:
        *stream << "Red "; break;
    case BLUE:
        *stream << "Blue "; break;
    case GREEN:
        *stream << "Green "; break;
    case HUE:
        *stream << "Hue "; break;
    default:
        *stream << "? "; break;
    }
}
