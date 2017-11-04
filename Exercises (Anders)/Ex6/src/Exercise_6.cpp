/*
 * RoVi1
 * Exercise 3: The Frequency domain and filtering
 */

// e17-1-gbe848aa

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

// Rearranges the quadrants of a Fourier image so that the origin is at the
// center of the image.

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv,
        "{help   |            | print this message}"
        "{@image | ./lena.bmp | image path}"
        "{filter |            | toggle to high-pass filter the input image}"
    );

    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }

    // Load image as grayscale
    std::string filename = parser.get<std::string>("@image");
    cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);

    if (img.empty()) {
        std::cout << "Input image not found at '" << filename << "'\n";
        return 1;
    }

    // bluring image for better thresholding
    // Create filter
    cv::Mat filter = (cv::Mat_<double>(3,3) <<
    					(1.0/16.0), (2.0/16.0), (1.0/16.0),
    					(2.0/16.0), (4.0/16.0), (2.0/16.0),
    					(1.0/16.0), (2.0/16.0), (1.0/16.0));

    // Apply filter

    /*for (size_t i = 0; i < img.rows; i++) {
      for (size_t k = 0; k < img.cols; k++) {
        if (img.at<uchar>(i,k) < 90)
          img.at<uchar>(i,k) = 0;
        else
          img.at<uchar>(i,k) = 255;
      }
    }*/
    for (size_t i = 0; i < 40; i++) {
      cv::filter2D(img, img, -1, filter);
    }

    for (size_t i = 0; i < img.rows; i++) {
      for (size_t k = 0; k < img.cols; k++) {
        if (img.at<uchar>(i,k) < 110)
          img.at<uchar>(i,k) = 0;
        else
          img.at<uchar>(i,k) = 255;
      }
    }
    cv::imshow("lort", img);
    cv::waitKey();

    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(img,contours,cv::RETR_TREE, cv::CHAIN_APPROX_NONE );

    cv::Mat contourImg(img.size(), CV_8UC3, cv::Scalar(0,0,0));
    cv::Scalar colors[3];
    colors[0] = cv::Scalar(255,0,0);
    colors[1] = cv::Scalar(0,255,0);
    colors[2] = cv::Scalar(0,0,255);

    for (size_t i = 0; i < contours.size(); i++) {
        cv::drawContours(contourImg, contours, i, colors[i % 3]);
    }

    std::cout << "Number of contours " << contours.size() << std::endl;
    cv::imwrite("BluredImage.png", contourImg);

    cv::waitKey();
    return 0;
}
