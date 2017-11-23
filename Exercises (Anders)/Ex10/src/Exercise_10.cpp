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

cv::Mat segmentateColor(cv::Mat image){

    const char* window_name = "Color segmentation";
    const char* input = "Input Image";
    int hueMin = 105, saturationMin = 111, valueMin = 49;
    int hueMax = 125, saturationMax = 167, valueMax = 91;

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

cv::Mat segmentateEuclideanDistance(cv::Mat color, cv::Mat image){

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


    /*
    std::vector<cv::Mat> splitted;
    split(image, splitted);

    //show BGR channels
    cv::Mat emptyImg = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
    cv::Mat blue(image.rows, image.cols, CV_8UC3);
    cv::Mat green(image.rows, image.cols, CV_8UC3);
    cv::Mat red(image.rows, image.cols, CV_8UC3);

    cv::Mat in1[] = {splitted[0], emptyImg, emptyImg};
    cv::Mat in2[] = {emptyImg, splitted[1], emptyImg};
    cv::Mat in3[] = {emptyImg, emptyImg, splitted[2]};

    int from_to[] = {0,0, 1,1, 2,2};

    mixChannels(in1, 3, &blue, 1, from_to, 3);
    mixChannels(in2, 3, &green, 1, from_to, 3);
    mixChannels(in3, 3, &red, 1, from_to, 3);

    imshow("blue", blue);
    imshow("green", green);
    imshow("red", red);


    //convert to HSV
    cv::Mat HSV(image.rows, image.cols, CV_8UC3);
    cv::cvtColor(image, HSV, CV_BGR2HSV);

    std::vector<cv::Mat> HSV_planes;
    split(HSV,HSV_planes);
    cv::Mat H = HSV_planes[0];
    cv::Mat S = HSV_planes[1];
    cv::Mat V = HSV_planes[2];

    imshow("hue", H);
    imshow("saturation", S);
    imshow("value", V);
    */

    cv::Mat segmented;

    /*cv::Rect rec(396,156,30,50);
    cv::Mat colorOfInterest = image(rec);
    segmented = segmentateEuclideanDistance(colorOfInterest,image);*/

    segmented = segmentateColor(image);
    segmented = opening(segmented, 0, 3);
    segmented = closing(segmented, 0, 3);

    cv::imshow("segmented", segmented);


    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(segmented,contours,cv::RETR_TREE, cv::CHAIN_APPROX_NONE );

    cv::Mat contourImg(image.size(), CV_8UC3, cv::Scalar(0,0,0));
    cv::Scalar colors[3];
    colors[0] = cv::Scalar(255,0,0);
    colors[1] = cv::Scalar(0,255,0);
    colors[2] = cv::Scalar(0,0,255);

    for (size_t i = 0; i < contours.size(); i++) {
        cv::drawContours(contourImg, contours, i, colors[i % 3]);
    }

    //Get the moments
    std::vector<cv::Moments> mu(contours.size() );
    for (int i = 0; i < contours.size(); i++) {
        mu[i] = cv::moments(contours[i],false);
    }

    //get the mass centers
    std::vector<cv::Point2f> mc(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        //mc[i] = cv::Point2f(mu[i].m10/mu[i].m00, mu[i].m01/mu[i].m00);
        contourImg.at<cv::Vec3b>(mu[i].m01/mu[i].m00,mu[i].m10/mu[i].m00) = cv::Vec3b(254,254,254);
    }
    //int x = mu[0].m10/mu[0].m00;
    //int y = mu[0].m01/mu[0].m00;

   // std::cout << x << "  " << y << std::endl;



    cv::imshow("Contours", contourImg);

    while (cv::waitKey() != 27);
    ; // OpenCV processes slider events here



    return 0;
}
