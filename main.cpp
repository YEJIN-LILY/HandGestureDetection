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
Mat getImage(const Mat& img) {
	Mat result;

	medianBlur(img, result, 5);

	return result;
}

// 피부 검출 및 이진화
Mat skinDetection(const Mat& image, int minCr = 128, int maxCr = 170, int minCb = 73, int maxCb = 158) {
	Mat YCrCb;
	cvtColor(image, YCrCb, COLOR_BGR2YCrCb);

	vector<Mat> planes;

	split(YCrCb, planes);

	Mat mask(image.size(), CV_8U, Scalar(0));   //결과 마스크를 저장할 영상

	int nr = image.rows; 
	int nc = image.cols;

	for (int i = 0; i < nr; i++) {
		uchar* CrPlane = planes[1].ptr<uchar>(i);   //Cr채널의 i번째 행 주소
		uchar* CbPlane = planes[2].ptr<uchar>(i);   //Cb채널의 i번째 행 주소

		for (int j = 0; j < nc; j++) {
			if ((minCr < CrPlane[j]) && (CrPlane[j] < maxCr) && (minCb < CbPlane[j]) && (CbPlane[j] < maxCb))
				mask.at<uchar>(i, j) = 255;
		}
	}
	erode(mask, mask, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);

	return mask;
}
// 손바닥 검출
Mat palmDetectuon(Mat img);

// 손가락 개수 세기
Mat countFinger(Mat img);



int main(int argc, char** argv)
{
	Mat image;
	
	VideoCapture cap;
	
	int deviceID = 0;            
	int apiID = cv::CAP_ANY;     
	cap.open(deviceID + apiID); 
	if (!cap.isOpened()) { cerr << "ERROR! Unable to open camera\n"; return -1; }
	cout << "Start grabbing" << endl << "Press any key to terminate" << endl; 
	for (;;) {
		cap.read(image); 
		if (image.empty()) { cerr << "ERROR! blank frame grabbed\n"; break; } 
		namedWindow("webcam Image");
		imshow("webcam Image", image); //live image test(나중에 삭제)
		image=getImage(image);
		image=skinDetection(image);
		namedWindow("binary Image");
		imshow("binary Image", image);
		if (waitKey(5) >= 0) break; 
		
		image = palmDetectuon(image);
		namedWindow("Palm Image");
		imshow("Palm Image", image);

		image = countFinger(image);
		namedWindow("Finger Image");
		imshow("Finger Image", image);

	} 


	
	return 0;
}
