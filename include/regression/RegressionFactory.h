#ifndef _REGRESSION_FACTORY_H
#define _REGRESSION_FACTORY_H

#include "regression/RegressionModel.h"
#include "regression/ModelComponent.h"


class RegressionFactory{
public:

    ModelPtr deserializeFromDB(const void* blob, unsigned int len);

    void serializeToDB(ModelPtr model, const void* &blob, unsigned int &len);

    void createNew(std::string modelName, std::string testName);

    void addNewComponent(ModelComponent::ModelType type,
                         float begin, float end,
                         ModelComponent::VariableType var);

    ModelPtr getCreatedModel();
private:
    ModelPtr mTempModel;

    std::string mOutstring;
};

#endif
