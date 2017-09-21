#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;


int main( int argc, char** argv ) 
{

	cv::namedWindow( "main", cv::WINDOW_AUTOSIZE );
	cv::Mat frame;
	cv::VideoCapture g_cap(0);

	while(true) 
	{

		g_cap >> frame; 

		cv::imshow( "main", frame );
		if (waitKey(30) >= 0)
			break;

	}
	return(0);
}