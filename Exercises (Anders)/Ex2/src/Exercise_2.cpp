/*
 * RoVi1
 * Example application to load and display an image
 */

// v1.0-4-gdfe246a

#include <opencv2/opencv.hpp>
#include <iostream>

void fAdd(cv::Mat input, int variable)
{
  for (size_t i = 0; i < input.rows; i++)
    for (size_t j = 0; j < input.cols; j++)
      input.at<uchar>(i,j) = cv::saturate_cast<uchar>(input.at<uchar>(i,j) + variable);

}

void drawLine( cv::Mat img, cv::Point start, cv::Point end )
{
    int thickness = 1;
    int lineType = 8;
    cv::line( img, start, end,
          cv::Scalar( 255, 255, 255),
          thickness,
          lineType );
}


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
    cv::Mat img = cv::imread(filename,0);

    if (img.empty()) {
        std::cout << "Input image not found at '" << filename << "'\n";
        return 1;
    }

    cv::Mat transformImg = img.clone();
/*
1. Intensity transformations
  - Loop through each pixel value x in the image and apply the function f(x)=x+50.
    Remember to handle overflow, (look at the saturate_cast function).
  - Visualize the original image and the transformed image.
*/

    fAdd(transformImg,50);
    cv::imshow("Original", img);
    cv::imshow("Transformed Image", transformImg);

/*
2. Histogram calculation
  - Use calcHist to calculate an intensity histogram of the input grayscale image
    and the intensity-transformed image.
  - Implement histogram equalization. Use Gonzalez and Woods Eq. 3.3-8
    (also checkout equalizeHist).
  - Visualize the histograms.
  - See the histogram calculation and histogram equalization tutorials
    for inspiration. You will have to modify the code to single-channel images.
*/

      // Initialize parameters
      int histSize = 256;    // bin size
      float range[] = { 0, 256 };
      const float *ranges[] = { range };

      // Calculate histogram
      cv::Mat hist;
	cv::Mat histNorm;
      cv::calcHist( &img, 1, 0, cv::Mat(), hist, 1, &histSize, ranges, true, false );

      // Plot the histogram
      int hist_w = 512; int hist_h = 256;
      int bin_w = cvRound( (double) hist_w/histSize );

      cv::Mat histImg = cv::Mat::zeros( hist_h, hist_w, 0 );
      cv::normalize(hist, histNorm, 0, histImg.rows, cv::NORM_MINMAX, -1);

      for (size_t i = 0; i < histSize; i++){
        drawLine(histImg, cv::Point((i)*bin_w, hist_h), 
			  cv::Point((i)*bin_w, hist_h - histNorm.at<float>(0,i)));
      }

      cv::imshow("Histogram", histImg);

      // create CDF
      float sum = 0;
      cv::Mat histCDF = hist.clone();
      for (size_t i = 0; i < histSize; i++) {
        sum += 255*(hist.at<float>(i)/(img.rows*img.cols));
        histCDF.at<float>(i) = sum;
      }

      // normalize it
      //cv::normalize(histCDF, histCDF, 0, histImg.rows, cv::NORM_MINMAX, -1);
	
      //plot CDF
      cv::Mat CDF = cv::Mat::zeros( hist_h, hist_w, 0 );

      for (size_t i = 0; i < hist.rows; i++){
        drawLine(CDF, cv::Point((i)*bin_w, hist_h), 
		      cv::Point((i)*bin_w, hist_h - histCDF.at<float>(i)));
      }
      cv::imshow("CDF", CDF);

      //apply CDF as transform to image
      cv::Mat EQImg = img.clone();

      for (size_t i = 0; i < EQImg.rows; i++) {
        for (size_t j = 0; j < EQImg.cols; j++) {
            EQImg.at<uchar>(i,j) = histCDF.at<float>(img.at<uchar>(i,j));
        }
      }
      cv::imshow("Equalized image", EQImg);

      // Making a histgram from the equalized image
      // Calculate histogram
      cv::Mat histCDFImg;
      cv::calcHist( &EQImg, 1, 0, cv::Mat(), histCDFImg, 1, &histSize, ranges, true, false );

      // Plot the histogram
      cv::normalize(histCDFImg, histCDFImg, 0, 256, cv::NORM_MINMAX, -1);
      cv::Mat histImg1 = cv::Mat::zeros( hist_h, hist_w, 0 );

      for (size_t i = 0; i < histSize; i++){
        drawLine(histImg1, cv::Point((i)*bin_w, hist_h), 
			   cv::Point((i)*bin_w, hist_h - histCDFImg.at<float>(i)));
      }

      cv::imshow("Histogram of the equalized image", histImg1);
/*
3. Linear filtering
  - Get to know how filtering works in OpenCV using filter2D.
  - Create a 3-by-3 box filter kernel and apply it to the input image.
  - Create your own filtering function (you cannot use filter2D) that does the following:
      - For each pixel, sort the 9 pixel values of the 3-by-3 neighborhood
        (including the pixel itself).
      - Take the mean of the three middle values and store that as the filtered pixel value.
      - Since border pixels do not have 9 neighbors, you can either use
        padding or offset your algorithm to avoid the borders.
  - Visualize the input and the two filtered images.
*/

      cv::Mat A = (cv::Mat_<int>(3, 3) << -1, -1,-1, -1, 8, -1, -1, -1, -1);
      std::cout << A << std::endl;


    cv::waitKey();

    return 0;
}
