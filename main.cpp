
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
Mat getImage(const Mat& img);

// 피부 검출 및 이진화
Mat skinDetection(const Mat& image, int minCr = 133, int maxCr = 173, int minCb = 77, int maxCb = 127);

// 손바닥 검출
Point palmDetection(Mat img);


void countFinger(Mat img, Point palm);

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



		image = getImage(image);
		image = skinDetection(image);
		namedWindow("binary Image");
		imshow("binary Image", image);




		Point palm;
		palm = palmDetection(image);

		//namedWindow("Palm Image");
		//imshow("Palm Image", image);

		countFinger(image, palm);
		//namedWindow("Finger Image");
		//imshow("Finger Image", image);

		if (waitKey(5) >= 0) break;
	}





	cout << "end" << endl;

	waitKey(0);

	return 0;
}


// 영상을 프레임별로 계속 가져오기
Mat getImage(const Mat& img) {
	Mat result;

	medianBlur(img, result, 5);

	return result;
}

// 피부 검출 및 이진화
Mat skinDetection(const Mat& image, int minCr, int maxCr, int minCb, int maxCb) {
	
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
	dilate(mask, mask, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 1);

	
	return mask;
}

// 손바닥 검출
Point palmDetection(Mat img)
{
	Mat before;
	Mat after;

	before = img.clone();
	
	bool allBlack = true;
	

	while (allBlack)
	{

		erode(before, after, Mat());

		//imshow("before", before);
		//imshow("after", after);

		for (int j = 0; j < img.rows; j++)
		{
			//j열의 주소(nc개만큼) 가져오기
			uchar* data = after.ptr<uchar>(j);
			for (int i = 0; i < img.cols; i++)
			{
				if (data[i]) allBlack = false;
			}

		}



		//waitKey(100);

		if (allBlack)
		{
			//imshow("return", before);
			// 가운데 점 계산

			int col = 0;
			int row = 0;
			int count = 0;

			for (int j = 0; j < img.rows; j++)
			{
				//j열의 주소(nc개만큼) 가져오기
				uchar* data = before.ptr<uchar>(j);
				for (int i = 0; i < img.cols; i++)
				{
					if (data[i])
					{
						count++;
						row += j;
						col += i;
					}
				}

			}

			row = (int)(row / count);
			col = (int)(col / count);

			Point palm(col, row);

			//after.at<uchar>(row, col) = 255;
			cvtColor(img, img, COLOR_GRAY2BGR);

			circle(img, palm, 5, Scalar(123, 255, 123), -1);

			namedWindow("Palm Image");
			imshow("Palm Image", img);



			return palm;
		}

		allBlack = true;
		before = after.clone();
	}


}



// 손가락 개수 세기
void countFinger(Mat img, Point palm) {

	Mat dst;
	img.copyTo(dst);



	//Mat image = imread("hand.jpg");

	//Point center = palmDetectuon(img); // input gray-scale image
	//Point_<double> center((double)dst.size().width / 2, (double)dst.size().height / 2);
	Point_<double> center(palm);
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
			if (img.at<uchar>(x, y) == 0) { // 원이 그려진 곳에 검은색 화소가 있다면
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


	/*

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



		//printf("t = %d   (x, y) = %d  \n", theta, dst.at<uchar>(x, y));
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

		waitKey(50);
	}

	cout << "count: " << count << endl;
	count = (count / 2) - 1;
	cout << "손가락 개수: " << count << endl;


	*/

}






