#ifndef _REGRESSION_MODEL_H
#define _REGRESSION_MODEL_H

//#include "../include/ModelComponent.h" //will need this later
                                         //for smart pointers

#include <opencv2/core/core.hpp>
#include <vector>
#include <string>

//forward declare a friend
class RegressionFactory;

//forward declare components for pointing
//delete this when smart pointers happen
class ModelComponent;

/*
 * For now, DO NOT COPY objects of this class, dumb pointers are used
 *
 *
 * ...srs mens, don't do it
 */

class RegressionModel{
public:
    friend class RegressionFactory;

    //evaluate a test sample, return the estimation
    float evaluate(std::vector<cv::Scalar> colors);

    //add a calibration point to the model
    void calibrate(std::vector<cv::Scalar> colors,
                   float calibrationValue);

    //announce the indices of incoming matrices. Use -1 for not present
    void setIndices(int time, int red, int green, int blue, int hue);

    //number of calibration runs stored
    int getNumCalibrations();

    int getModelRunTime();

    //matrix of raw model output from calibrations
    cv::Mat getRawCalData();

    //unique model name
    std::string GetModelName();

    //name of substance being tested
    std::string GetTestName();

    //have at least two calibration runs been done?
    bool isCalibrated();

private:
    //private constructor, cannot build outside the factory
    RegressionModel();

    //private pusher for model components
    void pushComponent(ModelComponent* ptr);

    //private unknown evaluation
    float evaluateUnknown(cv::Mat weights);

    //run the model
    void runModel(std::vector<cv::Scalar> colors);

    //get a row vector of run results, starting with a 1.0
    cv::Mat getModelWeights();

    //positions of variables in input matrices
    int mRed, mGreen, mBlue, mHue, mTime;

    //list of the components that make up the model
    std::vector<ModelComponent*> mComponents;

    //linear regression model for the final weights
    ModelComponent* mFinalComponent;

    //calibration test outcomes, in raw data format
    //rows of form [y, w1, w2, w3...]
    //where wi is the regression output from the ith model component
    cv::Mat mCalibrationData;

    //final regression weights for use in the evaluation of an unknown
    cv::Mat mFinalWeights;

    //name of the model, eg. "Chad's Baller Glucose Model"
    std::string mModelName;

    //name of test being performed, eg. "Glucose"
    std::string mTestName;

};

#endif


