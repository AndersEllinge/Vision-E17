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

float distance(int u, int v, int P, int Q)
{
    float distance = sqrt(pow(u-(P/2),2) + pow(v-(Q/2),2));

    return distance;
}

cv::Mat butterWorthHighPass(cv::Size mag, int order, int d0)
{
    cv::Mat_<cv::Vec2f> complex(mag);
      for (int i = 0; i < complex.rows; i++)
        for (int j = 0; j < complex.cols; j++){
          complex(i,j)[0] = 1/(1+pow(d0/distance(i,j,complex.rows,complex.cols),2*order));
          complex(i,j)[1] = 0;
        }
    return complex;
}

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

    // Expand the image to an optimal size.
    cv::Mat padded;
    int opt_rows = cv::getOptimalDFTSize(img.rows);
    int opt_cols = cv::getOptimalDFTSize(img.cols);
    cv::copyMakeBorder(img, padded, 0, opt_rows - img.rows , 0, opt_cols - img.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));


    // Make place for both the real and complex values by merging planes into a
    // cv::Mat with 2 channels.
    // Use float element type because frequency domain ranges are large.
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat_<float>::zeros(padded.size())};
    cv::Mat complex;
    cv::merge(planes, 2, complex);

    // Compute DFT
    cv::dft(complex,complex);

    //shift the dft
    dftshift(complex);
    cv::Mat butter = butterWorthHighPass(complex.size(),2,250);

    //apply filter to spectrum
    cv::mulSpectrums(butter, complex, complex, 0);

    // Shift back quadrants of the spectrum (use dtfshift)
    dftshift(complex);

    // Restore by performing inverse DFT (see cv::dft with the DFT_INVERSE flag)
    cv::Mat filtered; // the resulting filtered image
    dft(complex,filtered, cv::DFT_INVERSE|cv::DFT_REAL_OUTPUT);

    // Normalize and show filtered image
    cv::normalize(filtered, filtered, 0, 1, cv::NORM_MINMAX);
    cv::imshow("Filtered image", filtered);
    //cv::imshow("Butter filter", butter);
    cv::waitKey();


    cv::waitKey();
    return 0;
}
