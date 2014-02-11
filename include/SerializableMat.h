#ifndef _SER_MAT_H
#define _SER_MAT_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>


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
            int height, width;
            height = size().height;
            width = size().width;
            for(int c = 0; c < height; c++)
            {
                for(int i = 0; i < width; i++)
                {
                    mMat[c][i] = row(c).at<float>(i);
                }
            }
            ar & mMat;
            for(int c = 0; c < height; c++)
            {
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
