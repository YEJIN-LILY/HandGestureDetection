
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

// 손가락 개수 세기
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




		Point palmCenter;
		palmCenter = palmDetection(image);

		//namedWindow("Palm Image");
		//imshow("Palm Image", image);

		countFinger(image, palmCenter);
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

			Point palmCenter(col, row);

			//after.at<uchar>(row, col) = 255;
			cvtColor(img, img, COLOR_GRAY2BGR);

			circle(img, palmCenter, 5, Scalar(123, 255, 123), -1);

			namedWindow("Palm Image");
			imshow("Palm Image", img);



			return palmCenter;
		}

		allBlack = true;
		before = after.clone();
	}


}


// 손가락 개수 세기
void countFinger(Mat img, Point center) {

	Mat dst, color;
	//img.copyTo(dst);
	dst = img.clone();


	color = img.clone();
	cvtColor(color, color, COLOR_GRAY2BGR);
	cvtColor(dst, dst, COLOR_GRAY2BGR);
	//circle(color, center, 5, Scalar(123, 255, 123), -1);


	double radius = 15.0;
	int x, y;
	bool stop = false;
	while (!stop)
	{
		for (int theta = 0; theta < 360; theta++) {


			x = (int)(cos(theta * PI / 180) * radius + center.y);
			y = (int)(sin(theta * PI / 180) * radius + center.x);
			if (0 < x && x < img.rows && 0 < y && y < img.cols)
			{
				// 체크하고 있는 곳 표시
				color.at<Vec3b>(x, y)[0] = 255;
				color.at<Vec3b>(x, y)[1] = 120;
				color.at<Vec3b>(x, y)[2] = 50;

				// 원이 그려진 곳에 검은색 화소가 있다면
				if (img.at<uchar>(x, y) == 0)
				{ 
					stop = true; // while 탈출~~
					break; // for 루프 탈출~~
				}

				//namedWindow("color");
				//imshow("color", color);
				//waitKey(10);
			}
		}
		radius += 1; // 원 반지름 증가
	}

	//namedWindow("inner circle");
	//imshow("inner circle", color);


	//Mat image = imread("hand.jpg");

	//Point center = palmDetectuon(img); // input gray-scale image
	//Point_<double> center((double)dst.size().width / 2, (double)dst.size().height / 2);
	//Point_<double> center(palm);
	//cout << "손바닥 중심점 좌표:" << center << endl;


	// 최종 원 그리기
	//radius = radius * 1.5;
	radius = radius * 2;

	int count = 0;
	int pre_x, pre_y;

	pre_x = (int)(cos(0 * PI / 180) * radius + center.y);
	pre_y = (int)(sin(0 * PI / 180) * radius + center.x);


	//printf("row, column = %d, %d\n", img.rows, img.cols);
	for (int theta = 1; theta < 360; theta++) 
	{
		x = (int)(cos(theta * PI / 180) * radius + center.y);
		y = (int)(sin(theta * PI / 180) * radius + center.x);


		if ( !(0 < pre_x && pre_x < img.rows && 0 < pre_y && pre_y < img.cols) )
		{
			pre_x = x;
			pre_y = y;
			continue;
		}



		if (0 < x && x < img.rows && 0 < y && y < img.cols)
		{
			// 체크하고 있는 곳 표시. 호박색
			color.at<Vec3b>(x, y)[0] = 50;
			color.at<Vec3b>(x, y)[1] = 120;
			color.at<Vec3b>(x, y)[2] = 255;

			//printf("before, now = (%3d, %3d), (%3d, %3d)\n", pre_x, pre_y, x, y);
			//printf("befroe, now = %3d, %3d\n\n", img.at<uchar>(pre_x, pre_y), img.at<uchar>(x, y));

			// 이전 화소와 값이 다르다면 카운트
			if ( img.at<uchar>(pre_x, pre_y) != img.at<uchar>(x, y)) 
			{ 
				color.at<Vec3b>(x, y)[0] = 50;
				color.at<Vec3b>(x, y)[1] = 255;
				color.at<Vec3b>(x, y)[2] = 120;
				count++;
			}

		}
		pre_x = x;
		pre_y = y;


		imshow("circles", color);
		//waitKey(1);
	}

	printf("count: %d\n", count);


	int fingerCount = (count / 2) - 1;

	printf("fingerCount: %d\n\n", fingerCount);


	string text = "fingerCount = ";
	
	if (fingerCount < 0)
	{
		text = "No hand";
	}
	else
	{
		text += to_string(fingerCount);
	}

	int fontFace = FONT_HERSHEY_PLAIN;
	double fontScale = 2;
	int thickness = 3;

	int baseline = 0;
	Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);
	baseline += thickness;
	// center the text
	Point textOrg((img.cols - textSize.width) / 2, (img.rows + textSize.height) / 1.2);


	// then put the text itself
	putText(dst, text, textOrg, fontFace, fontScale, Scalar(200, 200, 70), thickness, 8);



	imshow("Detected Fingers", dst);



}





