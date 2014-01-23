#ifndef _MODEL_COMPONENT_H
#define _MODEL_COMPONENT_H

#include <math.h>
#include <opencv2/core/core.hpp>

//forward declaration of friend
class RegressionModel;

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
    virtual float getWeight();

    //enumerate the types of model components available
    enum ModelType{
        POINT = 0,
        LINEAR,
        EXPONENTIAL
    };

    ModelComponent(float begin, float end);

    //add a friend to look at our privates, lulz
    friend class RegressionModel;
private:
    //region of frames to analyze between
    float mBegin;
    float mEnd;

    //weights
    cv::Mat mWeights;

    //private getter for friends only
    float getBegin();
    float getEnd();

    //helper function to strip off matrix entries not within
    //begin -> end
    cv::Mat cutToSize(cv::Mat x);
};


class LinearRegression
{
public:
    LinearRegression(float begin, float end);
    virtual void evaluate(cv::Mat x);
    virtual float getWeight();
private:

};

class PointAnalysis
{
public:
    PointAnalysis(float begin, float end);
    virtual void evaluate(cv::Mat x);
    virtual float getWeight();
private:

};

class ExponentialRegression
{
public:
    PointAnalysis(float begin, float end);
    virtual void evaluate(cv::Mat x);
    virtual float getWeight();
private:

};


#endif
