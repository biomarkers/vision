#ifndef _SER_MAT_H
#define _SER_MAT_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <iostream>


//forward declaring serialization class
namespace boost{
namespace serialization{
class access;
}
}

namespace cv{

    class SerializableMat : public cv::Mat
    {
    public:
        SerializableMat();
        SerializableMat(cv::Mat mat);

        void putMatrix(cv::Mat mat);

        friend class boost::serialization::access;

    private:
        std::vector< std::vector< float> > mMat;

        template<typename Archive>
        void serialize(Archive& ar, const unsigned version)
        {
            (void)version;

            //std::cout << "Serializing matrix\n";

            std::vector<float> rowz;
            int height, width;
            height = size().height;
            width = size().width;
            for(int c = 0; c < height; c++)
            {
                mMat.push_back(rowz);
                for(int i = 0; i < width; i++)
                {
                    mMat[c].push_back(row(c).at<float>(i));
                }
            }

            //std::cout << "now sticking serialized matrix into archive\n";

            ar & mMat;

            height = mMat.size();
            width = 0;
            if(height > 0)
                width = mMat[0].size();

            cv::Mat rowMat(1, width, CV_32F);

            release();
            for(int c = 0; c < height; c++)
            {
                push_back(rowMat);
                for(int i = 0; i < width; i++)
                {
                    row(c).at<float>(i) = mMat[c][i];
                }
            }
            mMat.clear();
        }
    };

}

#endif
