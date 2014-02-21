#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>

#include "../include/RegressionFactory.h"
#include "../include/RegressionModel.h"
#include "../include/ModelComponent.h"
#include "../include/ModelComponents.h"

#include <iostream>
#include <fstream>

BOOST_CLASS_EXPORT_IMPLEMENT(LinearRegression);
BOOST_CLASS_EXPORT_IMPLEMENT(ExponentialRegression);
BOOST_CLASS_EXPORT_IMPLEMENT(PointAnalysis);

ModelPtr RegressionFactory::deserializeFromDB(const void* str, unsigned int len)
{
    ModelPtr model;

    char* instr = new char[len];
    memcpy(instr, str, len);
    std::string instring;
    instring.append(instr, len);
    std::istringstream ggre;
    ggre.str(instring);

    boost::archive::binary_iarchive ar(ggre);
    ar & model;

    free(instr);

    return model;
}

void RegressionFactory::serializeToDB(ModelPtr model, const void* &blob, unsigned int &len)
{
    std::ostringstream ggnore;

    boost::archive::binary_oarchive ar(ggnore);
    ar & model;

    mOutstring = ggnore.str();
    blob = mOutstring.c_str();
    len = mOutstring.length();
}


void RegressionFactory::createNew(std::string modelName, std::string testName)
{
    mTempModel.reset(new RegressionModel());
    mTempModel->mModelName = modelName;
    mTempModel->mTestName = testName;
}

void RegressionFactory::addNewComponent(ModelComponent::ModelType type,
                     float begin, float end,
                     ModelComponent::VariableType var)
{
    if(!mTempModel)
    {
        std::cout << "crud, no model\n";
        return;
    }

    ComponentPtr comp;
    switch(type)
    {
    case ModelComponent::LINEAR:
        comp.reset(new LinearRegression(begin, end, var));
        break;
    case ModelComponent::EXPONENTIAL:
        comp.reset(new ExponentialRegression(begin, end, var));
        break;
    case ModelComponent::POINT:
        comp.reset(new PointAnalysis(begin, end, var));
        break;
    default:
        break;
    }
    if(comp)
        mTempModel->pushComponent(comp);
    else
        std::cout << "Component not created\n\n";
}

ModelPtr RegressionFactory::getCreatedModel()
{
    return mTempModel;
}
