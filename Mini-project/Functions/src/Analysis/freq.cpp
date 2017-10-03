#include <opencv2/opencv.hpp>
#include <iostream>

void dftshift(cv::Mat& mag);

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv,
        "{@image | ./lena.bmp | image path}"
    );

    // Load image as grayscale
    std::string filename = parser.get<std::string>("@image");
    cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);

    if (img.empty()) {
        std::cout << "Input image not found at '" << filename << "'\n";
        return -1;
    }

    // Expand the image to an optimal size.
    cv::Mat padded;
    int opt_rows = cv::getOptimalDFTSize(img.rows * 2);
    int opt_cols = cv::getOptimalDFTSize(img.cols * 2);
    cv::copyMakeBorder(img, padded, 0, opt_rows - img.rows , 0, opt_cols - img.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));


    // Make place for both the real and complex values by merging planes into a
    // cv::Mat with 2 channels.
    // Use float element type because frequency domain ranges are large.
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat_<float>::zeros(padded.size())};
    cv::Mat complex;
    cv::merge(planes, 2, complex);

    // Compute DFT
    cv::dft(complex, complex);

    // Split real and complex planes (you can reuse the 'planes' array to save the result)
    cv::split(complex, planes);

    // Compute the magnitude and phase (see e.g. cv::cartToPolar)
    cv::Mat mag, phase;
    cv::cartToPolar(planes[0], planes[1], mag, phase);

    // Shift quadrants so the Fourier image origin is in the center of the image
    dftshift(mag);
  	
  	// Switch to logarithmic scale to be able to display on screen
    mag += cv::Scalar::all(1);
    cv::log(mag, mag);

    // Normalize for visualization
    cv::normalize(mag, mag, 0, 1, cv::NORM_MINMAX);
    //cv::normalize(phase, phase, 0, 1, cv::NORM_MINMAX);

    // Save images 
    cv::Mat magOut, phaseOut;
    mag.convertTo(magOut, CV_8U, 255);
    phase.convertTo(phaseOut, CV_8U, 255);
    cv::imwrite("freq_magnitude.bmp", magOut);
    cv::imwrite("freq_phase.bmp", phaseOut);
    
    return 0;
}

void dftshift(cv::Mat& mag)
{
    int cx = mag.cols / 2;
    int cy = mag.rows / 2;

    cv::Mat tmp;
    cv::Mat q0(mag, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(mag, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(mag, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(mag, cv::Rect(cx, cy, cx, cy));

    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}
