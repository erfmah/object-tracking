#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;


int main( int argc, char** argv ) 
{

	cv::namedWindow( "main", cv::WINDOW_AUTOSIZE );
	// cv::namedWindow( "gaussian blur", cv::WINDOW_AUTOSIZE );
	// cv::namedWindow( "hsv", cv::WINDOW_AUTOSIZE );
	// cv::namedWindow( "inRange", cv::WINDOW_AUTOSIZE );
	// cv::namedWindow( "Erode", cv::WINDOW_AUTOSIZE );
	// cv::namedWindow( "Dilate", cv::WINDOW_AUTOSIZE );
	// cv::namedWindow( "Countours", cv::WINDOW_AUTOSIZE );


	cv::Mat frame;
	cv::VideoCapture g_cap(0);

	Scalar greenLower(29, 86, 6);
	Scalar greenUpper(64, 255, 255);

	double ballsize = 2500;
	RNG rng(12345);
 	Scalar color = Scalar( 0, 0, 255);
	
 	vector<Point2f> movingPath;
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
		dilate(eroded, dilated, element);

		vector< vector<Point> > contours;
		findContours(dilated, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

		// you could also reuse img1 here
		Mat contoursDrawed = Mat::zeros(dilated.rows, dilated.cols, CV_8UC1);

		vector< vector<Point> > mainConts;
		for (int i = 0; i < contours.size(); ++i)
		{
			if (contourArea(contours[i]) > ballsize) 
			{
				mainConts.push_back(contours[i]);
			}
		}
		drawContours(contoursDrawed, mainConts, -1, Scalar(255), CV_FILLED);

		vector<vector<Point> > contours_poly( mainConts.size() );
		vector<Rect> boundRect( mainConts.size() );
		vector<Point2f>center( mainConts.size() );
		vector<float>radius( mainConts.size() );
		for( int i = 0; i < mainConts.size(); i++ )
		{
			approxPolyDP( Mat(mainConts[i]), contours_poly[i], 3, true );
			boundRect[i] = boundingRect( Mat(contours_poly[i]) );
			minEnclosingCircle( contours_poly[i], center[i], radius[i] );
			circle( frame, center[i], (int)radius[i], color, 2, 8, 0 );
			movingPath.push_back(center[i]);
		}
		if (movingPath.size() > 2)
			for (int i = 0; i < movingPath.size() - 1; ++i)
			{
				float thikness = (sqrt((float)(i + 1) / 2.0) * 2.5);
				line(frame, movingPath[i], movingPath[i + 1], color, thikness);
			}
    	
    	while(movingPath.size() > 10)
    		movingPath.erase(movingPath.begin());

		cv::imshow( "main", frame );
		// cv::imshow( "gaussian blur", gaussianBlurred );
		// cv::imshow( "hsv", HSVframe );
		// cv::imshow( "inRange", inRangeOfGreen );
		// cv::imshow( "Erode", eroded );
		// cv::imshow( "Dilate", dilated );
		// cv::imshow( "Countours", contoursDrawed );

		if (waitKey(30) >= 0)
			break;

	}
	return(0);
}