#ifndef __QUAD_REG_H__
#define __QUAD_REG_H__

#include "regression/ModelComponent.h"

//forward declaring serialization class
namespace boost{
namespace serialization{
class access;
}
}

/*
 * In all technicality this isn't really a quadratic regression
 * We're just taking the sqaure root of the data before running it through a
 * linear regression model. This should work for the purposes of this app...
 */

class QuadraticRegression : public ModelComponent
{
public:
    QuadraticRegression(float begin, float end, ModelComponent::VariableType variable);
    virtual void evaluate(cv::Mat x);
    virtual float getEstimation(cv::Mat x);
    virtual float getWeight();
    virtual ModelType getModelType();
    virtual std::string getStatString();
private:
    virtual float graphPoint(int second);
    friend class boost::serialization::access;
    QuadraticRegression() {}

    cv::Mat squareRoot(cv::Mat x);
    ComponentPtr mLinearComponent;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned version)
    {
        (void)version;
        ar & boost::serialization::base_object<ModelComponent>(*this) & mLinearComponent;
    }
};

BOOST_CLASS_EXPORT_KEY(QuadraticRegression);

#endif
