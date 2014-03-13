#ifndef _REGRESSION_MODEL_H
#define _REGRESSION_MODEL_H

#include "../include/ModelComponent.h" //will need this later
                                         //for smart pointers
#include "../include/SerializableScalar.h"
#include "../include/SerializableMat.h"

#include <opencv2/core/core.hpp>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

//forward declaring serialization class
namespace boost{
namespace serialization{
class access;
}
}

//forward declare a friend
class RegressionFactory;

//forward declare the damn class to typedef a pointer for it
class RegressionModel;
//typedef a shared ptr for this class
typedef boost::shared_ptr<RegressionModel> ModelPtr;

class RegressionModel{
public:
    friend class RegressionFactory;

    //evaluate a test sample, return the estimation
    float evaluate(std::vector<cv::SerializableScalar> colors);

    //add a calibration point to the model
    void calibrate(std::vector<cv::SerializableScalar> colors,
                   float calibrationValue);

    //re-run the calibration without any new data
    void dryCalibrate();

    //super secret function, don't touch!
    void superSecretCalibrationOverride(cv::Mat newCalibration);

    //announce the indices of incoming matrices. Use -1 for not present
    void setIndices(int time, int red, int green, int blue, int hue);

    //number of calibration runs stored
    int getNumCalibrations();

    ///grab the last frame we need to look at
    int getModelRunTime();

    //data grabbing functions to get the raw data from the last run (calibration or evaluation)
    float getRed(int second);
    float getGreen(int second);
    float getBlue(int second);
    //get a point to graph for the last regression run on the given component
    float getRegressionPoint(int component, int second);

    //get the calibration value used for a calibration run (eg mgdL)
    float getCalibrationConcentration(int run);

    //set the statistical data to a certain calibration run
    void setStatsForCalibration(int run);

    //matrix of raw model output from calibrations
    cv::Mat getRawCalData();

    //unique model name
    std::string getModelName();

    //name of substance being tested
    std::string getTestName();

    //get string with statistical data about last test run
    std::string getStatData();

    //throw away a calibration run
    void chuckCalibration(int run);

    //have at least two calibration runs been done?
    bool isCalibrated();

    //query the important UI things about each model component
    int queryNumComponents();
    int queryBegin(int component);
    int queryEnd(int component);
    ModelComponent::VariableType queryVariable(int component);
    ModelComponent::ModelType queryModelType(int component);

private:
    //private constructor, cannot build outside the factory
    RegressionModel();

    //private pusher for model components
    void pushComponent(ComponentPtr ptr);

    //private unknown evaluation
    float evaluateUnknown(cv::Mat weights);

    //run the model
    void runModel(std::vector<cv::SerializableScalar> colors);

    //get a row vector of run results
    cv::Mat getModelWeights();

    //binary search for value near index on given column of matrix
    float getDataPoint(int index, int column, std::vector<cv::SerializableScalar>* pvec);

    //positions of variables in input matrices
    int mRed, mGreen, mBlue, mHue, mTime;

    //list of the components that make up the model
    std::vector<ComponentPtr> mComponents;

    //linear regression model for the final weights
    ComponentPtr mFinalComponent;

    //calibration test outcomes, in raw data format
    //rows of form [y, w1, w2, w3...]
    //where wi is the regression output from the ith model component
    cv::SerializableMat mCalibrationData;

    //raw data of calibration tests, so shit can be graphed later on yo
    std::vector<std::vector<cv::SerializableScalar> > mRawCalibrationData;

    //was the last run a calibration or an evaluation?
    bool mWasEvaluation;

    //raw data for last evaluation
    std::vector<cv::SerializableScalar> mRawEvaluationData;

    //final regression weights for use in the evaluation of an unknown
    cv::SerializableMat mFinalWeights;

    //name of the model, eg. "Chad's Baller Glucose Model"
    std::string mModelName;

    //name of test being performed, eg. "Glucose"
    std::string mTestName;

    //allow serialization
    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned version)
    {
        std::cout << "archiving!\n\n";
        (void)version;
        ar & mRed & mGreen & mBlue & mHue & mTime & mComponents & mFinalComponent & mCalibrationData & mRawCalibrationData & mWasEvaluation & mRawEvaluationData & mFinalWeights & mModelName & mTestName;
    }

};

#endif


