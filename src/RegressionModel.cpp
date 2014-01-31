#include "../include/RegressionModel.h"
#include "../include/ModelComponent.h"
#include <iostream>

RegressionModel::RegressionModel()
{
    //blue because that's just how I'm feelin'
    //...not really it just doesn't matter
    mFinalComponent = new LinearRegression(-100000, 100000, ModelComponent::BLUE);
    //mFinalComponent = new ExponentialRegression(-10000, 10000, ModelComponent::BLUE);
}

//evaluate a test sample, return the estimation
float RegressionModel::evaluate(std::vector<cv::Scalar> colors)
{
    runModel(colors);
    cv::Mat weights = getModelWeights();
    return evaluateUnknown(weights);
}

//add a calibration point to the model
void RegressionModel::calibrate(std::vector<cv::Scalar> colors,
               float calibrationValue)
{
    runModel(colors);
    //std::cout << "CALIBRATING: " << mComponents[0]->getWeight() << std::endl;
    cv::Mat weights = getModelWeights();
    weights.at<float>(0) = calibrationValue;
    mCalibrationData.push_back(weights);
    mFinalComponent->evaluate(mCalibrationData);
    //std::cout << "CALIBRATE THIS: " << mCalibrationData << std::endl;
    mFinalWeights = mFinalComponent->mWeights;
}

void RegressionModel::dryCalibrate()
{
    mFinalComponent->evaluate(mCalibrationData);
    mFinalWeights = mFinalComponent->mWeights;
}

void RegressionModel::superSecretCalibrationOverride(cv::Mat newCalibration)
{
    mCalibrationData = newCalibration;
}

void RegressionModel::setIndices(int time, int red, int green, int blue, int hue)
{
    mTime = time;
    mRed = red;
    mGreen = green;
    mBlue = blue;
    mHue = hue;
}

//number of calibration runs stored
int RegressionModel::getNumCalibrations()
{
    return mCalibrationData.size().height;
}

//return biggest end frame
int RegressionModel::getModelRunTime()
{
    int last = 0;
    for(int c = 0; c < mComponents.size(); c++)
    {
        int temp = mComponents[c]->getEnd();
        if(temp > last)
            last = temp;
    }
    return last;
}

//matrix of raw model output from calibrations
cv::Mat RegressionModel::getRawCalData()
{
    return mCalibrationData;
}

//unique model name
std::string RegressionModel::GetModelName()
{
    return mModelName;
}

//name of substance being tested
std::string RegressionModel::GetTestName()
{
    return mTestName;
}

//have at least two calibration runs been done?
bool RegressionModel::isCalibrated()
{
    return mCalibrationData.size().height > 1;
}

int RegressionModel::queryBegin(int component)
{
    if(component < mComponents.size())
        return mComponents[component]->getBegin();
    return 0;//throw error later
}

int RegressionModel::queryEnd(int component)
{
    if(component < mComponents.size())
        return mComponents[component]->getEnd();
    return 0;//throw error later
}

ModelComponent::VariableType RegressionModel::queryVariable(int component)
{
    if(component < mComponents.size())
        return mComponents[component]->getVarType();
    return ModelComponent::INVALID_VAR;//throw error later
}

ModelComponent::ModelType RegressionModel::queryModelType(int component)
{
    if(component < mComponents.size())
        return mComponents[component]->getModelType();
    return ModelComponent::INVALID_TYPE;//throw error later
}

//private pusher for model components
void RegressionModel::pushComponent(ModelComponent* ptr)
{
    mComponents.push_back(ptr);
}

//private unknown evaluation
float RegressionModel::evaluateUnknown(cv::Mat weights)
{
    std::cout << "sup\n";
    std::cout << mFinalWeights << "\n" << weights << "\n";
    cv::Mat result = mFinalWeights.t() * weights.t();

    //for exp regr
    //float r1, r2;
    //r1 = mFinalWeights.row(0).at<float>(0);
    //r2 = mFinalWeights.row(1).at<float>(0);
    //result.at<float>(0) = exp(r2) + r1;

    return result.at<float>(0);
}

void RegressionModel::runModel(std::vector<cv::Scalar> colors)
{
    cv::Mat red, green, blue, hue;
    cv::Mat row(1,2,CV_32F);
    cv::Scalar val;
    for(int c = 0; c < colors.size(); c++)
    {
        val = colors[c];
        row.at<float>(1) = val(mTime);
        if(mBlue >= 0)
        {
            row.at<float>(0) = val(mBlue);
            blue.push_back(row);
        }
        if(mGreen >= 0)
        {
            row.at<float>(0) = val(mGreen);
            green.push_back(row);
        }
        if(mRed >= 0)
        {
            row.at<float>(0) = val(mRed);
            red.push_back(row);
        }
        if(mHue >= 0)
        {
            row.at<float>(0) = val(mHue);
            hue.push_back(row);
        }
    }

    for(int c = 0; c < mComponents.size(); c++)
    {
        ModelComponent::VariableType var;
        var = mComponents[c]->mVar;
        if(var == ModelComponent::RED)
            mComponents[c]->evaluate(red);
        if(var == ModelComponent::BLUE)
            mComponents[c]->evaluate(blue);
        if(var == ModelComponent::GREEN)
            mComponents[c]->evaluate(green);
        if(var == ModelComponent::HUE)
            mComponents[c]->evaluate(hue);
    }
}

cv::Mat RegressionModel::getModelWeights()
{
    cv::Mat weights(1, mComponents.size() + 1, CV_32F, 1.f);
    for(int c = 0; c < mComponents.size(); c++)
    {
        weights.at<float>(c+1) = mComponents[c]->getWeight();
    }
    //std::cout << "GettingWeights: " << weights << std::endl;
    return weights;
}






