#include "regression/ExponentialRegression.h"
#include "regression/LinearRegression.h"
#include "serialization/SerializableScalar.h"
#include <math.h>
#include <iostream>

//BOOST_CLASS_EXPORT_IMPLEMENT(ExponentialRegression);

ExponentialRegression::ExponentialRegression(float begin, float end, ModelComponent::VariableType variable) :
    ModelComponent(begin, end, variable)
{
    //no construction
}

void ExponentialRegression::evaluate(cv::Mat x)
{
    x = cutToSize(x);
    float sem = getSquaredFromMean(x);
    cv::Mat y;
    y = logMat(x, .98);

    ComponentPtr linearModel(new LinearRegression(mBegin, mEnd, mVar));
    linearModel->evaluate(y);
    mWeight = linearModel->getWeight();

    float se = 0;
    float val;
    for(int c = 0; c < x.size().height; c++)
    {
        val = exp(mWeight * x.row(c).at<float>(1)) + mDisp;
        se += pow(val - x.row(c).at<float>(0), 2.f);
    }
    mR2 = (1.f - se/sem);

    mWeights = cv::Mat(2,1,CV_32F,0.f);
    mWeights.row(0).at<float>(0) = mDisp;
    mWeights.row(1).at<float>(0) = mWeight;
}

float ExponentialRegression::getEstimation(cv::Mat x)
{
    //assuming x will be [1, x]...
    return exp(mWeight * x.row(0).at<float>(1)) + mDisp;
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

std::string ExponentialRegression::getStatString()
{
    std::ostringstream data;
    data << "Exponential Regression Component R^2 = " << mR2 << "\n";
    return data.str();
}

float ExponentialRegression::getWeight()
{
    return mWeight;
}

float ExponentialRegression::graphPoint(int second)
{
    if(second < mBegin || second > mEnd)
        return 0;
    return exp(mWeight * second) + mDisp;
}

ModelComponent::ModelType ExponentialRegression::getModelType()
{
    return ModelComponent::EXPONENTIAL;
}
