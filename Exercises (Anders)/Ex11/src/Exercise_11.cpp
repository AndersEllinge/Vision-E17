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
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

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

    cv::Mat img_1 = cv::imread( argv[1], cv::IMREAD_GRAYSCALE );
    cv::Mat img_2 = cv::imread( argv[2], cv::IMREAD_GRAYSCALE );
    if( !img_1.data || !img_2.data )
    { std::cout<< " --(!) Error reading images " << std::endl; return -1; }
    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;
    cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create( minHessian );
    std::vector<cv::KeyPoint> keypoints_1, keypoints_2;
    detector->detect( img_1, keypoints_1 );
    detector->detect( img_2, keypoints_2 );
    //-- Draw keypoints
    cv::Mat img_keypoints_1; cv::Mat img_keypoints_2;
    drawKeypoints( img_1, keypoints_1, img_keypoints_1, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT );
    drawKeypoints( img_2, keypoints_2, img_keypoints_2, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT );
    //-- Show detected (drawn) keypoints
    imshow("Keypoints 1", img_keypoints_1 );
    imshow("Keypoints 2", img_keypoints_2 );

    while (cv::waitKey() != 27)
        ; // OpenCV processes slider events here

    //-- Step 1: Detect the keypoints using SURF Detector, compute the descriptors
    //int minHessian = 400;
    //Ptr<SURF> detector = SURF::create();
    //detector->setHessianThreshold(minHessian);
    //std::vector<KeyPoint> keypoints_1, keypoints_2;
    cv::Mat descriptors_1, descriptors_2;
    detector->detectAndCompute( img_1, cv::Mat(), keypoints_1, descriptors_1 );
    detector->detectAndCompute( img_2, cv::Mat(), keypoints_2, descriptors_2 );
    //-- Step 2: Matching descriptor vectors with a brute force matcher
    cv::BFMatcher matcher(cv::NORM_L2);
    std::vector< cv::DMatch > matches;
    matcher.match( descriptors_1, descriptors_2, matches );
    //-- Draw matches
    cv::Mat img_matches;
    drawMatches( img_1, keypoints_1, img_2, keypoints_2, matches, img_matches );
    //-- Show detected matches
    imshow("Matches", img_matches );

    while (cv::waitKey() != 27)
        ; // OpenCV processes slider events here



    //-- Step 1: Detect the keypoints using SURF Detector, compute the descriptors
    //int minHessian = 400;
    //Ptr<SURF> detector = SURF::create();
    //detector->setHessianThreshold(minHessian);
    //std::vector<KeyPoint> keypoints_1, keypoints_2;
    //Mat descriptors_1, descriptors_2;
    //detector->detectAndCompute( img_1, Mat(), keypoints_1, descriptors_1 );
    //detector->detectAndCompute( img_2, Mat(), keypoints_2, descriptors_2 );
    //-- Step 2: Matching descriptor vectors using FLANN matcher
    cv::FlannBasedMatcher matcherFlann;
    std::vector< cv::DMatch > matchesFlann;
    matcherFlann.match( descriptors_1, descriptors_2, matchesFlann );
    double max_dist = 0; double min_dist = 100;
    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptors_1.rows; i++ )
    { double dist = matchesFlann[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }
    printf("-- Max dist : %f \n", max_dist );
    printf("-- Min dist : %f \n", min_dist );
    //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
    //-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
    //-- small)
    //-- PS.- radiusMatch can also be used here.
    std::vector< cv::DMatch > good_matchesFlann;
    for( int i = 0; i < descriptors_1.rows; i++ )
    { if( matchesFlann[i].distance <= cv::max(2*min_dist, 0.02) )
        { good_matchesFlann.push_back( matches[i]); }
    }
    //-- Draw only "good" matches
    cv::Mat img_matchesFlann;
    drawMatches( img_1, keypoints_1, img_2, keypoints_2,
                 good_matchesFlann, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
                 std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    //-- Show detected matches
    imshow( "Good Matches", img_matches );
    for( int i = 0; i < (int)good_matchesFlann.size(); i++ )
    { printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matchesFlann[i].queryIdx, good_matchesFlann[i].trainIdx ); }

    while (cv::waitKey() != 27)
        ; // OpenCV processes slider events here


    return 0;
}
