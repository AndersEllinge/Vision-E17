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

cv::Mat segmentateColorSlider(cv::Mat image){

    const char* window_name = "Color segmentation";
    const char* input = "Input Image";
    int hueMin = 105, saturationMin = 111, valueMin = 49;
    int hueMax = 125, saturationMax = 167, valueMax = 170;

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

cv::Mat segmentateHSV(cv::Mat image){

    int hueMin = 105, saturationMin = 111, valueMin = 49;
    int hueMax = 125, saturationMax = 167, valueMax = 170;

    //convert to HSV
    cv::Mat HSV(image.rows, image.cols, CV_8UC3);
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

cv::Mat drawPoints(std::vector<cv::Point2i> points, cv::Mat image){

    cv::Mat dst = image.clone();

    for (int i = 0; i < points.size(); i++) {
        dst.at<cv::Vec3b>(points[i]) = cv::Vec3b(254,254,254);
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

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv,
        "{help   |            | print this message}"
        "{@image |../lego.jpg | image path}"
        "{filter |            | toggle to high-pass filter the input image}"
    );

    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }

    // Load image as grayscale
    std::string filename = parser.get<std::string>("@image");
    cv::Mat image = cv::imread(filename,cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cout << "Input image not found at '" << filename << "'\n";
        return 1;
    }


    cv::Mat segmented;

    segmented = segmentateHSV(image);
    segmented = opening(segmented, 0, 3);
    segmented = closing(segmented, 0, 3);

    std::vector<std::vector<cv::Point> > contours = findContours(segmented);

    segmented = drawContours(contours,segmented);

    //Get the moments
    std::vector<cv::Moments> mu = getMu(contours);

    //get the mass centers
    std::vector<cv::Point2i> mc = getCenterPoints(mu,contours);

    segmented = drawPoints(mc,segmented);

    mc = toRobotPoints(mc,segmented);




    cv::imshow("Contours", segmented);

    while (cv::waitKey() != 27);
    ; // OpenCV processes slider events here



    return 0;
}
