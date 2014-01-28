#include "../include/RegressionFactory.h"
#include "../include/RegressionModel.h"
#include "../include/ModelComponent.h"

#include <iostream>

RegressionModel* RegressionFactory::loadFromFile(std::string filename)
{
    std::cout << "Functionality not implemented\n";
    (void)filename;
    return 0;
}

void RegressionFactory::createNew(std::string modelName, std::string testName)
{
    mTempModel = new RegressionModel();
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

    ModelComponent* comp;
    switch(type)
    {
    case ModelComponent::LINEAR:
        comp = new LinearRegression(begin, end, var);
        break;
    case ModelComponent::EXPONENTIAL:
        comp = new ExponentialRegression(begin, end, var);
        break;
    case ModelComponent::POINT:
        comp = new PointAnalysis(begin, end, var);
        break;
    }
    if(comp)
        mTempModel->pushComponent(comp);
    else
        std::cout << "Component not created\n\n";
}

RegressionModel* RegressionFactory::getCreatedModel()
{
    return mTempModel;
}
