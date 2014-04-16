#ifndef __EXP_REG_H__
#define __EXP_REG_H__

#include "regression/ModelComponent.h"


//forward declaring serialization class
namespace boost{
namespace serialization{
class access;
}
}

class ExponentialRegression : public ModelComponent
{
public:
    ExponentialRegression(float begin, float end, ModelComponent::VariableType variable);
    virtual void evaluate(cv::Mat x);
    virtual float getWeight();
    virtual ModelType getModelType();
    virtual std::string getStatString();
private:
    virtual float graphPoint(int second);
    ExponentialRegression(){}
    cv::Mat logMat(cv::Mat x, float percent);
    float mWeight; //more ezmode weights
    float mDisp;


    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned version)
    {
        (void)version;
        ar & boost::serialization::base_object<ModelComponent>(*this);
        ar & mWeight & mDisp;
    }
};

BOOST_CLASS_EXPORT_KEY(ExponentialRegression);

#endif
