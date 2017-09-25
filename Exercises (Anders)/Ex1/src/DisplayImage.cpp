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
    
    /* -------------------- Exercise 1.1 -----------------------*/
    /* Write a program that, using OpenCV, loads the color image 
       and changes its pixel values to draw a black rectangle.
       Complete the task in (preferable no less than tree) 
       differnet ways . explore different methods of accessing 
       image pixel values using the OpenCV API */
    
    // A
    cv::Mat imgA = img.clone(); 
    
	cv::Vec3b blackPixel(0,0,0);

  	for(int i = 100; i <= 200 ;i++) {
	    for(int j = 350; j <= 440; j++)
		    imgA.at<cv::Vec3b>(i,j) = blackPixel;
	};
	
   
	cv::imshow("ImageA", imgA);
	
	// B
	cv::Mat imgB = img.clone(); 
	
	for(int i = 100; i <= 200; i++){
	    cv::Vec3b* p_row = imgB.ptr<cv::Vec3b>(i);
	    for(int j = 350; j <= 440; j++){
	        p_row[j] = blackPixel;
	    }
	}
	
	cv::imshow("ImageB", imgB);
	
	// C
	
	
	 
	cv::Mat grayImg = img.clone(); 
	
	cv::cvtColor(img, grayImg, CV_RGB2GRAY);
	 
	for(int i = 100; i <= 200 ;i++) {
	    for(int j = 350; j <= 440; j++)
		    grayImg.at<uchar>(i,j) = 0;
	};
	
	cv::imshow("Gray Image", grayImg);
	 
    cv::waitKey();

    return 0;
}
