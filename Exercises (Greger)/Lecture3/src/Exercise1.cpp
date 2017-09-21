/*
 * RoVi1
 * Example application to load and display an image
 */

// v1.0-4-gdfe246a

#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    // Parse command line arguments
    cv::CommandLineParser parser(argc, argv,
        "{help   |            | print this message}"
        "{@image | ./book.jpg | image path}"
    );

    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }

    // Load image
    std::string filename = parser.get<std::string>("@image");
    cv::Mat img = cv::imread(filename);

    if (img.empty()) {
        std::cout << "Input image not found at '" << filename << "'\n";
        return 1;
    }

    // Do something
    
    // Convert to gray image
	cv::Mat grayImg;
	cv::cvtColor(img, grayImg, CV_BGR2GRAY);
	
    cv::imshow("Gray", grayImg);
    cv::imwrite("../Gray.jpg", grayImg);
    
    
    // First version
    cv::Mat out1;
    out1 = img.clone();
    for(int i = 100; i < 221; i++) {
        for(int j = 350; j < 441; j++) {
            out1.at<cv::Vec3b>(i, j)[0] = 0;
            out1.at<cv::Vec3b>(i, j)[1] = 0;
            out1.at<cv::Vec3b>(i, j)[2] = 0;
        }
    }
    
    cv::imshow("Output1", out1);
    cv::imwrite("../output1.jpg", out1);
    
    // Second version
    cv::Mat out2;
    out2 = img.clone();
    cv::Vec3b pixel(0,0,255);
	for(int i = 100; i < 221; i++) {
        for(int j = 350; j < 441; j++) {
            out2.at<cv::Vec3b>(i, j) = pixel;
        }
    }
    
    cv::imshow("Output2", out2);
    cv::imwrite("../output2.jpg", out2);
    
    // Third version
    cv::Mat out3;
    out3 = img.clone();
	for(int i = 100; i < 221; i++) {
	    cv::Vec3b* p_row = out3.ptr<cv::Vec3b>(i);
        for(int j = 350; j < 441; j++) {
            p_row[j] = cv::Vec3b::all(0);
        }
    }
    
    cv::imshow("Output3", out3);
    cv::imwrite("../output3.jpg", out3);
    
    
    // Modify gray image
    cv::Mat outGray;
    outGray = grayImg.clone();
    for(int i = 100; i < 221; i++) {
        for(int j = 350; j < 441; j++) {
            outGray.at<uchar>(i, j) = 0;
        }
    }
    
    cv::imshow("GrayOutput", outGray);
    cv::imwrite("../GrayOutput.jpg", outGray);
    
    
    cv::waitKey();

    return 0;
}
