#include "regression/ExponentialRegression.h"
#include "regression/LinearRegression.h"
#include "serialization/SerializableScalar.h"
#include <math.h>
#include <iostream>

//BOOST_CLASS_EXPORT_IMPLEMENT(ExponentialRegression);

ExponentialRegression::ExponentialRegression(float begin, float end, ModelComponent::VariableType variable) :
    ModelComponent(begin, end, variable),
    mLinearComponent(new LinearRegression(begin, end, variable))
{
    //no construction
}

void ExponentialRegression::evaluate(cv::Mat x)
{
    x = cutToSize(x);
    float sem = getSquaredFromMean(x);
    cv::Mat y;

    std::cout << x << "\n\n";

    y = logMat(x, .98);

    std::cout << y << "\n\n";

    mLinearComponent->evaluate(y);
    mWeight = mLinearComponent->getWeight();

    float se = 0;
    float val;
    for(int c = 0; c < x.size().height; c++)
    {
        val = exp(mWeight * x.row(c).at<float>(1)) + mDisp;
        se += pow(val - x.row(c).at<float>(0), 2.f);
    }
    mR2 = (1.f - se/sem);
}

float ExponentialRegression::getEstimation(cv::Mat x)
{
    //assuming x will be [1, x]...
    float val = mLinearComponent->getEstimation(x);
    return exp(val) + mDisp;
}


//not taking noise into account, just find minimum y value and drop all the values by
// percent of the minimum value
cv::Mat ExponentialRegression::logMat(cv::Mat x, float percent)
{
    float minimum = x.col(0).at<float>(0);
    int height = x.size().height;

    for(int c = 0; c < height; c++)
    {
        float val = x.col(0).at<float>(c);
        if(val < minimum)
            minimum = val;
    }

    if(minimum > 0)
    {
        mDisp = minimum * percent;
    }
    else
    {
        //flip percent over 100% so that we end up shifting
        //all values to be above zero
        mDisp = minimum * (2.f - percent);
    }

    cv::Mat out(0,2,CV_32F);

    for(int c = 0; c < height; c++)
    {
        float val = x.col(0).at<float>(c) - mDisp;
        if(val > 0)
        {
            x.col(0).at<float>(c) = log(val);
            out.push_back(x.row(c));
        }
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
    float val = mLinearComponent->graphPoint(second);
    return exp(val) + mDisp;
}

ModelComponent::ModelType ExponentialRegression::getModelType()
{
    return ModelComponent::EXPONENTIAL;
}
