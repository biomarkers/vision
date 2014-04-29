#include "regression/RegressionModel.h"
#include "regression/ModelComponent.h"
#include "regression/ModelComponents.h"
#include <iostream>
#include <fstream>

RegressionModel::RegressionModel()
{
    mWasEvaluation = false;
    //blue because that's just how I'm feelin'
    //...not really it just doesn't matter
    mFinalComponent.reset(new LinearRegression(-100000, 100000, ModelComponent::BLUE));
    mFinalPCA.reset(new LinearRegression(-100000, 100000, ModelComponent::BLUE));
    //mFinalComponent = new ExponentialRegression(-10000, 10000, ModelComponent::BLUE);
    mHasCircle = false;
    mPCAdone = false;
}

//evaluate a test sample, return the estimation
float RegressionModel::evaluate(std::vector<cv::SerializableScalar> colors)
{
    mWasEvaluation = true;
    mCalibrationToGraph = -1;
    mRawEvaluationData = colors;
    runModel(colors);
    cv::Mat weights = getModelWeights();
    mLastEvaluation = evaluateUnknown(weights);
    cv::Mat weightsPCA = runPCA(weights);
    std::cout << "PCA evaluation: " << evaluatePCA(weightsPCA);
    return mLastEvaluation;
}

//add a calibration point to the model
void RegressionModel::calibrate(std::vector<cv::SerializableScalar> colors,
               float calibrationValue)
{
    mWasEvaluation = false;
    //push back the color data to be stored in the database
    mRawCalibrationData.push_back(colors);
    //run the model components over the new color data
    runModel(colors);

    //std::cout << "CALIBRATING: " << mComponents[0]->getWeight() << std::endl;

    //grab the weights returned by the components
    cv::Mat weights = getModelWeights();
    //slap in the y value given by the user
    weights.at<float>(0) = calibrationValue;
    //push this back to the set of total calibration data
    mCalibrationData.push_back(weights);

    //do the planar evaluation with the final component
    mFinalComponent->evaluate(mCalibrationData);

    //std::cout << "CALIBRATE THIS: " << mCalibrationData << std::endl;

    mFinalWeights = mFinalComponent->mWeights;

    //calculate a new PCA transformation
    createPCATransform();

    //do a linear regression on the PCA'd data
    if(mPCAdone)
    {
        mFinalPCA->evaluate(runPCA(mCalibrationData));
        mPCAWeights = mFinalPCA->mWeights;
    }

    //set the calibration just done as the one to graph
    mCalibrationToGraph = mCalibrationData.size().height - 1;
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

void RegressionModel::createPCATransform()
{
    //std::cout << "Creating PCA transform\n";

    mPCAdone = false;

    if(mCalibrationData.size().height >= 2)
    {
        //return one principal components
        //this is easy to graph on the iphone, and won't affect models using only
        //a single component
        cv::Mat dataMinusYVals = stripFirstCol(mCalibrationData);

        //std::cout << "actually doing PCA fn\n";

        mPCA = cv::PCA(dataMinusYVals, cv::Mat(), CV_PCA_DATA_AS_ROW, 1);
        mPCAdone = true;
    }
}

cv::Mat RegressionModel::runPCA(cv::Mat data)
{
    //std::cout << "running PCA transform\n";

    cv::Mat newData = stripFirstCol(data);
    if(mPCAdone)
    {
        cv::Mat result = mPCA.project(newData);
        //std::cout << "projected!\n";
        cv::Mat result2(result.size().height, result.size().width+1, CV_32F);

        //std::cout << "data height: " << data.size().height << "\n";
        //std::cout << "result height: " << result.size().height << "\n";
        //std::cout << "result width: " << result.size().width << "\n";


        data.col(0).copyTo(result2.col(0));
        //std::cout << "copied first column\n";
        //should only be one more column after pca
        result.col(0).copyTo(result2.col(1));
        return result2;
    }
    return data;
}

cv::Mat RegressionModel::stripFirstCol(cv::Mat data)
{
    std::cout << "stripping column\n";

    //std::cout << "data width: " << data.size().width << "\n";

    cv::Mat dataMinusYVals(data.size().height, data.size().width - 1, CV_32F);
    
    //std::cout << "dmy width: " << dataMinusYVals.size().width << "\n";

    for(int c = 1; c < data.size().width; c++)
    {
        //std::cout << c << std::endl;
        data.col(c).copyTo(dataMinusYVals.col(c-1));
    }
    //std::cout << "done\n";
    return dataMinusYVals;
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

//data grabbing functions to get the raw data from the last run (calibration or evaluation)
float RegressionModel::getRed(int second)
{
    if(mWasEvaluation)
        return getDataPoint(second, mRed, &mRawEvaluationData);
    else
        return getDataPoint(second, mRed, &mRawCalibrationData[mCalibrationToGraph]);
}

float RegressionModel::getGreen(int second)
{
    if(mWasEvaluation)
        return getDataPoint(second, mGreen, &mRawEvaluationData);
    else
        return getDataPoint(second, mGreen, &mRawCalibrationData[mCalibrationToGraph]);
}

float RegressionModel::getBlue(int second)
{
    if(mWasEvaluation)
        return getDataPoint(second, mBlue, &mRawEvaluationData);
    else
        return getDataPoint(second, mBlue, &mRawCalibrationData[mCalibrationToGraph]);
}

float RegressionModel::getRegressionPoint(int component, int second)
{
    if(mComponents.size() > component)
        return mComponents[component]->graphPoint(second);
    return 0;
}

//binary search for value near index on given column of matrix
float RegressionModel::getDataPoint(int index, int column, std::vector<cv::SerializableScalar>* pvec)
{
    int a = 0;
    int b = pvec->size();
    int in = pvec->size() / 2;
    while(fabs(index - (*pvec)[in][mTime]) > .01)//increase this up to .5 to slightly improve performance when graphing if needed
    {
        if((*pvec)[in][mTime] > index)
            b = in;
        else if((*pvec)[in][mTime] <= index)
            a = in;
        in = a + (b-a)/2;
        if(a == in || b == in)
            break;
    }
    std::cout << (*pvec)[in][mTime] << " " << abs(index - (*pvec)[in][mTime]) << " ";
    return (*pvec)[in][column];
}

int RegressionModel::getGraphedCalibration()
{
    return mCalibrationToGraph;
}

//get the calibration value used for a calibration run (eg mgdL)
float RegressionModel::getCalibrationConcentration(int run)
{
    return mCalibrationData.row(run).at<float>(0);
}

//set the statistical data and graph output to a certain calibration run
void RegressionModel::setStatsForCalibration(int run)
{
    if(run < mRawCalibrationData.size())
    {
        runModel(mRawCalibrationData[run]);
        mCalibrationToGraph = run;
    }
}


//matrix of raw model output from calibrations
cv::Mat RegressionModel::getRawCalData()
{
    return mCalibrationData;
}

//unique model name
std::string RegressionModel::getModelName()
{
    return mModelName;
}

//name of substance being tested
std::string RegressionModel::getTestName()
{
    return mTestName;
}

std::string RegressionModel::getStatData()
{
    std::string data;
    for(int c = 0; c < mComponents.size(); c++)
    {
        data.append(mComponents[c]->getStatString());
    }
    return data;
}

void RegressionModel::setCircle(float center, float radius)
{
    if(mHasCircle)
        return;
    mCircleCenter = center;
    mCircleRadius = radius;
    mHasCircle = true;
}


float RegressionModel::getCircleCenter()
{
    if(!mHasCircle)
        return -1;
    return mCircleCenter;
}

float RegressionModel::getCircleRadius()
{
    if(!mHasCircle)
        return -1;
    return mCircleRadius;
}

//have at least two calibration runs been done?
bool RegressionModel::isCalibrated()
{
    return mCalibrationData.size().height > 1;
}

int RegressionModel::queryNumComponents()
{
    return mComponents.size();
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
void RegressionModel::pushComponent(ComponentPtr ptr)
{
    mComponents.push_back(ptr);
}

//private unknown evaluation
float RegressionModel::evaluateUnknown(cv::Mat weights)
{
    std::cout << mFinalWeights << "\n" << weights << "\n";
    cv::Mat result = mFinalWeights.t() * weights.t();

    return result.at<float>(0);
}

//private evaluation using PCA
float RegressionModel::evaluatePCA(cv::Mat weights)
{
    std::cout << mPCAWeights << "\n" << weights << "\n";
    cv::Mat result = mPCAWeights.t() * weights.t();

    return result.at<float>(0);
}

void RegressionModel::runModel(std::vector<cv::SerializableScalar> colors)
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
    //std::cout << "Getting Weights: " << weights << std::endl;
    return weights;
}

//throw away last calibration run
void RegressionModel::chuckCalibration(int run)
{
    cv::Mat temp(0, mCalibrationData.size().width, CV_32F);
    for(int c = 0; c < mCalibrationData.size().height; c++)
    {
        if(c != run)
            temp.push_back(mCalibrationData.row(c));
    }
    mCalibrationData = temp;
    mRawCalibrationData.erase(mRawCalibrationData.begin() + run);
    if(mCalibrationData.size().height > 0)
        dryCalibrate();
}







