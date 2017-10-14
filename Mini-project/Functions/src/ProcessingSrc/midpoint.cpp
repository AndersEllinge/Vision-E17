#include <opencv2/opencv.hpp>
#include <algorithm>

cv::Mat midpoint(cv::Mat im, int size);
int clamp(int x, int min, int max);

int main( int argc, char** argv ) {

    // Parser setup
    cv::CommandLineParser parser(argc, argv,
        "{@image | ./lena.bmp | image path}"
        "{size | 3 | }"

    );

    // Load image as grayscale
    std::string filename = parser.get<std::string>("@image");
    cv::Mat src = cv::imread(filename, cv::IMREAD_GRAYSCALE);

    if (src.empty()) {
        std::cout << "Input image not found at '" << filename << "'\n";
        return -1;
    }

    // Do filtering
    int size = parser.get<int>("size");
    std::cout << "Size: " << size << std::endl;
    cv::Mat out = midpoint(src, size);

    cv::imwrite("processed.bmp", out);


    return 0;
}

int clamp(int x, int min, int max) {

	if(x < min)
		return min;
	if(x > max)
		return max;
	return x;
}

cv::Mat midpoint(cv::Mat im, int size) {
	double min, max;
	int val;
	cv::Mat out;
	out = im.clone();

	for (int x = 0; x < im.rows; x++) {
		for (int y = 0; y < im.cols; y++) {
			int xPointS = clamp(x - (size/2), 0, im.rows - 1);
			int yPointS = clamp(y - (size/2), 0, im.cols - 1);
			int xPointE = clamp(x + (size/2), 0, im.rows - 1);
			int yPointE = clamp(y + (size/2), 0, im.cols - 1);

			cv::Point p1 = cv::Point(yPointS, xPointS);
			cv::Point p2 = cv::Point(yPointE, xPointE);

			cv::Mat tmp = im(cv::Rect(p1, p2)).clone();
			cv::minMaxLoc(tmp, &min, &max);
			val = (int)(min+max)/2;
			out.at<uchar>(x, y) = val;
		}
	}

	return out;
}
