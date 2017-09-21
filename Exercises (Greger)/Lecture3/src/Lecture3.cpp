#include <opencv2/opencv.hpp>
#include <iostream>

int main( int argc, char** argv )
{
    cv::Mat src, intensityChanged;

    // Load image
    src = cv::imread( argv[1], 1 );

    if( !src.data )
        { return -1; }
    
    
    
	return 0;
}
