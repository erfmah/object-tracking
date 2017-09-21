#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;


int main( int argc, char** argv ) 
{

	cv::namedWindow( "main", cv::WINDOW_AUTOSIZE );
	cv::namedWindow( "gaussian blur", cv::WINDOW_AUTOSIZE );
	cv::namedWindow( "hsv", cv::WINDOW_AUTOSIZE );
	cv::namedWindow( "inRange", cv::WINDOW_AUTOSIZE );
	cv::namedWindow( "Erode", cv::WINDOW_AUTOSIZE );
	cv::namedWindow( "Dilate", cv::WINDOW_AUTOSIZE );


	cv::Mat frame;
	cv::VideoCapture g_cap(0);

	Scalar greenLower(29, 86, 6);
	Scalar greenUpper(64, 255, 255);
	while(true) 
	{

		g_cap >> frame	; 


		Mat gaussianBlurred;
		GaussianBlur(frame, gaussianBlurred, Size(9, 9), 0, 0);

		Mat HSVframe;
		cvtColor(gaussianBlurred, HSVframe, CV_BGR2HSV);

		Mat inRangeOfGreen;
		inRange(HSVframe, greenLower, greenUpper, inRangeOfGreen);


		int erosion_size = 6;  
      Mat element = getStructuringElement(cv::MORPH_CROSS,
              cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
              cv::Point(erosion_size, erosion_size) );
		Mat eroded;
		erode(inRangeOfGreen, eroded, element);

		Mat dilated;
		erode(inRangeOfGreen, dilated, element);


		cv::imshow( "main", frame );
		cv::imshow( "gaussian blur", gaussianBlurred );
		cv::imshow( "hsv", HSVframe );
		cv::imshow( "inRange", inRangeOfGreen );
		cv::imshow( "Erode", eroded );
		cv::imshow( "Dilate", dilated );

		if (waitKey(30) >= 0)
			break;

	}
	return(0);
}