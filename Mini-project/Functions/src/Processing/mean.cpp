#include <opencv2/opencv.hpp>

int main( int argc, char** argv ) {
    
    // Load image
    cv::Mat src;
    src = cv::imread( argv[1], 0);

    if( !src.data )
        { return -1; }
    
    // Create filter
    cv::Mat filter = (cv::Mat_<double>(3,3) << 
    					(1.0/16.0), (2.0/16.0), (1.0/16.0),
    					(2.0/16.0), (4.0/16.0), (2.0/16.0),
    					(1.0/16.0), (2.0/16.0), (1.0/16.0));
    					
    // Apply filter
    cv::Mat output;
    cv::filter2D(src, output, -1, filter);
    
    // Save and show
    //cv::imshow("Result from median filtering", output);
    cv::imwrite("processed.bmp", output);
    
    return 0;
}

