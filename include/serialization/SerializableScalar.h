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

    class SerializableScalar : public cv::Scalar
    {
    public:
        SerializableScalar();
        SerializableScalar(Scalar scalar);

        void putScalar(cv::Scalar scalar);

        friend class boost::serialization::access;
    private:
        double m0, m1, m2, m3;

        template<typename Archive>
        void serialize(Archive& ar, const unsigned version)
        {
            (void)version;
            m0 = this->val[0];
            m1 = this->val[1];
            m2 = this->val[2];
            m3 = this->val[3];
            ar & m0 & m1 & m2 & m3;
            cv::Scalar bob(m0,m1,m2,m3);
            (*this).val[0] = m0;
            (*this).val[1] = m1;
            (*this).val[2] = m2;
            (*this).val[3] = m3;
        }
    };

}

#endif
