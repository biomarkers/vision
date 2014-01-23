#ifndef _REGRESSION_MODEL_H
#define _REGRESSION_MODEL_H

#include "../include/ModelComponent.h"
#include <stdlib>
#include <vector>
#include <string>

//forward declare a friend
class RegressionFactory;

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
    float evaluate(std::vector<cv::Scalar> colors, int time,
                  int hue, int saturation, int lightness);

    void calibrate(std::vector<cv::Scalar> colors, int time,
                   int hue, int saturation, int lightness,
                   float calibrationValue);

    //number of calibration runs stored
    int getNumCalibrations();

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
    RegressionModel(std::string name);

    //private pusher for model components
    void pushComponent(ModelComponent* ptr);

    //private unknown evaluation
    float evaluateUnknown(cv::Mat weights);

    //list of the components that make up the model
    std::vector<ModelComponent*> mComponents;

    //calibration test outcomes, in raw data format
    //rows of form [y, w1, w2, w3...]
    //where wi is the regression output from the ith model component
    cv::Mat mCalibrationData;

    //final regression weights for use in the evaluation of an unknown
    cv::Scalar mFinalWeights;

    //name of the model, eg. "Chad's Baller Glucose Model"
    std::string mModelName;

    //name of test being performed, eg. "Glucose"
    std::string mTestName;

};

#endif


