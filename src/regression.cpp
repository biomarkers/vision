#include <cstdio>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <string>
#include <fstream>

//append crap to a CSV
void appendToCSV(cv::Mat M, std::string fname)
{
    std::ofstream fout;
    fout.open(fname.c_str(), std::ofstream::out | std::ofstream::app);

    for(int c = 0; c < M.size().height; c++)
    {
        for(int i = 0; i < M.size().width; i++)
        {
            fout << M.row(c).at<float>(i);
            if(i < M.size().width - 1)
                fout << ",";
        }
        fout << std::endl;
    }
}

//read a csv file and return it in a float32 matrix
cv::Mat readcsv(std::string fname)
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

    //matrix to fill
    cv::Mat M(0, cols, CV_32F);
    //temp matrix to hold each row as we read the file
    cv::Mat Y(1, cols, CV_32F);

    //ghetto ass re-seek to top of the file
    in.close();
    in.open(fname.c_str(), std::ifstream::in);

    char delim;
    float tmp;

    while(!in.eof())
    {
        for(int i = 0; i < cols; i++)
        {
            in >> tmp;
            Y.row(0).col(i) = tmp;
            if(i < cols-1)
                in >> delim;
        }
        M.push_back(Y);
    }
    return M;
}

//Take a matrix with rows in [y, x1, x2, x3...] form, and output linear
//regression result [b, w1, w2, w3...]
cv::Mat linearRegression(cv::Mat M)
{
    cv::Mat W(1, M.size().width, CV_32F);
    cv::Mat Y(M.size().height, 1, CV_32F);
    cv::Mat X(M.size().height, M.size().width, CV_32F, 1.f);

    Y = M.col(0);
    for(int c = 1; c < M.size().width; c++)
    {
        M.col(c).copyTo(X.col(c));
    }

    //w = (x.transpose() * x).inverse() * x.transpose() * y;
    W = (X.t() * X).inv() * X.t() * Y;

    return W;
}

//Take a matrix M of the form [x, y1, y2, y3....]
//and ouput [yi, x]
cv::Mat pickAndInverse(cv::Mat M, int i)
{
    cv::Mat X(M.size().height, 2, CV_32F);
    M.col(i).copyTo(X.col(0));
    M.col(0).copyTo(X.col(1));
    //X.col(0) = M.col(i) + 0;
    //X.col(1) = M.col(0) + 0;
    return X;
}


int main(int argc, char** argv)
{
    //read in the matrix, form [time, B, G, R]
    cv::Mat M = readcsv("rgb.csv");

    //separate into 3 matrices, forms
    //[R, time] [G, time], [B, time]
    cv::Mat R = pickAndInverse(M, 3);
    cv::Mat G = pickAndInverse(M, 2);
    cv::Mat B = pickAndInverse(M, 1);

    //calculate linear regression for each color
    cv::Mat Rs = linearRegression(R);
    cv::Mat Gs = linearRegression(G);
    cv::Mat Bs = linearRegression(B);

    std::cout << "\nRed slope:   " << Rs.row(0).at<float>(1);
    std::cout << "\nGreen slope: " << Gs.row(0).at<float>(1);
    std::cout << "\nBlue slope:  " << Bs.row(0).at<float>(1);

    //the value of whatever we're regressing the slopes against
    int yVal = 0;

    std::cout << "\nEnter calibration: ";
    std::cin >> yVal;

    cv::Mat out(1, 4, CV_32F);
    out.row(0).col(0) = yVal;
    out.row(0).col(1) = Rs.row(0).at<float>(1);;
    out.row(0).col(2) = Gs.row(0).at<float>(1);;
    out.row(0).col(3) = Bs.row(0).at<float>(1);;

    appendToCSV(out, "cal.csv");

    std::cin.get();
    return 0;
}
