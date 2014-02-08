#include "../include/ModelComponent.h"
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

/*
 *      POINT ANALYSIS
 */

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

/*
 *      LINEAR REGRESSION
 */

LinearRegression::LinearRegression(float begin, float end, ModelComponent::VariableType variable) :
    ModelComponent(begin, end, variable)
{
    //no construction;
}

//support for multiple features as well, for use with the final regression
void LinearRegression::evaluate(cv::Mat x)
{
    x = cutToSize(x);

    cv::Mat weights(1, x.size().width, CV_32F);
    cv::Mat dependent(x.size().height, 1, CV_32F);
    cv::Mat independent(x.size().height, x.size().width, CV_32F, 1.f);

    x.col(0).copyTo(dependent.col(0));
    for(int c = 1; c < x.size().width; c++)
    {
        x.col(c).copyTo(independent.col(c));
    }

    weights = (independent.t() * independent).inv() * independent.t() * dependent;

    mWeights = weights;
}

float LinearRegression::getWeight()
{
    if(mWeights.size().height > 0)
        return mWeights.row(0).at<float>(1);
    else
        return 0; //should throw some error here, get around to that later

}

ModelComponent::ModelType LinearRegression::getModelType()
{
    return ModelComponent::LINEAR;
}

/*
 *      EXP REGRESSION
 */

ExponentialRegression::ExponentialRegression(float begin, float end, ModelComponent::VariableType variable) :
    ModelComponent(begin, end, variable)
{
    //no construction
}

void ExponentialRegression::evaluate(cv::Mat x)
{
    x = cutToSize(x);
    x = logMat(x, .98);

    ModelComponent* linearModel = new LinearRegression(mBegin, mEnd, mVar);
    linearModel->evaluate(x);
    mWeight = linearModel->getWeight();

    mWeights = cv::Mat(2,1,CV_32F,0.f);
    mWeights.row(0).at<float>(0) = mDisp;
    mWeights.row(1).at<float>(0) = mWeight;
}

cv::Mat ExponentialRegression::logMat(cv::Mat x, float percent)
{
    int count = 0, bad_count = 0;
    float total = 0, avg = 0;

    int height = x.size().height;

    int end = height * percent;
    if(end == height && height > 2)
    {
        end -= 1;
    }
    for(int c = height-1; c > end; c--)
    {
        count += 1;
        total += x.col(0).at<float>(c);
    }
    avg = total / count;
    avg -= .05;

    mDisp = avg;

    cv::Mat out(0,2,CV_32F);

    for(int c = 0; c < height; c++)
    {
        float val = x.col(0).at<float>(c) - avg;
        if(val > 0)
        {
            x.col(0).at<float>(c) = log(val);
            out.push_back(x.row(c));
        }
        else
            bad_count++;
    }

    return out;

}

float ExponentialRegression::getWeight()
{
    return mWeight;
}


ModelComponent::ModelType ExponentialRegression::getModelType()
{
    return ModelComponent::EXPONENTIAL;
}



















