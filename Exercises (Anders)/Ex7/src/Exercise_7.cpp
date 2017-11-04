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

/*void sobelEdgeDectiong(cv::Mat image){

    // First we declare the variables we are going to use
    cv::Mat src, src_gray;
    cv::Mat grad;
    const std::string window_name = "Sobel Demo - Simple Edge Detector";
    int ksize = 3;
    int scale = -1;
    int delta = 0;
    int ddepth = CV_16S;

    std::function<void()> f_sobel = [&]() {
        // Remove noise by blurring with a Gaussian filter ( kernel size = 3 )
        cv::GaussianBlur(image, src, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
        cv::Mat grad_x, grad_y;
        cv::Mat abs_grad_x, abs_grad_y;

        if( ksize%2 != 0){
            Sobel(src, grad_x, ddepth, 1, 0, ksize, scale, delta, cv::BORDER_DEFAULT);
            Sobel(src, grad_y, ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);
            //converting back to CV_8U
            convertScaleAbs(grad_x, abs_grad_x);
            convertScaleAbs(grad_y, abs_grad_y);
            addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
            imshow(window_name, grad);
        }
    };

    cv::namedWindow(window_name, cv::WINDOW_NORMAL);
    cv::createTrackbar("ksize:", window_name, &ksize, 30, on_trackbar, &f_sobel);
    cv::createTrackbar("scale:", window_name, &scale, 20, on_trackbar, &f_sobel);
    f_sobel(); // Initial call
}*/


/*void cannyEdgeDetection(cv::Mat image){

    cv::Mat dst, detected_edges;
    //int edgeThresh = 1;
    int lowThreshold = 1;
    int const max_lowThreshold = 100;
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
    cv::createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, on_trackbar, &f_canny );
    f_canny(); // Initial call
}*/


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

    //----------------- Canny part ------------------//
    cv::Mat dst, detected_edges;
    //int edgeThresh = 1;
    int lowThreshold = 1;
    int const max_lowThreshold = 100;
    int ratio = 3;
    int kernel_size = 3;
    const char* window_name = "Edge Map";

    std::function<void()> f_canny = [&]() {
        if( kernel_size %2 != 0) {
            dst.create(image.size(), image.type());
            blur(image, detected_edges, cv::Size(3, 3));
            Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);
            dst = cv::Scalar::all(0);
            image.copyTo(dst, detected_edges);
            imshow(window_name, dst);
        }
    };

    cv::namedWindow( window_name, cv::WINDOW_NORMAL );
    cv::createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, on_trackbar, &f_canny );
    cv::createTrackbar( "Ratio:", window_name, &ratio, 10, on_trackbar, &f_canny );
    cv::createTrackbar( "Kernel:", window_name, &kernel_size, 7, on_trackbar, &f_canny );
    f_canny(); // Initial call



    //----------------- Sobel part ---------------//
    // First we declare the variables we are going to use
    cv::Mat src, src_gray;
    cv::Mat grad;
    const std::string window_sobel_name = "Sobel Demo - Simple Edge Detector";
    int ksize = 3;
    int scale = -1;
    int delta = 0;
    int ddepth = CV_16S;

    std::function<void()> f_sobel = [&]() {
        // Remove noise by blurring with a Gaussian filter ( kernel size = 3 )
        cv::GaussianBlur(image, src, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
        cv::Mat grad_x, grad_y;
        cv::Mat abs_grad_x, abs_grad_y;

        if( ksize%2 != 0){
            Sobel(src, grad_x, ddepth, 1, 0, ksize, scale, delta, cv::BORDER_DEFAULT);
            Sobel(src, grad_y, ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);
            //converting back to CV_8U
            convertScaleAbs(grad_x, abs_grad_x);
            convertScaleAbs(grad_y, abs_grad_y);
            addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
            imshow(window_sobel_name, grad);
        }
    };

    cv::namedWindow(window_sobel_name, cv::WINDOW_NORMAL);
    cv::createTrackbar("ksize:", window_sobel_name, &ksize, 30, on_trackbar, &f_sobel);
    cv::createTrackbar("scale:", window_sobel_name, &scale, 20, on_trackbar, &f_sobel);
    cv::createTrackbar("delta:", window_sobel_name, &delta, 20, on_trackbar, &f_sobel);
    f_sobel(); // Initial call





    while (cv::waitKey() != 27)
        ; // OpenCV processes slider events here

    return 0;
}
