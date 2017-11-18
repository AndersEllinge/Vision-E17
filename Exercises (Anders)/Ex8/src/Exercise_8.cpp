/*
 * RoVi1
 * Exercise 3: The Frequency domain and filtering
 */

// e17-1-gbe848aa

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <functional>
#include <iostream>


// Calls the std::function<void()> object passed as 'userdata'
void on_trackbar(int, void* userdata)
{
    (*static_cast<std::function<void()>*>(userdata))();
}


cv::Mat cannyEdgeDetection(cv::Mat image){

    cv::Mat dst, detected_edges;
    //int edgeThresh = 1;
    int lowThreshold = 95;
    int const max_lowThreshold = 200;
    int ratio = 3;
    int kernel_size = 3;
    const char* window_name = "Edge Map";

    std::function<void()> f_canny = [&]() {
        dst.create( image.size(), image.type() );
        blur( image, detected_edges, cv::Size(3,3) );
        Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
        dst = cv::Scalar::all(0);
        image.copyTo( dst, detected_edges);
        imshow( window_name, dst );
    };

    cv::namedWindow( window_name, cv::WINDOW_AUTOSIZE );
    cv::createTrackbar( "Low Threshold:", window_name, &lowThreshold, max_lowThreshold, on_trackbar, &f_canny );
    cv::createTrackbar( "Ratio Threshold:", window_name, &ratio, max_lowThreshold, on_trackbar, &f_canny );

    f_canny(); // Initial call

     while (cv::waitKey() != 27);
         ; // OpenCV processes slider events here
    cvDestroyWindow(window_name);
    return dst;


}

void houghLineTransform(cv::Mat image){

    const char* window_name = "Hough line transform";

    cv::Mat dst,cdst;
    int theta_res = 180;
    int rho_res = 1;
    int threshold = 100;
    cv::Point pt1, pt2;
    float rho, theta;
    double a , b;
    double x0, y0;
    std::vector<cv::Vec2f> lines;

    dst.create( image.size(), image.type());

    std::function<void()> f = [&]() {
        lines.clear();
        image.copyTo(dst);
        cvtColor(image, cdst, CV_GRAY2BGR);
        HoughLines(dst, lines, rho_res, CV_PI/theta_res, threshold, 0, 0 );
        for( size_t i = 0; i < lines.size(); i++ )
        {
            rho = lines[i][0];
            theta = lines[i][1];
            a = cos(theta);
            b = sin(theta);
            x0 = a*rho;
            y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            line( cdst, pt1, pt2, cv::Scalar(0,0,255), 1, CV_AA);
        }

        cv::imshow(window_name, cdst);

    };

    cv::namedWindow( window_name, cv::WINDOW_AUTOSIZE );
    cv::createTrackbar( "Theta:", window_name, &theta_res, 360, on_trackbar, &f );
    cv::createTrackbar( "p:", window_name, &rho_res, 100,  on_trackbar, &f );
    cv::createTrackbar( "Threshold:", window_name, &threshold, 200,on_trackbar, &f );
    f();
    while (cv::waitKey() != 27);
        ; // OpenCV processes slider events here
    cvDestroyWindow(window_name);


}



int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv,
        "{help   |            | print this message}"
        "{@image |            | image path}"
        "{filter |            | toggle to high-pass filter the input image}"
    );

    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }

    // Load image as grayscale
    std::string filename = parser.get<std::string>("@image");
    cv::Mat image = cv::imread(filename, cv::IMREAD_GRAYSCALE);

    if (image.empty()) {
        std::cout << "Input image not found at '" << filename << "'\n";
        return 1;
    }


    cv::Mat edgeImage = cannyEdgeDetection(image);

    houghLineTransform(edgeImage);





    /*
    std::vector<cv::Vec4i> lines;
    HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
        cv::Vec4i l = lines[i];
        line( cdst, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, CV_AA);
    }
    */



    cv::imshow("source", image);
    //cv::imshow("detected lines", cdst);


    while (cv::waitKey() != 27)
        ; // OpenCV processes slider events here

    return 0;
}
