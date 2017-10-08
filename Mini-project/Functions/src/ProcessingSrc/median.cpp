#include <opencv2/opencv.hpp>

int main( int argc, char** argv ) {
    
    // Load image
    cv::Mat src;
    src = cv::imread( argv[1], 0);

    if( !src.data )
        { return -1; }
    
    // Filter
    cv::Mat output;
    cv::medianBlur(src, output, 11);
    
    // Save and show
    //cv::imshow("Result from median filtering", output);
    cv::imwrite("processed.bmp", output);
    
    return 0;
}

