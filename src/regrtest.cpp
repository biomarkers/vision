#include "../include/RegressionFactory.h"
#include "../include/RegressionModel.h"
//#include "../include/BiomarkerImageProcessor.h"

#include <iostream>
#include <string>
#include <fstream>

//read a csv file and return it in a vector of scalars
std::vector<cv::Scalar> readcsv(std::string fname)
{
    //Open up that file bor
    std::ifstream in;
    in.open(fname.c_str(), std::ifstream::in);
    //make sure it exists and hardcore crash if it doesn't
    if(!in.good())
        exit(-1);

    //We need to figure out the number of elements per row
    int cols = 0;
    std::string temp;

    //so read it in and count the commas
    getline(in, temp);
    //assume no comma at the end of line
    cols = std::count(temp.begin(), temp.end(), ',') + 1;

    //vector to fill
    std::vector<cv::Scalar> M;
    //temp scalar to hold each row as we read the file
    cv::Scalar Y(0.f, 0.f, 0.f, 0.f);

    //ghetto re-seek to top of the file
    in.close();
    in.open(fname.c_str(), std::ifstream::in);

    char delim;
    float tmp;

    while(!in.eof())
    {
        for(int i = 0; i < cols; i++)
        {
            in >> tmp;
            Y[i] = tmp;
            if(i < cols-1)
                in >> delim;
        }
        if(!in.eof()) //push the new row if we didn't run into the end of file during reading
            M.push_back(Y);
    }
    return M;
}

int main(int argc, char** argv)
{
    std::cout << "Jollies!\n";

    ModelPtr model;

    RegressionFactory factory;

    factory.createNew("Joe's test", "Glucose");

    //sample model ideas
    //factory.addNewComponent(ModelComponent::LINEAR, 0, 1000, ModelComponent::GREEN);
    factory.addNewComponent(ModelComponent::LINEAR, 0, 9789, ModelComponent::RED);
    std::cout << "runtime " << factory.getCreatedModel()->getModelRunTime() << std::endl;
    factory.addNewComponent(ModelComponent::POINT, 8500, 9790, ModelComponent::RED);
    std::cout << "runtime " << factory.getCreatedModel()->getModelRunTime() << std::endl;
    //factory.addNewComponent(ModelComponent::POINT, 0, 9790, ModelComponent::RED);
    //factory.addNewComponent(ModelComponent::EXPONENTIAL, 0, 9790, ModelComponent::BLUE);
    //factory.addNewComponent(ModelComponent::EXPONENTIAL, 1, 400, ModelComponent::HUE);

    model = factory.getCreatedModel();

    model->setIndices(3,2,1,0,-1);

    std::vector<cv::Scalar> colors;
    std::vector<cv::Scalar> colors1;
    std::vector<cv::Scalar> colors2;
    std::vector<cv::Scalar> colors3;
    std::vector<cv::Scalar> colors4;
    std::vector<cv::Scalar> colors5;

    colors = readcsv("etc/40mg.csv");
    model->calibrate(colors, 40);
    colors1 = readcsv("etc/100mg.csv");
    model->calibrate(colors1, 100);
    colors2 = readcsv("etc/200mg.csv");
    model->calibrate(colors2, 200);
    colors3 = readcsv("etc/300mg.csv");
    model->calibrate(colors3, 300);
    colors4 = readcsv("etc/400mg.csv");
    model->calibrate(colors4, 400);
    colors5 = readcsv("etc/unknown.csv");

    float result = model->evaluate(colors);
    std::cout << "\n---------------------\nResult: " << result
              << "\n---------------------\n";
    result = model->evaluate(colors1);
    std::cout << "\n---------------------\nResult: " << result
              << "\n---------------------\n";
    result = model->evaluate(colors2);
    std::cout << "\n---------------------\nResult: " << result
              << "\n---------------------\n";
    result = model->evaluate(colors3);
    std::cout << "\n---------------------\nResult: " << result
              << "\n---------------------\n";
    result = model->evaluate(colors4);
    std::cout << "\n---------------------\nResult: " << result
              << "\n---------------------\n";
    result = model->evaluate(colors5);
    std::cout << "\n---------------------\nResult: " << result
              << "\n---------------------\n";

    cv::Mat shit = model->getRawCalData();
    std::cout << shit << "\n\n";

}
