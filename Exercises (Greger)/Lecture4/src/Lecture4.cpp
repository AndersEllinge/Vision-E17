#include <opencv2/opencv.hpp>
#include <iostream>

void dftshift(cv::Mat& mag);
cv::Mat butterHP(cv::Mat im, int order, int d0);

int main( int argc, char** argv )
{
    cv::Mat src;

    // Load image
    src = cv::imread( argv[1], 0);

    if( !src.data )
        { return -1; }
        
    // Pad image
    cv::Mat padded;
    int opt_rows = cv::getOptimalDFTSize(src.rows);
    int opt_cols = cv::getOptimalDFTSize(src.cols);
    cv::copyMakeBorder(src, padded, 0, opt_rows, 0, opt_cols, cv::BORDER_CONSTANT, 0);

    // Do dft
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat_<float>::zeros(padded.size())};
    cv::Mat dft;
    cv::merge(planes, 2, dft);

    std::cout << "Channels: " << dft.channels() << std::endl;

    cv::dft(dft, dft);

    // Split into amplitude and phase channel
    cv::Mat dftRes[2];
    cv::split(dft, dftRes);
    cv::Mat mag;
    cv::magnitude(dftRes[0], dftRes[1], mag);
    dftshift(mag);
    
    
    // Apply filter
    cv::Mat filter;
    filter = butterHP(mag, 2, 250);
    
    
    
    

	// Visualize magnitude
    mag += cv::Scalar::all(1);
    cv::log(mag, mag);
    cv::normalize(mag, mag, 0, 1, cv::NORM_MINMAX);
    


    //cv::imshow("im", I);
    //cv::imshow("real", dftRes[0]);
    //cv::imshow("imag", dftRes[1]);
    //cv::imshow("mag", mag);
    //cv::imshow("padded", padded);
    cv::imshow("filter", filter);
    
    
    cv::waitKey();
    
    return 0;
}

cv::Mat butterHP(cv::Mat im, int order, int d0) {
	cv::Mat out = cv::Mat_<float>::zeros(im.size());
	
	for(int x = 0; x < out.rows; x++) {
		for (int y = 0; y < out.cols; y++) {
			float dist = sqrt(pow(x,2.0) + pow(y,2.0));
			float val = 1.0 / (1.0 + pow(dist / d0, -2.0 * order));
			out.at<float>(x,y) = val; 
		}
	}
	
	return out;
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
