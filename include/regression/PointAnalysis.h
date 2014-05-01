#ifndef __POINT_REG_H__
#define __POINT_REG_H__

#include "regression/ModelComponent.h"


//forward declaring serialization class
namespace boost{
namespace serialization{
class access;
}
}

class PointAnalysis : public ModelComponent
{
public:
    PointAnalysis(float begin, float end, ModelComponent::VariableType variable);
    virtual void evaluate(cv::Mat x);
    virtual float getEstimation(cv::Mat x);
    virtual float getWeight();
    virtual ModelType getModelType();
    virtual std::string getStatString();
private:
    virtual float graphPoint(int second);
    PointAnalysis(){}
    float mAvg; //ezmode weight

    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned version)
    {
        (void)version;
        ar & boost::serialization::base_object<ModelComponent>(*this);
        ar & mAvg;
    }
};

BOOST_CLASS_EXPORT_KEY(PointAnalysis);

#endif
