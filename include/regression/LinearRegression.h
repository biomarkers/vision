#ifndef __LIN_REG_H__
#define __LIN_REG_H__

#include "regression/ModelComponent.h"

//forward declaring serialization class
namespace boost{
namespace serialization{
class access;
}
}

class LinearRegression : public ModelComponent
{
public:
    LinearRegression(float begin, float end, ModelComponent::VariableType variable);
    virtual void evaluate(cv::Mat x);
    virtual float getWeight();
    virtual ModelType getModelType();
    virtual std::string getStatString();
private:
    virtual float graphPoint(int second);
    friend class boost::serialization::access;
    LinearRegression() {}

    template<typename Archive>
    void serialize(Archive& ar, const unsigned version)
    {
        (void)version;
        ar & boost::serialization::base_object<ModelComponent>(*this);
    }
};

BOOST_CLASS_EXPORT_KEY(LinearRegression);

#endif
