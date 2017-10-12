#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv,
        "{help   |            | print this message}"
        "{@image | ./lena.bmp | image path}"
        "{t 	 |     128    | treshold}"
    );

    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }

    // Load image as grayscale
    std::string filename = parser.get<std::string>("@image");
    cv::Mat im = cv::imread(filename, cv::IMREAD_GRAYSCALE);

    if (im.empty()) {
        std::cout << "Input image not found at '" << filename << "'\n";
        return 1;
    }
    
    int t = parser.get<int>("t");
    
    // Create binary of input im
    cv::Mat binary;
    cv::threshold(im, binary, t, 255, CV_THRESH_BINARY);

    
    return 0;
}
