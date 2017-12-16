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
#include "opencv2/calib3d.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <chrono>
#include <fstream>

// Calls the std::function<void()> object passed as 'userdata'
void on_trackbar(int, void* userdata)
{
    (*static_cast<std::function<void()>*>(userdata))();
}

std::vector<cv::Point2i> toRobotPoints(std::vector<cv::Point2i> points, cv::Mat image){

    cv::Point2i UV(image.cols/2,image.rows/2);
    std::vector<cv::Point2i> newPoints = points;
    //std::cout << "x: " << UV.x << " y: " << UV.y << std::endl;
    for (int i = 0; i < points.size() ; ++i) {
        // std::cout << "x: " << (UV-points[i]).x << " y: " << (UV-points[i]).y << std::endl;
        newPoints[i] = UV-points[i];
    }
    return newPoints;
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

cv::Mat drawPoints(std::vector<cv::Point2i> points, cv::Mat image, cv::Vec3b color){

    cv::Mat dst = image.clone();

    for (int i = 0; i < points.size(); i++) {
        dst.at<cv::Vec3b>(points[i]) = color;
    }

    return dst;
}

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv,
        "{help   |            | print this message}"
        "{@object|            | object path}"
        "{@scenes |           | scene path}"
        "{output |            | name for saved picture}"
        "{hard |            | name for saved picture}"
    );

    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }
    std::string outname = parser.get<std::string>("output");
    std::string object = parser.get<std::string>("@object");
    std::string scenes = parser.get<std::string>("@scenes");

    cv::Mat img_object = imread( object, cv::IMREAD_GRAYSCALE );
    cv::Mat img_scene = imread( scenes, cv::IMREAD_GRAYSCALE );

    if( !img_object.data || !img_scene.data )
    { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;

    cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create( minHessian );

    std::vector<cv::KeyPoint> keypoints_object;
    cv::Mat descriptors_object;
    detector->detectAndCompute( img_object, cv::Mat() ,keypoints_object, descriptors_object );

//start timer
    auto start = std::chrono::system_clock::now();


    std::vector<cv::KeyPoint>  keypoints_scene;

    //-- Step 2: Calculate descriptors (feature vectors)

    cv::Mat descriptors_scene;

   // detector->detectAndCompute( img_object, cv::Mat() ,keypoints_object, descriptors_object );
    detector->detectAndCompute( img_scene, cv::Mat(), keypoints_scene, descriptors_scene );


    //-- Step 3: Matching descriptor vectors using FLANN matcher
    cv::FlannBasedMatcher matcher;
    std::vector< cv::DMatch > matches;
    matcher.match( descriptors_object, descriptors_scene, matches );

    double max_dist = 0; double min_dist = 100;

    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptors_object.rows; i++ )
    { double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }

    //printf("-- Max dist : %f \n", max_dist );
    //printf("-- Min dist : %f \n", min_dist );

    //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
    std::vector< cv::DMatch > good_matches;

    for( int i = 0; i < descriptors_object.rows; i++ )
    {
        if( matches[i].distance < 3*min_dist )
            good_matches.push_back( matches[i]);
    }
/*
    //-- Step 3: Matching descriptor vectors using FLANN matcher
    cv::Ptr<cv::FlannBasedMatcher> matcher = cv::FlannBasedMatcher::create();
    std::vector<std::vector< cv::DMatch >> matches;
    matcher->knnMatch(descriptors_object,descriptors_scene,matches,2);


    //-- Draw only "good" matches
    std::vector< cv::DMatch > good_matches;

    for( int i = 0; i < matches.size(); i++ )
    {
        if( matches[i][0].distance < matches[i][1].distance*0.75)
            good_matches.push_back( matches[i][0]);
    }
*/
    //-- Localize the object
    std::vector<cv::Point2f> obj;
    std::vector<cv::Point2f> scene;

    for( int i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches
        obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
        scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
    }

    cv::Mat H = findHomography( obj, scene, cv::RANSAC );
    if(H.empty())
    {
        std::cout << "No good matches, no new points calculated" << std::endl;
        return 1;
    }
    //-- Get the corners from the image_1 ( the object to be "detected" )
    std::vector<cv::Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0,0);
    obj_corners[1] = cvPoint( img_object.cols, 0 );
    obj_corners[2] = cvPoint( img_object.cols, img_object.rows );
    obj_corners[3] = cvPoint( 0, img_object.rows );
    std::vector<cv::Point2f> scene_corners(4);

    perspectiveTransform( obj_corners, scene_corners, H);


    //-- Show detected matches

    std::vector<cv::Point2i> points;
    points.push_back(scene_corners[0] );
    points.push_back(scene_corners[1] );
    points.push_back(scene_corners[2] );
    points.push_back(scene_corners[3] );

    auto end = std::chrono::system_clock::now();

    long elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();

    std::ofstream outfile;

    if (parser.has("hard")) {
        outfile.open("marker3HardTime.csv", std::ios_base::app);
        outfile << elapsed_time << "," << matches.size() << std::endl;
    }
    else{
        outfile.open("marker3Time.csv", std::ios_base::app);
        outfile << elapsed_time << "," << matches.size() << std::endl;
    }

    std::vector<cv::Point2i> allPoints;
    allPoints = toRobotPoints(allPoints, img_scene);

    for (int i = 0; i < points.size(); i++) {
       cv::circle(img_scene,points[i],5,0,2);
    }

    if (parser.has("hard"))
        cv::imwrite("results/marker3_hard/" + outname, img_scene);
    else
        cv::imwrite("results/marker3/" + outname, img_scene);


    return 1;
}

