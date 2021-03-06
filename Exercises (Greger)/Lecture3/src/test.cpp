/*
 * RoVi1
 * Exercise 3: The Frequency domain and filtering
 */

// e17-1-gbe848aa

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

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
    // cv::copyMakeBorder(img, padded, ...)

    // Make place for both the real and complex values by merging planes into a
    // cv::Mat with 2 channels.
    // Use float element type because frequency domain ranges are large.
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat_<float>::zeros(padded.size())};
    cv::Mat complex;
    // ...

    // Compute DFT
    // ...

    // Split real and complex planes (you can reuse the 'planes' array to save the result)
    // ...

    // Compute the magnitude and phase (see e.g. cv::cartToPolar)
    cv::Mat mag, phase;
    // ...

    // Shift quadrants so the Fourier image origin is in the center of the image
    // (see above dtfshift function)
    // ...


    // Do high-pass filtering if the '--filter' commandline flag was set
    // (part 2. of the exercise)
    if (parser.has("filter")) {
        // Remove the low frequency part in the middle of the spectrum (by setting
        // it to zero in a square at the center)
        // ...

        // Shift back quadrants of the spectrum (use dtfshift)
        // ...

        // Compute complex DFT planes from magnitude/phase (see e.g. cv::polarToCart)
        // ...

        // Merge into one image (you can reuse the 'complex' image to save the result)
        // ...

        // Restore by performing inverse DFT (see cv::dft with the DFT_INVERSE flag)
        cv::Mat filtered; // the resulting filtered image
        // ...

        // Normalize and show filtered image
        cv::normalize(filtered, filtered, 0, 1, cv::NORM_MINMAX);
        cv::imshow("Filtered image", filtered);
    }

    // Switch to logarithmic scale to be able to display on screen
    mag += cv::Scalar::all(1);
    cv::log(mag, mag);

    // Transform the matrix with float values into a viewable image form (float
    // values between 0 and 1) and show the result.
    cv::normalize(mag, mag, 0, 1, cv::NORM_MINMAX);
    // cv::normalize(phase, phase, 0, 1, cv::NORM_MINMAX);
    cv::imshow("Input image", img);
    //cv::imshow("Magnitude", mag);
    // cv::imshow("Phase", phase);

    cv::waitKey();
    return 0;
}
