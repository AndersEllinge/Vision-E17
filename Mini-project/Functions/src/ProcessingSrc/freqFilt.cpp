#include <opencv2/opencv.hpp>
#include <iostream>

void dftshift(cv::Mat& mag);
cv::Mat butterHP(cv::Size mag, int order, int d0);
cv::Mat butterLP(cv::Size mag, int order, int d0);
cv::Mat butterBS(cv::Size mag, int order, int d0, float width);

int main(int argc, char* argv[]) {

    cv::CommandLineParser parser(argc, argv,
        "{@image | ./lena.bmp | image path}"
        "{integer |  | }"
        "{float |  | }"
        "{buttBS |  | }"
        "{order_1 |  | }"
        "{d0_1 |  | }"
        "{order_2 |  | }"
        "{d0_2 |  | }"
        "{d0 |  | }"
        "{d |  | }"
        "{width |  | }"
    );

    if (parser.has("float")) {
    	std::cout << "float was set to " << parser.get<float>("float") << std::endl;
    }

    if (parser.has("integer")) {
    	std::cout << "integer was set to " << parser.get<int>("integer") << std::endl;
    }

    // Load image as grayscale
    std::string filename = parser.get<std::string>("@image");
    cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);

    if (img.empty()) {
        std::cout << "Input image not found at '" << filename << "'\n";
        return -1;
    }

    // Expand the image to an optimal size
    cv::Mat padded;
    int opt_rows = cv::getOptimalDFTSize(img.rows * 2);
    int opt_cols = cv::getOptimalDFTSize(img.cols * 2);
    cv::copyMakeBorder(img, padded, 0, opt_rows - img.rows , 0, opt_cols - img.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    // Create mat for DFT
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat_<float>::zeros(padded.size())};
    cv::Mat complex;
    cv::merge(planes, 2, complex);

    // Compute DFT and shift
    cv::dft(complex, complex);
    dftshift(complex);

    // If butterworth HP filter
    if (parser.has("buttHP") && parser.has("order_1") && parser.has("d0")) {

    	// Generate butterworth filter
    	cv::Mat filterHP = butterHP(complex.size(), parser.get<int>("order_1"), parser.get<int>("d0"));
      cv::Mat magn, angl, magOut;
      cv::Mat planesButtHP[2];
      /*
      cv::split(filterHP, planesButtHP);
      cv::cartToPolar(planesButtHP[0], planesButtHP[1], magn, angl);
      magn += cv::Scalar::all(1);
      cv::log(magn, magn);
      cv::normalize(magn, magn, 0, 1, cv::NORM_MINMAX);
      magn.convertTo(magOut, CV_8U, 255);
      cv::imwrite("MagnitudeHP.bmp", magOut);*/


    	// Apply filter to dft
    	cv::mulSpectrums(filterHP, complex, complex, 0);


      cv::split(complex, planesButtHP);
      cv::cartToPolar(planesButtHP[0], planesButtHP[1], magn, angl);
      magn += cv::Scalar::all(1);
      cv::log(magn, magn);
      cv::normalize(magn, magn, 0, 1, cv::NORM_MINMAX);
      magn.convertTo(magOut, CV_8U, 255);
      cv::imwrite("MagnitudeFreqWithHP.bmp", magOut);

    	// Shift back quadrants
    	dftshift(complex);

    	// Restore image with reverse DFT
    	cv::Mat filtered;
    	dft(complex,filtered, cv::DFT_INVERSE|cv::DFT_REAL_OUTPUT);

    	// Normalize and convert to grayscale
    	cv::normalize(filtered, filtered, 0, 1, cv::NORM_MINMAX);
    	cv::Mat filteredOut;
    	filtered.convertTo(filteredOut, CV_8U, 255);

      //crop padding
      cv::Rect rec(0,0,img.cols,img.rows);
      cv::Mat cropImg = filteredOut(rec);

    	// Save image
    	cv::imwrite("processed.bmp", cropImg);
    	return 0;

    }

    if (parser.has("buttBS") && parser.has("order_1")  && parser.has("d0") && parser.has("width")) {

    	// Generate butterworth filter
    	cv::Mat filterBS = butterBS(complex.size(), parser.get<int>("order_1"), parser.get<int>("d0"),parser.get<int>("width"));

      cv::Mat magn, angl, magOut;
      cv::Mat planesButtBS[2];
      cv::split(filterBS, planesButtBS);
      cv::cartToPolar(planesButtBS[0], planesButtBS[1], magn, angl);
      magn += cv::Scalar::all(1);
      cv::log(magn, magn);
      cv::normalize(magn, magn, 0, 1, cv::NORM_MINMAX);

      magn.convertTo(magOut, CV_8U, 255);
      cv::imwrite("MagnitudeBS.bmp", magOut);

    	// Apply filter to dft
    	cv::mulSpectrums(filterBS, complex, complex, 0);

      cv::split(complex, planesButtBS);
      cv::cartToPolar(planesButtBS[0], planesButtBS[1], magn, angl);
      magn += cv::Scalar::all(1);
      cv::log(magn, magn);
      cv::normalize(magn, magn, 0, 1, cv::NORM_MINMAX);
      magn.convertTo(magOut, CV_8U, 255);
      cv::imwrite("MagnitudeFreqWithBS.bmp", magOut);

    	// Shift back quadrants
    	dftshift(complex);

    	// Restore image with reverse DFT
    	cv::Mat filtered;
    	dft(complex,filtered, cv::DFT_INVERSE|cv::DFT_REAL_OUTPUT);

    	// Normalize and convert to grayscale
    	cv::normalize(filtered, filtered, 0, 1, cv::NORM_MINMAX);
    	cv::Mat filteredOut;
    	filtered.convertTo(filteredOut, CV_8U, 255);

      //crop padding
      cv::Rect rec(0,0,img.cols,img.rows);
      cv::Mat cropImg = filteredOut(rec);

    	// Save image
    	cv::imwrite("processed.bmp", cropImg);
    	return 0;

    }


    std::cout << "No filter compatible with imputs" << std::endl;

    return 0;
}

void dftshift(cv::Mat& mag) {
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

cv::Mat butterHP(cv::Size size, int order, int d0) {
    cv::Mat_<cv::Vec2f> complex(size);
    for (int i = 0; i < complex.rows; i++) {
    	for (int j = 0; j < complex.cols; j++){
        	float distance = sqrt(pow(i-(complex.rows/2),2) + pow(j-(complex.cols/2),2));
        	complex(i,j)[0] = 1/(1+pow(d0/distance,2*order));
        	complex(i,j)[1] = 0;
    	}
	}
    return complex;
}

cv::Mat butterLP(cv::Size size, int order, int d0) {
  cv::Mat_<cv::Vec2f> complex(size);
  for (int i = 0; i < complex.rows; i++) {
    for (int j = 0; j < complex.cols; j++){
        float distance = sqrt(pow(i-(complex.rows/2),2) + pow(j-(complex.cols/2),2));
        complex(i,j)[0] = 1/(1+pow(distance/d0,2*order));
        complex(i,j)[1] = 0;
    }
}
  return complex;
}

cv::Mat butterBS(cv::Size size, int order, int d0, float width) {
  cv::Mat_<cv::Vec2f> complex(size);
  for (int i = 0; i < complex.rows; i++) {
    for (int j = 0; j < complex.cols; j++){
        float distance = sqrt(pow(i-(complex.rows/2),2) + pow(j-(complex.cols/2),2));
        complex(i,j)[0] = 1/(1+pow((distance*width)/pow(distance - d0,2),2*order));
        complex(i,j)[1] = 0;
    }
}
  return complex;
}
