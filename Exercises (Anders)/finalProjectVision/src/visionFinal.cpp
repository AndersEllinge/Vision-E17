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
#include <fstream>
#include <algorithm>
#include <chrono>
#include <ctime>


// Calls the std::function<void()> object passed as 'userdata'
void on_trackbar(int, void* userdata)
{
    (*static_cast<std::function<void()>*>(userdata))();
}

cv::Mat segmentateColorSlider(cv::Mat image){

    const char* window_name = "Color segmentation";
    const char* input = "Input Image";
    int hueMin = 105, saturationMin = 83, valueMin = 42;
    int hueMax = 125, saturationMax = 180, valueMax = 115;

    //convert to HSV
    cv::Mat HSV(image.rows, image.cols, CV_8UC3);
    cv::cvtColor(image, HSV, CV_BGR2HSV);

    cv::Mat dst;

    std::function<void()> f = [&]() {

        inRange(HSV, cv::Scalar(hueMin,saturationMin,valueMin), cv::Scalar(hueMax,saturationMax,valueMax), dst);

        cv::imshow(window_name, dst);

    };

    cv::namedWindow( window_name, cv::WINDOW_NORMAL );
    cv::createTrackbar( "HueMin:", window_name, &hueMin, 255, on_trackbar, &f );
    cv::createTrackbar( "HueMax:", window_name, &hueMax, 255, on_trackbar, &f );
    cv::createTrackbar( "SatMin:", window_name, &saturationMin, 255,  on_trackbar, &f );
    cv::createTrackbar( "SatMax:", window_name, &saturationMax, 255,  on_trackbar, &f );
    cv::createTrackbar( "ValMin:", window_name, &valueMin, 255,on_trackbar, &f );
    cv::createTrackbar( "ValMax:", window_name, &valueMax, 255,on_trackbar, &f );

    f();

    cv::namedWindow(input, cv::WINDOW_AUTOSIZE);
    cv::imshow(input, image);
    while (cv::waitKey() != 27);
    ; // OpenCV processes slider events here
    cvDestroyWindow(window_name);
    cvDestroyWindow(input);
    return dst;
}

cv::Mat segmentateEuclideanDistanceSlider(cv::Mat color, cv::Mat image){

    const char* window_name = "Color segmentation";
    const char* input = "Input Image";
    const char* ROI = "ROI";
    cv::Scalar avg = cv::mean(color);
    int threshold = 20;
    cv::Scalar thres;
    //double distance = cv::norm(threshold - avg);

    cv::Mat dst;

    std::function<void()> f = [&]() {

        thres = {threshold,threshold,threshold};
        inRange(image, avg-thres, avg+thres, dst);

        cv::imshow(window_name, dst);

    };

    cv::namedWindow( window_name, cv::WINDOW_AUTOSIZE );
    cv::createTrackbar( "HueMin:", window_name, &threshold, 255, on_trackbar, &f );

    f();
    cv::namedWindow(input, cv::WINDOW_AUTOSIZE);
    cv::imshow(input, image);
    cv::namedWindow(ROI, cv::WINDOW_AUTOSIZE);
    cv::imshow(ROI, color);
    while (cv::waitKey() != 27);
    ; // OpenCV processes slider events here
    cvDestroyWindow(window_name);
    cvDestroyWindow(ROI);
    cvDestroyWindow(input);

    return dst;
}

cv::Mat opening(cv::Mat image, int elementType, int kernelSize){
    //elementType 0 = MORPTH_RECT, 1 = MORPH_CROSS, 2 = MORPH_ELLIPSE
    cv::Mat dst;
    cv::Mat element = cv::getStructuringElement(elementType,
                      cv::Size(2*kernelSize+1, 2*kernelSize+1),
                      cv::Point(kernelSize,kernelSize));

    cv::erode(image,dst,element);
    cv::dilate(dst, dst, element);


    return dst;
}

cv::Mat closing(cv::Mat image, int elementType, int kernelSize){
    //elementType 0 = MORPTH_RECT, 1 = MORPH_CROSS, 2 = MORPH_ELLIPSE
    cv::Mat dst;
    cv::Mat element = cv::getStructuringElement(elementType,
                      cv::Size(2*kernelSize+1, 2*kernelSize+1),
                      cv::Point(kernelSize,kernelSize));

    cv::dilate(image,dst,element);
    cv::erode(dst,dst,element);


    return dst;
}

cv::Mat segmentateHSV(cv::Mat image, int hueMin, int hueMax, int saturationMin, int saturationMax, int valueMin, int valueMax){

    //convert to HSV
    cv::Mat HSV(image.rows, image.cols, CV_8UC3);
    cv::cvtColor(image, image, CV_BGRA2BGR);
    cv::cvtColor(image, HSV, CV_BGR2HSV);

    cv::Mat dst;

    inRange(HSV, cv::Scalar(hueMin,saturationMin,valueMin), cv::Scalar(hueMax,saturationMax,valueMax), dst);

    return dst;
}

cv::Mat segmentateED(cv::Mat color, cv::Mat image, int threshold){

    cv::Scalar avg = cv::mean(color);

    cv::Scalar thres ={threshold,threshold,threshold};

    cv::Mat dst;

    inRange(image, avg-thres, avg+thres, dst);

    return dst;
}

std::vector<std::vector<cv::Point> > findContours(cv::Mat image){

    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(image,contours,cv::RETR_TREE, cv::CHAIN_APPROX_NONE );

    return contours;
}

cv::Mat drawContours(std::vector<std::vector<cv::Point> > contours, cv::Mat image){

    cv::Mat contourImg(image.size(), CV_8UC3, cv::Scalar(0,0,0));
    cv::Scalar colors[3];
    colors[0] = cv::Scalar(255,0,0);
    colors[1] = cv::Scalar(0,255,0);
    colors[2] = cv::Scalar(0,0,255);

    for (size_t i = 0; i < contours.size(); i++) {
        cv::drawContours(contourImg, contours, i, colors[i % 3]);
    }

    return contourImg;
}

std::vector<cv::Moments> getMu(std::vector<std::vector<cv::Point> > contours ){
    std::vector<cv::Moments> mu(contours.size() );
    for (int i = 0; i < contours.size(); i++) {
        mu[i] = cv::moments(contours[i],false);
    }
    return mu;
}

std::vector<cv::Point2i> getCenterPoints(std::vector<cv::Moments> moments, std::vector<std::vector<cv::Point> > contours){
    std::vector<cv::Point2i> centerPoints(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        centerPoints[i] = cv::Point2f(moments[i].m10/moments[i].m00, moments[i].m01/moments[i].m00);
    }
    return centerPoints;
}

cv::Mat drawPoints(std::vector<cv::Point2i> points, cv::Mat image, cv::Vec3b color){

    cv::Mat dst = image.clone();

    for (int i = 0; i < points.size(); i++) {
        dst.at<cv::Vec3b>(points[i]) = color;
    }

    return dst;
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

std::vector<cv::Point2i> decideOnBlueMarkers(std::vector<cv::Point2i> mcBlue, std::vector<cv::Point2i> mcRed){

    if(mcRed.size() < 1 || mcBlue.size() < 3)
        return mcRed;

    cv::Vec2i vector1 = {mcBlue[0].x - mcRed[0].x,mcBlue[0].y - mcRed[0].y};
    cv::Vec2i vector2 = {mcBlue[1].x - mcRed[0].x,mcBlue[1].y - mcRed[0].y};
    cv::Vec2i vector3 = {mcBlue[2].x - mcRed[0].x,mcBlue[2].y - mcRed[0].y};

    std::vector<cv::Point2i> points;

    std::vector<int> dotProducts = {vector1.dot(vector2), vector1.dot(vector3),vector2.dot(vector3)};
    int idx = std::min_element(dotProducts.begin(),dotProducts.end())-dotProducts.begin();
    if(idx == 0){

        int crossProduct = vector1[0]*vector2[1] - vector1[1]*vector2[0];;
        if(crossProduct < 0){
            points = {mcBlue[0],mcBlue[1],mcBlue[2]};
        }
        else{
            points = {mcBlue[1],mcBlue[0],mcBlue[2]};
        }
    }
    else if(idx == 1){
        int crossProduct = vector1[0]*vector3[1] - vector1[1]*vector3[0];
        if(crossProduct < 0){
            points = {mcBlue[0],mcBlue[2],mcBlue[1]};
        }
        else{
            points = {mcBlue[2],mcBlue[0],mcBlue[1]};
        }
    }
    else if(idx == 2){
        int crossProduct = vector2[0]*vector3[1] - vector2[1]*vector3[0];
        if(crossProduct < 0){
            points = {mcBlue[1],mcBlue[2],mcBlue[0]};
        }
        else{
            points = {mcBlue[2],mcBlue[1],mcBlue[0]};
        }
    }
    return points;
}

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv,
        "{help     |            | print this message}"
        "{@image   |../lego.jpg | image path}"
        "{output   |            | name for saved picture}"
        "{slider   |            | Open program with slider bars}"
        "{time     |            | Test point detection versus time}"
        "{sequence |            | Find points on a sequence and show em on picture}"
        "{hard     |            | save in folder for hard sequence}"
    );

    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }

    // Load image as grayscale
    std::string filename = parser.get<std::string>("@image");
    std::string outname = parser.get<std::string>("output");
    cv::Mat image = cv::imread(filename,cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cout << "Input image not found at '" << filename << "'\n";
        return 1;
    }


    if (parser.has("slider"))
    {

        cv::Mat segmented;

        segmented = segmentateColorSlider(image);
        segmented = opening(segmented, 0, 3);
        segmented = closing(segmented, 0, 3);
        segmented = closing(segmented, 0, 3);


        std::vector<std::vector<cv::Point> > contours = findContours(segmented);

        segmented = drawContours(contours, segmented);

        //Get the moments
        std::vector<cv::Moments> mu = getMu(contours);

        //get the mass centers
        std::vector<cv::Point2i> mc = getCenterPoints(mu, contours);

        segmented = drawPoints(mc, segmented,cv::Vec3b(254,254,254));

        mc = toRobotPoints(mc, segmented);

        //cv::imwrite("results/marker1_hard/" + outname, segmented);

        cv::imshow("lort", segmented);

        while (cv::waitKey() != 27);
        //; // OpenCV processes slider events here



        return 0;
    }

    if (parser.has("time")) {
        auto start = std::chrono::system_clock::now();


        cv::Mat segmentedBlue, segmentedRed;

        //FIND BLUE//105,125,83,180,42,115
        segmentedBlue = segmentateHSV(image, 110, 120, 83, 180, 35, 153); //blue circles
        segmentedBlue = opening(segmentedBlue, 0, 3);
        segmentedBlue = closing(segmentedBlue, 0, 6);
        segmentedBlue = opening(segmentedBlue, 0, 14);

        std::vector<std::vector<cv::Point> > contoursBlue = findContours(segmentedBlue);

        //segmentedBlue = drawContours(contoursBlue, segmentedBlue);

        //Get the moments
        std::vector<cv::Moments> muBlue = getMu(contoursBlue);

        //get the mass centers
        std::vector<cv::Point2i> mcBlue = getCenterPoints(muBlue, contoursBlue);

        //segmentedBlue = drawPoints(mcBlue, segmentedBlue, cv::Vec3b(255,0,0));

        //mcBlue = toRobotPoints(mcBlue, segmentedBlue);


        //FIND RED //0,25,170,215,80,190
        segmentedRed = segmentateHSV(image, 0, 10, 137, 215, 80, 215); //red circles
        segmentedRed = opening(segmentedRed, 0, 3);
        segmentedRed = closing(segmentedRed, 0, 6);
        segmentedRed = opening(segmentedRed, 0, 14);

        std::vector<std::vector<cv::Point> > contoursRed = findContours(segmentedRed);

        //segmentedRed = drawContours(contoursRed, segmentedRed);

        //Get the moments
        std::vector<cv::Moments> muRed = getMu(contoursRed);

        //get the mass centers
        std::vector<cv::Point2i> mcRed = getCenterPoints(muRed, contoursRed);

        //segmentedRed = drawPoints(mcRed, segmentedRed,cv::Vec3b(0,0,255));

        //mcRed = toRobotPoints(mcRed, segmentedRed);

        //DO STUFF WITH RED AND BLUE
        //cv::Mat result = segmentedBlue + segmentedRed;

        std::vector<cv::Point2i> bluePoints = decideOnBlueMarkers(mcBlue, mcRed);

        if (bluePoints.size() < 3 || mcRed.size() < 1) {
            std::cout << "Did not find enough markers" << std::endl;
            return 1;
        }

        //result.at<cv::Vec3b>(bluePoints[0]) = {0,255,255};
        //result.at<cv::Vec3b>(bluePoints[1]) = {255,255,255};

        std::vector<cv::Point2i> allPoints;
        for (int i = 0; i < mcRed.size(); i++) {
            //cv::circle(result, mcRed[i], 10, cv::Vec3b(255, 0, 0), 4);
            allPoints.push_back(mcRed[i]);
        }
        for (int i = 0; i < bluePoints.size(); i++) {
            allPoints.push_back(bluePoints[i]);
            //cv::circle(result, bluePoints[i], 10, cv::Vec3b(0, 0, 255), 4);
        }
        allPoints = toRobotPoints(allPoints, image);


        auto end = std::chrono::system_clock::now();

        long elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        std::ofstream outfile;

        if(parser.has("hard")) {
            outfile.open("marker1TimeHard.csv", std::ios_base::app);
            outfile << elapsed_time << std::endl;
        }
        else {
            outfile.open("marker1Time.csv", std::ios_base::app);
            outfile << elapsed_time << std::endl;
        }


        //std::cout << "Red: " << allPoints[0] << "Blue1: " << allPoints[1] << "Blue2: " << allPoints[2]<< std::endl;

        //cv::imwrite("results/marker1/" + outname, result);
    }

    if (parser.has("sequence")) {
        cv::Mat segmentedBlue, segmentedRed;

        //FIND BLUE//105,125,83,180,42,115
        segmentedBlue = segmentateHSV(image, 110, 120, 83, 180, 35, 153); //blue circles
        segmentedBlue = opening(segmentedBlue, 0, 3);
        segmentedBlue = closing(segmentedBlue, 0, 6);
        segmentedBlue = opening(segmentedBlue, 0, 14);

        std::vector<std::vector<cv::Point> > contoursBlue = findContours(segmentedBlue);

        //segmentedBlue = drawContours(contoursBlue, segmentedBlue);

        //Get the moments
        std::vector<cv::Moments> muBlue = getMu(contoursBlue);

        //get the mass centers
        std::vector<cv::Point2i> mcBlue = getCenterPoints(muBlue, contoursBlue);

        //segmentedBlue = drawPoints(mcBlue, segmentedBlue, cv::Vec3b(255,0,0));

        //mcBlue = toRobotPoints(mcBlue, segmentedBlue);


        //FIND RED //0,25,170,215,80,190
        segmentedRed = segmentateHSV(image, 0, 10, 137, 215, 80, 215); //red circles
        segmentedRed = opening(segmentedRed, 0, 3);
        segmentedRed = closing(segmentedRed, 0, 6);
        segmentedRed = opening(segmentedRed, 0, 14);

        std::vector<std::vector<cv::Point> > contoursRed = findContours(segmentedRed);

        //segmentedRed = drawContours(contoursRed, segmentedRed);

        //Get the moments
        std::vector<cv::Moments> muRed = getMu(contoursRed);

        //get the mass centers
        std::vector<cv::Point2i> mcRed = getCenterPoints(muRed, contoursRed);

        //segmentedRed = drawPoints(mcRed, segmentedRed,cv::Vec3b(0,0,255));

        //mcRed = toRobotPoints(mcRed, segmentedRed);

        //DO STUFF WITH RED AND BLUE
        //cv::Mat result = segmentedBlue + segmentedRed;

        std::vector<cv::Point2i> bluePoints = decideOnBlueMarkers(mcBlue, mcRed);

        if (bluePoints.size() < 3 || mcRed.size() < 1) {
            std::cout << "Did not find enough markers" << std::endl;
            return 1;
        }

        //result.at<cv::Vec3b>(bluePoints[0]) = {0,255,255};
        //result.at<cv::Vec3b>(bluePoints[1]) = {255,255,255};

        //image = drawContours(contoursRed, image);
        //image = drawContours(contoursBlue, image);
        std::vector<cv::Point2i> allPoints;
        for (int i = 0; i < mcRed.size(); i++) {
            cv::circle(image, mcRed[i], 10, cv::Vec3b(0, 0, 255), 2);
            allPoints.push_back(mcRed[i]);
        }
        for (int i = 0; i < bluePoints.size(); i++) {
            allPoints.push_back(bluePoints[i]);
            cv::circle(image, bluePoints[i], 10, cv::Vec3b(255, 0, 0), 2);
        }
        allPoints = toRobotPoints(allPoints, image);

        if(parser.has("hard"))
            cv::imwrite("results/marker1_hard/" + outname, image);
        else
            cv::imwrite("results/marker1/" + outname, image);
    }



    return 0;
}
