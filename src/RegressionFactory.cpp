#include "../include/RegressionFactory.h"
#include "../include/RegressionModel.h"
#include "../include/ModelComponent.h"

#include <iostream>
#include <fstream>

ModelPtr RegressionFactory::loadFromFile(std::string filename)
{
    std::cout << "Functionality not implemented\n";
    std::ifstream ggin;
    ggin.open(filename.c_str());
    if(ggin.is_open())
    {
        std::cout << "Success!";
        ggin.close();
    }
    else
    {
        std::cout << "fuck you kyle\n\n\n\n\n\n\n";
    }

    (void)filename;
    ModelPtr blank;
    return blank;
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
