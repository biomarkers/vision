#include "../include/ModelComponent.h"
#include "../include/SerializableScalar.h"
#include <math.h>


ModelComponent::ModelComponent(float begin, float end, VariableType variable) :
    mBegin(begin), mEnd(end), mWeights(), mVar(variable)
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




