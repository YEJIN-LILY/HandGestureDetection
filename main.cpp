#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include<opencv2/imgproc.hpp>
#include<cmath>
using namespace cv;
using namespace std;

// HandGestureDetection


// 영상을 프레임별로 계속 가져오기
Mat getImage(Mat img);

// 피부 검출 및 이진화
Mat skinDetection(Mat img);

// 손바닥 검출
Mat palmDetectuon(Mat img);

// 손가락 개수 세기
Mat countFinger(Mat img);



int main(int argc, char** argv)
{
	Mat image;

	image = getImage(image);
	namedWindow("webcam Image");
	imshow("webcam Image", image);

	image = skinDetection(image);
	namedWindow("binary Image");
	imshow("binary Image", image);

	image = palmDetectuon(image);
	namedWindow("Palm Image");
	imshow("Palm Image", image);

	image = countFinger(image);
	namedWindow("Finger Image");
	imshow("Finger Image", image);

	return 0;
}
