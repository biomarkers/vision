#ifndef __DATA_EXPORTER_H
#define __DATA_EXPORTER_H

#include "regression/RegressionModel.h"

class DataExporter{
public:
    DataExporter(ModelPtr model);

    void exportCalibration();
    void exportDiagnosticRun();

    //std::string getCSVFilename();
    //std::string getTextFilename();

    std::string getCSVData();
    std::string getTextData();

private:
    //std::string mPath;
    ModelPtr mModel;

    std::string mCSVData;
    std::string mTextData;

    //bool mCSVFileCurrent;
    //bool mTextFileCurrent;

    //initialize the strings for csv and text files
    void createCSVData();
    void createTextData();

    //create files and return their names
    //std::string createCSVFile();
    //std::string createTextFile();

    void appendtoCSV(int run, float calibrationValue, std::vector<cv::SerializableScalar> data);
    void appendtoText(std::string line);

};


#endif
