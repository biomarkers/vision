#include "../include/ModelComponent.h"


ModelComponent::ModelComponent(float begin, float end) :
    mBegin(begin), mEnd(end), mWeights()
{
    //no construction
}

float ModelComponent::getBegin()
{
    return mBegin;
}

float ModelComponent::getEnd()
{
    return mEnd;
}

cv::Mat ModelComponent::cutToSize(cv::Mat x)
{
    cv::Mat out;
    //

}

/*
 *      POINT ANALYSIS
 */

PointAnalysis::PointAnalysis(float begin, float end) :
    ModelComponent(begin, end)
{
    //no construction
}


/*
 *      LINEAR REGRESSION
 */


/*
 *      EXP REGRESSION
 */
