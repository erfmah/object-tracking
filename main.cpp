#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <string.h>
#include <stdlib.h>
#include <cmath>

using namespace std;
using namespace cv;

int fd;
double degree = 0;
double getSpeed(double dist)
{
	return ((dist / 100.0)*10.0) + 1;
}
int open_port(void)
{
  int fd; /* File descriptor for the port */


  fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1)
  {
   /*
    * Could not open the port.
    */

    perror("open_port: Unable to open /dev/ttyf1 - ");
  }
  else
    fcntl(fd, F_SETFL, 0);

  return (fd);
}
int rotate_camera(int delta)
{
	//int fd = open_port();
	if (!((degree + delta < 0) || (degree + delta > 180)))
		degree += delta;
	char s[13];
	sprintf(s, "%d", delta);
	strcat(s, "\n");
	int n = write(fd, s, strlen(s));
	if (n < 0)
	  fputs("write() of 4 bytes failed!\n", stderr);
	return 0;
}
void init() 
{
	for (int i = 0; i < 180; ++i)
	{
		rotate_camera(-1);
	}
	fd = open_port();
	cv::namedWindow( "main", cv::WINDOW_AUTOSIZE );
}

int main( int argc, char** argv ) 
{
	init();
	degree = 0;
	// cv::namedWindow( "gaussian blur", cv::WINDOW_AUTOSIZE );
	// cv::namedWindow( "hsv", cv::WINDOW_AUTOSIZE );
	// cv::namedWindow( "inRange", cv::WINDOW_AUTOSIZE );
	// cv::namedWindow( "Erode", cv::WINDOW_AUTOSIZE );
	// cv::namedWindow( "Dilate", cv::WINDOW_AUTOSIZE );
	// cv::namedWindow( "Countours", cv::WINDOW_AUTOSIZE );

	double w, h;
	cv::Mat frame;
	cv::VideoCapture g_cap(1);

	Scalar greenLower(29, 86, 6);
	Scalar greenUpper(64, 255, 255);

	double ballsize = 2500;
	RNG rng(12345);
 	Scalar color = Scalar( 0, 0, 255);
	
 	vector<Point2f> movingPath;
 	Point2f lastDetect(1, 0);
 	bool founded = false;

	while(true) 
	{
		founded = false;
		g_cap >> frame;
		w = frame.cols;
		h = frame.rows; 
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
			lastDetect = center[i];
			founded = true;


			double rotated = (w / 2) - center[0].x;

			double thresh = 20;
			//double speed = getSpeed(abs(rotated));
			//cout << rotated << " ***** " << speed << endl;
			cout << center[i].x << endl;
			if (rotated < -1 * thresh){
				rotate_camera(-1);
				cout<<"rotate left";
			}
			if (rotated > thresh){
				rotate_camera(1);
				cout<<"rotate right";
			}
				
		}
		int turn = 5;
		if (!founded) {
			if ((degree > 180 - turn || degree < turn)){
				// turn = -1 * turn;
				lastDetect.x = lastDetect.x * -1;
			}
			if(lastDetect.x > 0){
				rotate_camera(turn);
			}
			else if(lastDetect.x < 0){
				rotate_camera(-1 * turn);
			}
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