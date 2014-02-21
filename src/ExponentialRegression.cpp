#include "../include/ExponentialRegression.h"
#include "../include/SerializableScalar.h"
#include "../include/LinearRegression.h"
#include <math.h>

//BOOST_CLASS_EXPORT_IMPLEMENT(ExponentialRegression);

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
