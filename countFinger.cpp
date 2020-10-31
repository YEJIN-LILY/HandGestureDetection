// ****** 이미지를 흑백으로 바꿔서 출력
// C:\Users\sue11\GRKim\ComputerVision\testProject\x64\Release> testProject.exe Images.jpg

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <string>
#include <math.h>
#include <stdio.h>

using namespace cv;
using namespace std;

#define PI 3.14159265

// HandGestureDetection


// 영상을 프레임별로 계속 가져오기
Mat getImage(Mat img);

// 피부 검출 및 이진화
Mat skinDetection(Mat img);

// 손바닥 검출
Point palmDetectuon(Mat img);

// 손가락 개수 세기
void countFinger(Mat img);



int main(int argc, char** argv)
{
	Mat image = imread("../../../handtest.jpg",0);
	resize(image, image, Size(500, 500));
	threshold(image, image, 100, 255, THRESH_BINARY);
	namedWindow("binary hand image");
	imshow("binary hand image", image);
	//image = getImage(image);

	//image = skinDetection(image);

	//image = palmDetectuon(image);

	countFinger(image);

	cout << "end" << endl;

	waitKey(0);

	return 0;
}

Point palmDetectuon(Mat img) {


}

void countFinger(Mat img) {

	Mat dst;
	img.copyTo(dst);
	
	

	//Mat image = imread("hand.jpg");

	//Point center = palmDetectuon(img); // input gray-scale image
	Point_<double> center((double)dst.size().width/2, (double)dst.size().height/2);
	cout << "손바닥 중심점 좌표:" << center << endl;
	
	//손바닥 중심점 그리기
	//circle(img, center, 2, Scalar(0, 255, 0), -1);

	double radius = 5.0;
	double x, y;
	bool stop;

	while (1) {

		// 원을 그린다
		stop = 0;
		//circle(img, center, radius, Scalar(255, 0, 0), 1, 8, 0);
		//imshow("count finger", img);
		//waitKey(50);
		//circle(img, center, radius, Scalar(255, 255, 255), 1, 8, 0); //remove

		//원이 그려진 곳에 검은색 화소가 있는지 검사
		for (int theta = 0; theta < 360; theta++) {
			
			x = (double)cos(theta * PI / 180) * radius + center.x;
			y = (double)sin(theta * PI / 180) * radius + center.y;
			//cout << "(x = " << x << ", y= " << y << ")  ";
			//cout << "value: " << img.at<uchar>(x, y) << endl;
			if (img.at<uchar>(x,y) == 0) { // 원이 그려진 곳에 검은색 화소가 있다면
				stop = 1;
				break; 
			}
		}

		if (stop) {
			break;
		}
		
		radius++;// 원 반지름 증가

		
	}
	// 실제 반지름 원
	//circle(img, center, radius, Scalar(0, 0, 255), 1, 8, 0); //red
	
	// 최종 원 그리기
	radius = radius * 1.5;
	circle(dst, center, radius, Scalar(0, 0, 255), 1, 8, 0); //red
	
	cout << "손바닥 중심점 좌표:" << center << ", 반지름:" << radius << endl;

	namedWindow("draw circle on hand");
	imshow("draw circle on hand", dst);


	

	// 손가락과 원이 겹치는 부분만 추출
	Mat circle_img = Mat::zeros(dst.size().width, dst.size().height, CV_8U);
	//circle(circle_img, center, radius, Scalar(255, 255, 255), 1, 8, 0); //red
	for (int theta = 0; theta < 360; theta++) {

		x = (double)cos(theta * PI / 180) * radius + center.x;
		y = (double)sin(theta * PI / 180) * radius + center.y;
		
		circle_img.at<uchar>(x, y) = 255;  // 원이 그려진 곳에 검은색 화소가 있다면
	}
	bitwise_and(img, circle_img, dst);

	namedWindow("& operate");
	imshow("& operate", dst);


	
	// 원 둘레를 돌며 손가락 카운트

	int pre_x = (double)cos(0 * PI / 180) * radius + center.x;
	int pre_y = (double)sin(0 * PI / 180) * radius + center.y;
	int count = 0;
	
	Mat test = Mat::zeros(dst.size().width, dst.size().height, CV_8UC1);
	dst.copyTo(test);
	cvtColor(dst, test, COLOR_GRAY2BGR);

	
	//Point th0(pre_x, pre_y);
	//circle(test, th0, 3, Scalar(0, 0, 255), 1, 8, 0); //red

	for (int theta = 1; theta < 360; theta++) {

		x = (double)cos(theta * PI / 180) * radius + center.x;
		y = (double)sin(theta * PI / 180) * radius + center.y;
		//cout << "(x = " << x << ", y= " << y << ")  ";
		//cout << "value: " << img.at<uchar>(x, y) << endl;
		//if ((img.at<uchar>(pre_x, pre_y) == 0) && (img.at<uchar>(x, y) == 255)) { // 이전 화소와 같이 다르다면
		//	img.at<uchar>(x, y) = 120;
		//	count++;
		//	
		//}
		//if (theta == 90) {
		//	Point th90(x, y);
		//	circle(test, th90, 3, Scalar(0, 255, 0), 1, 8, 0); //green
		//}
		//if (theta == 180) {
		//	Point th180(x, y);
		//	circle(test, th180, 3, Scalar(255, 0, 255), 1, 8, 0); //magenta
		//}
		//if (theta == 270) {
		//	Point th270(x, y);
		//	circle(test, th270, 3, Scalar(255, 255, 0), 1, 8, 0); //cyan
		//}

		

		printf("t = %d   (x, y) = %d  \n", theta, dst.at<uchar>(x, y));
		if (dst.at<uchar>(pre_x, pre_y) != dst.at<uchar>(x, y)) { // 이전 화소와 값이 다르다면
			//printf("---------------------red point (x, y) = (%d, %d)  value = %d", x, y, dst.at<uchar>(x, y));
			//test.at<Vec3b>(x, y)[2] = 255;
			
			Point th(x, y);
			circle(test, th, 3, Scalar(0, 0, 255), 1, 8, 0); //green

			//img.at<uchar>(x, y) = 120;
			count++;

		}
		pre_x = x;
		pre_y = y;
		imshow("test", test);

		waitKey(100);
	}
	
	cout << "count: " << count << endl;
	count = (count / 2) - 1;
	cout << "손가락 개수: " << count << endl;

}
