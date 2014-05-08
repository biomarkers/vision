#ifndef _SER_SCALAR_H
#define _SER_SCALAR_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


//forward declaring serialization class
namespace boost{
namespace serialization{
class access;
}
}

namespace cv{

    // extension of a cv::Scalar that can be serialized into a boost archive
    class SerializableScalar : public cv::Scalar
    {
    public:
        //default constructor, simply calls default cv::Scalar constructor
        SerializableScalar();

        //constructor taking a cv::Scalar so we can have an implicit conversion
        //from cv::Scalar -> cv::SerializableScalar. Allows us to send cv::Scalars
        //into template functions of type cv::SerializableScalar.
        //Calls putScalar to fill in the data.
        SerializableScalar(Scalar scalar);

        //copies the data from a given cv::Scalar into this object
        void putScalar(cv::Scalar scalar);

        friend class boost::serialization::access;
    private:
        double m0, m1, m2, m3;

        template<typename Archive>
        void serialize(Archive& ar, const unsigned version)
        {
            (void)version;
            //serialization code, pulls out the inner data into 4 doubles
            //and writes into archive
            m0 = this->val[0];
            m1 = this->val[1];
            m2 = this->val[2];
            m3 = this->val[3];
            ar & m0 & m1 & m2 & m3;
            //deserialization code, puts the values of the 4 doubles back into
            //the object
            this->val[0] = m0;
            this->val[1] = m1;
            this->val[2] = m2;
            this->val[3] = m3;
        }
    };

}

#endif
