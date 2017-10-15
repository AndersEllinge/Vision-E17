#include <opencv2/opencv.hpp>
#include <iostream>

void dftshift(cv::Mat& mag);
cv::Mat butterHP(cv::Size mag, int order, int d0);
cv::Mat butterLP(cv::Size mag, int order, int d0);

int main(int argc, char* argv[]) {

    cv::CommandLineParser parser(argc, argv,
        "{@image | ./lena.bmp | image path}"
        "{d | 7 | }"
        "{sC | 40 | }"
        "{sS | 40 | }"
    );
    
    // Load image as grayscale
    std::string filename = parser.get<std::string>("@image");
    cv::Mat im = cv::imread(filename, cv::IMREAD_GRAYSCALE);

    if (im.empty()) {
        std::cout << "Input image not found at '" << filename << "'\n";
        return -1;
    }
    
    cv::Mat output;
    cv::bilateralFilter(im, output, parser.get<int>("d"), parser.get<double>("sC"), parser.get<double>("sS"));
    
    cv::imwrite("processed.bmp", output);
    
}
