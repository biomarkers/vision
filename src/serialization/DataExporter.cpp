#include "serialization/DataExporter.h"


DataExporter::DataExporter(ModelPtr model):
    //mPath(path),
    mModel(model)//,
    //mCSVFileCurrent(false),
    //mTextFileCurrent(false)
{
}

void DataExporter::exportCalibration()
{
    //initialize strings
    createCSVData();
    createTextData();

    for(int c = 0; c < mModel->getNumCalibrations(); c++)
    {
        appendtoCSV(c, mModel->getCalibrationConcentration(c), mModel->mRawCalibrationData[c]);
        mModel->setStatsForCalibration(c);

        std::ostringstream stream;
        if(c != 0)
            stream << "\n\n";
        stream << "Calibration #" << c << "\n";
        stream << "Value: " << mModel->getCalibrationConcentration(c) << "\n\n";
        appendtoText(stream.str());
        appendtoText(mModel->getStatData());
    }
}

void DataExporter::exportDiagnosticRun()
{
    createCSVData();
    createTextData();

    appendtoCSV(-1, -1, mModel->mRawEvaluationData);
    appendtoText("Diagnostic run:\n\n");
    appendtoText(mModel->getStatData());

    std::ostringstream stream;
    stream << "Final evaluation: " << mModel->mLastEvaluation;
    appendtoText(stream.str());
}

/*
std::string DataExporter::getCSVFilename()
{
    return createCSVFile();
}

std::string DataExporter::getTextFilename()
{
    return createTextFile();
}
*/

std::string DataExporter::getCSVData()
{
    return mCSVData;
}

std::string DataExporter::getTextData()
{
    return mTextData;
}

/*
std::string DataExporter::createCSVFile()
{
    if(!mCSVFileCurrent)
    {

    }
    mCSVFileCurrent = true;
}

std::string DataExporter::createTextFile()
{
    if(!mTextFileCurrent)
    {

    }
    mTextFileCurrent = true;
}
*/

void DataExporter::createCSVData()
{
    mCSVData.clear();
    std::ostringstream stream;
    stream << "Run #, calibration value, ";
    int red, green, blue, hue, time;
    red = mModel->mRed;
    green = mModel->mGreen;
    blue = mModel->mBlue;
    hue = mModel->mHue;
    time = mModel->mTime;

    for(int c = 0; c < 4; c++)
    {
        if(red == c)
            stream << "Red";
        else if(green == c)
            stream << "Green";
        else if(blue == c)
            stream << "Blue";
        else if(hue == c)
            stream << "Hue";
        else if(time == c)
            stream << "Time";
        else
            stream << "?";

        if(c < 3)
            stream << ", ";
    }
    
    stream << "\n";

    mCSVData.append(stream.str());
    //mCSVFileCurrent = false;
}

void DataExporter::createTextData()
{
    mTextData.clear();
    //mTextFileCurrent = false;
}

void DataExporter::appendtoCSV(int run, float calibrationValue, std::vector<cv::SerializableScalar> data)
{
    //mCSVFileCurrent = false;
    std::ostringstream stream;
    for(int c = 0; c < data.size(); c++)
    {
        stream << run << "," << calibrationValue << "," << data[c][0] << "," << data[c][1]
               << "," << data[c][2] << "," << data[c][3] << "\n";
    }
    mCSVData.append(stream.str());
}

void DataExporter::appendtoText(std::string line)
{
    //mTextFileCurrent = false;
    mTextData.append(line);
}
