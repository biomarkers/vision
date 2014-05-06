#ifndef _REGRESSION_MODEL_H
#define _REGRESSION_MODEL_H

#include "regression/ModelComponent.h" //will need this later
                                         //for smart pointers
#include "serialization/SerializableScalar.h"
#include "serialization/SerializableMat.h"

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

    //grab the last frame we need to look at
    int getModelRunTime();

    enum RegressionType{
        PLANAR = 0,
        PCA_LINEAR,
        PCA_QUADRATIC,
        PCA_EXPONENTIAL,
        INVALID_TYPE
    };

    //get and set the regression type
    RegressionType getCurrentRegressionType();
    void setRegressionType(RegressionType type);

    /*
        Grab lines for the graphs of each calibration/evaluation run. These functions operate
        on either the calibration given in mCalibrationToGraph, or the last evaluation run
    */

    //data grabbing functions to get the raw rgb data from the last run (calibration or evaluation)
    float getRed(int second);
    float getGreen(int second);
    float getBlue(int second);
    //get a point to graph for the last regression run on the given component,
    float getRegressionPoint(int component, int second);

    /*
        Grab points for final regression graphs. These are all done in PCA space.
        Possible graphs are:
            Planar regression
            PCA Linear regression
            PCA Exponential regression
            PCA Quadratic regression
    */

    //get the x range of the PCA space
    void getPCASpaceRange(float &left, float &right);
    //here are the points for the calibration values that we're performing regression on
    void getCalibrationPointPostPCA(int index, float &xval, float &yval);
    //get a point to graph for the regression type mFinalRegressionType in PCA space
    float getFinalRegressionLine(int PCAindex);


    //get currently graphed calibration run
    int getGraphedCalibration();

    //get the calibration value used for a calibration run (eg mgdL)
    float getCalibrationConcentration(int run);

    //set the statistical data and graph output to a certain calibration run
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

    //set the circle center and size
    void setCircle(float centerx, float centery, float radius);

    //get the circle center and size
    float getCircleCenterX();
    float getCircleCenterY();
    float getCircleRadius();

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
    //runs weights through the regression given by
    //mFinalRegressionType
    float evaluateUnknown(cv::Mat weights);

    //run the model
    void runModel(std::vector<cv::SerializableScalar> colors);

    //get a row vector of run results
    cv::Mat getModelWeights();

    //binary search for value near index on given column of matrix
    float getDataPoint(int index, int column, std::vector<cv::SerializableScalar>* pvec);

    //create the PCA transformation
    void createPCATransform();

    //run the PCA transformation
    cv::Mat runPCA(cv::Mat data);

    //strip the first column off a matrix
    cv::Mat stripFirstCol(cv::Mat data);

    //positions of variables in input matrices
    int mRed, mGreen, mBlue, mHue, mTime;

    //list of the components that make up the model
    std::vector<ComponentPtr> mComponents;

    /*
        Regression Components for the final model
    */
    //n-planar regression
    ComponentPtr mFinalComponent;
    //PCA linear fit
    ComponentPtr mFinalPCALinear;
    //PCA quadratic fit
    ComponentPtr mFinalPCAQuad;
    //PCA LM fit
    ComponentPtr mFinalPCAExponential;

    //which component will be used for the final regression?
    RegressionType mFinalRegressionType;

    //PCA object for the transformation to PCA space
    cv::PCA mPCA;
    //has the PCA transform been built?
    bool mPCAdone;

    //calibration test outcomes, in matrix format
    //rows of form [y, w1, w2, w3...]
    //where wi is the regression output from the ith model component
    cv::SerializableMat mCalibrationData;
    //calibration test outcomes in PCA space
    //rows of [y, x]
    cv::SerializableMat mPCACalibrationData;

    //raw color data of calibration tests, so shit can be graphed later on yo
    std::vector<std::vector<cv::SerializableScalar> > mRawCalibrationData;

    //was the last run a calibration or an evaluation?
    bool mWasEvaluation;
    //if calibration, which one are we graphing?
    int mCalibrationToGraph;

    //raw rgb data for last evaluation
    std::vector<cv::SerializableScalar> mRawEvaluationData;

    //name of the model, eg. "Chad's Baller Glucose Model"
    std::string mModelName;

    //name of test being performed, eg. "Glucose"
    std::string mTestName;

    //center of circle of interest
    float mCircleCenterX;
    float mCircleCenterY;

    //radius of circle of interest
    float mCircleRadius;

    //is the circle set?
    float mHasCircle;

    //last result from evaluation
    float mLastEvaluation;

    //allow serialization
    friend class boost::serialization::access;
    //allow data exporting
    friend class DataExporter;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned version)
    {
        std::cout << "archiving!\n\n";
        (void)version;
        ar & mRed & mGreen & mBlue & mHue & mTime & mComponents & mFinalComponent & mFinalPCALinear
                & mFinalPCAQuad & mFinalPCAExponential & mCalibrationData & mRawCalibrationData & mWasEvaluation
                & mCalibrationToGraph & mRawEvaluationData & mModelName & mFinalRegressionType
                & mTestName & mCircleCenterX & mCircleCenterY & mCircleRadius & mHasCircle & mLastEvaluation;
        //PCA data cannot be serialized without some wrapper, so screw it we'll just recalculate it here
        //will restore mPCAdone as well
        createPCATransform();
    }

};

#endif


