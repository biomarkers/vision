#ifndef _REGRESSION_FACTORY_H
#define _REGRESSION_FACTORY_H

#include "../include/RegressionModel.h"
#include "../include/ModelComponent.h"


class RegressionFactory{
public:
    ModelPtr loadFromFile(std::string filename);

    void createNew(std::string modelName, std::string testName);

    void addNewComponent(ModelComponent::ModelType type,
                         float begin, float end,
                         ModelComponent::VariableType var);

    ModelPtr getCreatedModel();
private:
    ModelPtr mTempModel;
};

#endif
