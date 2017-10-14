#include <opencv2/opencv.hpp>
#include <iostream>

cv::Mat adaptiveMedian(cv::Mat im, int S_Min, int S_Max);
int clamp(int x, int min, int max);
double median(cv::Mat im);

int main( int argc, char** argv ) {
    cv::CommandLineParser parser(argc, argv,
        "{help   |            | print this message}"
        "{@image | ./book.jpg | image path}"
        "{S_Min       | | }"
        "{S_Max       | | }"
    );

    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }

    // Load image
    std::string filename = parser.get<std::string>("@image");
    cv::Mat im = cv::imread(filename,0);

    if (im.empty()) {
        std::cout << "Input image not found at '" << filename << "'\n";
        return 1;
    }

    // Filter
    cv::Mat output;
    output = adaptiveMedian(im, parser.get<int>("S_Min"), parser.get<int>("S_Max"));

    // Save and show
    //cv::imshow("Result from median filtering", output);
    cv::imwrite("processedAdapMedian.bmp", output);
    return 0;
}

int clamp(int x, int min, int max) {

	if(x < min)
		return min;
	if(x > max)
		return max;
	return x;
}

cv::Mat adaptiveMedian(cv::Mat im, int S_Min ,int S_Max) {
double min, max;
cv::Mat out;
out = im.clone();
bool repeatA = true;
  for (int x = 0; x < im.rows; x++) {
		for (int y = 0; y < im.cols; y++) {
      int S = S_Min; //new pixel reset size.

      while (repeatA) {
        repeatA = false;

        int xPointS = clamp(x - (S/2), 0, out.rows - 1);
  			int yPointS = clamp(y - (S/2), 0, out.cols - 1);
  			int xPointE = clamp(x + (S/2), 0, out.rows - 1);
  			int yPointE = clamp(y + (S/2), 0, out.cols - 1);

      	cv::Point p1 = cv::Point(yPointS, xPointS);
  			cv::Point p2 = cv::Point(yPointE, xPointE);

  			cv::Mat tmp = im(cv::Rect(p1, p2)).clone();
  			cv::minMaxLoc(tmp, &min, &max);
        double med = median(tmp);
        double A1 = med - min;
        double A2 = med - max;

        if (A1 > 0 && A2 < 0) {
          double B1 = out.at<uchar>(x, y) - min;
          double B2 = out.at<uchar>(x, y) - max;
          if (!(B1 > 0 && B2 < 0))
            out.at<uchar>(x, y) = med;
        }
        else{
          if (S < S_Max){
            S = S + 2;
            repeatA = true;
          }
          else{
            out.at<uchar>(x, y) = med;
          }
        }
      }
      repeatA = true;
		}
	}
return out;
}

double median(cv::Mat im){

  double m = (im.rows*im.cols)/2;
  int bin = 0;
  double med = -1.0;

  int histSize = 256;
  float range[] = {0 , 256};
  const float* histRange = {range};
  cv::Mat hist;
  cv::calcHist(&im, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange,1,0);

  for (int i = 0; i < histSize && med < 0.0; ++i) {
    bin += cvRound(hist.at< float >(i));
    if (bin > m && med < 0.0)
      med = i;
  }
return med;
}
