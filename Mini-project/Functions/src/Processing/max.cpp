#include <opencv2/opencv.hpp>

int main( int argc, char** argv ) {
    
    // Load image
    cv::Mat src;
    src = cv::imread( argv[1], 0);

    if( !src.data )
        { return -1; }

	// Filter
	cv::Mat kernel;
	cv::Mat output;
	cv::dilate(src, output, kernel);
	
	// Save and show
    //cv::imshow("Result from max filtering", output);
    cv::imwrite("processed.bmp", output);
}
