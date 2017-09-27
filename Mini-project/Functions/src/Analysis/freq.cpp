#include <opencv2/opencv.hpp>
#include <iostream>

void dftshift(cv::Mat& mag);

int main( int argc, char** argv )
{
    cv::Mat src;

    // Load image
    src = cv::imread( argv[1], 0);

    if( !src.data )
        { return -1; }

    // Do dft?
    cv::Mat I = src.clone();
    cv::Mat planes[] = {cv::Mat_<float>(I), cv::Mat_<float>::zeros(I.size())};
    cv::Mat dft;
    cv::merge(planes, 2, dft);

    std::cout << "Channels: " << dft.channels() << std::endl;

    cv::dft(dft, dft);

    // Split into amplitude and phase channel
    cv::Mat dftRes[2];
    cv::split(dft, dftRes);
    cv::Mat mag;
    cv::magnitude(dftRes[0], dftRes[1], mag);


    mag += cv::Scalar::all(1);
    cv::log(mag, mag);
    cv::normalize(mag, mag, 0, 1, cv::NORM_MINMAX);
    dftshift(mag);


    //cv::imshow("im", I);
    //cv::imshow("real", dftRes[0]);
    //cv::imshow("imag", dftRes[1]);
    //cv::imshow("mag", mag);
    //cv::waitKey();
    
    cv::imwrite("freq_magnitude.bmp", mag);
    
    return 0;
}

void dftshift(cv::Mat& mag)
{
    int cx = mag.cols / 2;
    int cy = mag.rows / 2;

    cv::Mat tmp;
    cv::Mat q0(mag, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(mag, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(mag, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(mag, cv::Rect(cx, cy, cx, cy));

    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}
