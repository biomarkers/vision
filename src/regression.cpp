#include <cstdio>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <string>
#include <fstream>
#include <math.h>

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
            Y.row(0).col(i) = tmp;
            if(i < cols-1)
                in >> delim;
        }
        if(!in.eof()) //push the new row if we didn't run into the end of file during reading
            M.push_back(Y);
    }
    return M;
}


// Take a matrix with rows in [y, x1, x2, x3...] form, where x1, x2, etc are
// separate features, and outputs a regression result of form
// [ constant, a1, b1, ... a2, b2, ..., a3, b3, ... ...]
// where the regression equation is of the form
// y = constant + a1x1 + b2x2 + a2x1^2 + b2x2^2 + a3x1^3 + b3x2^3 ...
cv::Mat genericRegression(cv::Mat M, int power)
{
    int inwidth = M.size().width - 1;
    int width = inwidth * power;

    cv::Mat W(1, width + 1, CV_32F);
    cv::Mat Y(M.size().height, 1, CV_32F);
    cv::Mat X(M.size().height, width + 1, CV_32F, 1.f);

    Y = M.col(0);
    for(int c = 1; c < M.size().width; c++)
    {
        M.col(c).copyTo(X.col(c));
        for(int k = 1; k < power; k++)
        {
            int g = c + k * inwidth;
            M.col(c).copyTo(X.col(g));

            for(int i = 0; i < M.size().height; i++)
            {
                float val = pow(M.col(c).at<float>(i), k+1);  // * M.col(c).at<float>(i);
                X.col(g).at<float>(i) = val;
            }
        }
    }

    W = (X.t() * X).inv() * X.t() * Y;

    return W;

}

//Take a matrix M of the form [x, y1, y2, y3....]
//and ouput [yi, x]
cv::Mat pickAndReverse(cv::Mat M, int i)
{
    cv::Mat X(M.size().height, 2, CV_32F);
    M.col(i).copyTo(X.col(0));
    M.col(3).copyTo(X.col(1));
    return X;
}

//take some asymptotic curve and
float findLastValue(cv::Mat M, float percent)
{


}

cv::Mat normalizeLog(cv::Mat M, float percent)
{
    int count = 0, bad_count = 0;
    float total = 0, avg = 0;

    int height = M.size().height;

    //hack here, fix this up later, we don't want more than 400 frames @ 3/sec
    //approx 2 minutes 13 seconds
    height = 400;

    int end = height * percent;
    for(int c = height-1; c > end; c--)
    {
        count += 1;
        total += M.col(0).at<float>(c);
    }
    avg = total / count;

    cv::Mat out(0,2,CV_32F);

    for(int c = 0; c < height; c++)
    {
        //stuff in here to truncate from time 20 - 400
        if(M.col(1).at<float>(c) < 20)
            continue;
        if(M.col(1).at<float>(c) > 400)
            break;


        float val = M.col(0).at<float>(c) - avg;
        if(val > 0)
        {
            M.col(0).at<float>(c) = log(val);
            out.push_back(M.row(c));
        }
        else
            bad_count++;
    }
    std::cout << "Rows under zero: " << bad_count << std::endl;
    return out;
}

int main(int argc, char** argv)
{
    //read in the matrix, form [time, B, G, R]
    cv::Mat M = readcsv("rgb.csv");

    //separate into 3 matrices, forms
    //[R, time] [G, time], [B, time]
    cv::Mat R = pickAndReverse(M, 2);
    cv::Mat G = pickAndReverse(M, 1);
    cv::Mat B = pickAndReverse(M, 0);

    R = normalizeLog(R, .93);
    G = normalizeLog(G, .93);
    B = normalizeLog(B, .93);

    int order;
    std::cout << "\nEnter order: ";
    std::cin >> order;

    //calculate regression for each color
    cv::Mat Rs = genericRegression(R, order);
    cv::Mat Gs = genericRegression(G, order);
    cv::Mat Bs = genericRegression(B, order);

    cv::Mat Rout(1, 2 + order, CV_32F);
    cv::Mat Gout(1, 2 + order, CV_32F);
    cv::Mat Bout(1, 2 + order, CV_32F);

    for(int c = 0; c <= order; c++)
    {
        std::cout << "\nRed coefficient " << c << "  : " << Rs.row(0).at<float>(c);
        std::cout << "\nGreen coefficient " << c << ": " << Gs.row(0).at<float>(c);
        std::cout << "\nBlue coefficient " << c << " : " << Bs.row(0).at<float>(c);

        Rout.row(0).col(c + 1) = Rs.row(0).at<float>(c);
        Gout.row(0).col(c + 1) = Gs.row(0).at<float>(c);
        Bout.row(0).col(c + 1) = Bs.row(0).at<float>(c);
    }

    //the value of whatever we're regressing the slopes against
    int yVal = 0;

    std::cout << "\nEnter calibration: ";
    std::cin >> yVal;

    Rout.row(0).col(0) = yVal;
    Gout.row(0).col(0) = yVal;
    Bout.row(0).col(0) = yVal;

    appendToCSV(Rout, "cal.csv");
    appendToCSV(Gout, "cal.csv");
    appendToCSV(Bout, "cal.csv");

    appendToCSV(R, "r.csv");
    appendToCSV(G, "g.csv");
    appendToCSV(B, "b.csv");

    std::cin.get();
    return 0;
}
