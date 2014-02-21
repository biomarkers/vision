#include "../include/LinearRegression.h"
#include <math.h>
#include "../include/SerializableScalar.h"

//BOOST_CLASS_EXPORT_IMPLEMENT(LinearRegression);

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
