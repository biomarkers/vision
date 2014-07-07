
/* ========================================================================
 *  Copyright 2014 Kyle Cesare, Kevin Hess, Joe Runde, Chadd Armstrong, Chris Heist
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 * ========================================================================
 */

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
    virtual std::string getStatString(bool printBounds);
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
