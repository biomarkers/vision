#ifndef _MODEL_COMPONENT_H
#define _MODEL_COMPONENT_H

#include <opencv2/core/core.hpp>
#include <boost/shared_ptr.hpp>
#include "../include/SerializableMat.h"

//forward declaration of friend
class RegressionModel;

//forward declaring serialization class
namespace boost{
namespace serialization{
class access;
}
}

//abstract interface for a component of a regression model
class ModelComponent
{
public:
    /*
     * concrete classes must implement some evaluation of a
     * regression between mBegin and mEnd
     *
     * x should have the dependent variable in the first column
     * and the independent variable in the remaining columns
     * eg: [y, x]
     */
    virtual void evaluate(cv::Mat x) = 0;

    //concrete classes must implement this-
    //obtain the results of the regression, for now just a float
    virtual float getWeight() = 0;

    //enumerate the types of model components available
    enum ModelType{
        POINT = 0,
        LINEAR,
        EXPONENTIAL,
        INVALID_TYPE
    };

    //enumerate the variable that this component is testing
    enum VariableType{
        RED = 0,
        BLUE,
        GREEN,
        HUE,
        INVALID_VAR
    };

    //generic constructor, fills in the range and value to test
    ModelComponent(float begin, float end, VariableType variable);
    ~ModelComponent();

    //add a friend to look at our privates, lulz
    friend class RegressionModel;
protected:
    //region of frames to analyze between
    float mBegin;
    float mEnd;

    //weights
    cv::SerializableMat mWeights;

    //variable to be tested
    VariableType mVar;

    //private getters for friends only
    float getBegin();
    float getEnd();
    VariableType getVarType();
    virtual ModelType getModelType() = 0;

    //helper function to strip off matrix entries not within
    //range [mBegin, mEnd]
    cv::Mat cutToSize(cv::Mat x);

    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned version)
    {
        (void)version;
        ar & mBegin & mEnd & mWeights & mVar;
    }
};

//typedef a shared ptr for this class
typedef boost::shared_ptr<ModelComponent> ComponentPtr;

class LinearRegression : public ModelComponent
{
public:
    LinearRegression(float begin, float end, ModelComponent::VariableType variable);
    virtual void evaluate(cv::Mat x);
    virtual float getWeight();
    virtual ModelType getModelType();
private:
};

class PointAnalysis : public ModelComponent
{
public:
    PointAnalysis(float begin, float end, ModelComponent::VariableType variable);
    virtual void evaluate(cv::Mat x);
    virtual float getWeight();
    virtual ModelType getModelType();
private:
    float mAvg; //ezmode weight
};

class ExponentialRegression : public ModelComponent
{
public:
    ExponentialRegression(float begin, float end, ModelComponent::VariableType variable);
    virtual void evaluate(cv::Mat x);
    virtual float getWeight();
    virtual ModelType getModelType();
private:
    cv::Mat logMat(cv::Mat x, float percent);
    float mWeight; //more ezmode weights
    float mDisp;
};


#endif
