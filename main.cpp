
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
#include <queue>


using namespace cv;
using namespace std;

#define PI 3.14159265
#define qSize 10


// HandGestureDetection

// 블러된 영상을 프레임별로 계속 가져오기
Mat getImage(const Mat& img);

// 피부 검출 및 이진화
Mat skinDetection(const Mat& image, int minCr = 133, int maxCr = 173, int minCb = 77, int maxCb = 127);

// 손바닥 중심 검출
Point palmDetection(Mat img);

// 손가락 개수 세기
int countFinger(Mat img, Point palm);

// 손바닥으로 그림 그리기
void palmPaint(Mat& paper, Point palmCenter, int fingerCount);


int main(int argc, char** argv)
{
	Mat image;
	VideoCapture cap;
	Mat paper(Size(480, 640), CV_8UC3, Scalar(255, 255, 255));


	int fingerCount = -1;

	queue<int> countQ;

	for (int i = 0; i < qSize; i++)
	{
		countQ.push(-1);
	}

	// 웹캠
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


		// 블러된 영상을 프레임별로 계속 가져오기
		image = getImage(image);

		// 피부 검출 및 이진화
		image = skinDetection(image);
		namedWindow("binary Image");
		imshow("binary Image", image);

		// 손바닥 중심 검출
		Point palmCenter;
		palmCenter = palmDetection(image);


		// 손가락 개수 세기
		int tmpFingerCount;
		tmpFingerCount = countFinger(image, palmCenter);

		// 손가락을 6개 이상 찾으면 5개라고 한다.
		if (tmpFingerCount > 5) { tmpFingerCount = 5; }




		// ~~~ 큐에 있는 모든 수 중 최빈값으로 fingerCount 업데이트

		// 큐에 현재 값 넣고 예전 값 빼기
		countQ.push(tmpFingerCount);
		countQ.pop();

		queue<int> tmp(countQ);

		int mode[7] = { 0 }; // '손가락개수'의 개수를 센다
		while (!tmp.empty())
		{
			if (tmp.front() >= 0)  // 손 있는 경우
			{ 
				mode[tmp.front()]++; 
				
			}
			else { mode[6]++; } // 손 없는 경우
			tmp.pop();
		}


		// 손가락 개수가 몇 개인지 cmd창에 보여준다
		printf("%3d%3d%3d%3d%3d%3d%3d\n", 0, 1, 2, 3, 4, 5, -1);

		// 손가락 개수 최빈값 구하기
		int max = -1; 
		int idx = -1;// 제일 많은 '손가락개수'
		for (int i = 0; i < 7; i++)
		{
			printf("%3d", mode[i]);
			if (max <= mode[i])
			{
				max = mode[i];
				idx = i;
			}
		}


		fingerCount = idx;
		if (idx == 6) // 손이 없는 경우
		{
			fingerCount = -1;
		}
		printf("\nupdated fingerCount: %d\n\n", fingerCount);
		

		// 그림판
		resize(paper, paper, image.size());
		palmPaint(paper, palmCenter, fingerCount);

		if (waitKey(5) >= 0) break;
	}


	cout << "end" << endl;

	waitKey(1000);

	return 0;
}


// 블러된 영상을 프레임별로 계속 가져오기
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

	dilate(mask, mask, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
	erode(mask, mask, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 1);


	return mask;
}

// 손바닥 중심 검출
Point palmDetection(Mat img)
{

	Mat before;
	Mat after;

	before = img.clone();

	bool allBlack = true;

	// 계속 침식하다가 after의 화소가 전부 black이 되면 before에 남아있는 화소의 평균 위치 리턴
	while (allBlack)
	{
		erode(before, after, Mat());

		//imshow("before", before);
		//imshow("after", after);

		// after이 전부 블랙인지 검사
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

		//  after의 화소가 전부 black이 되면 before에 남아있는 화소의 평균 위치 리턴
		if (allBlack)
		{
			//imshow("return", before);
			// 가운데 점 계산

			int col = 0;
			int row = 0;
			int count = 0;

			// before에 있는 모든 화소의 평균 위치 계산
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

			if (count == 0) count++;
			row = (int)(row / count);
			col = (int)(col / count);

			Point palmCenter(col, row);

			// 찾은 손바닥 중심 시각화
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
int countFinger(Mat img, Point center) {

	// 1. 손바닥 중심부터 원을 그려나가며 내부 최대 원 찾기
	// 2. 손가락 개수 세기: 반지름이 내부 최대 원의 2배인 원 둘레를 따라가며 검정, 흰색이 바뀌는 횟수 찾기
	// 3. 텍스트로 보여주기

	// dst: 결과 영상
	// color: 원 그린 영상
	Mat dst, color;
	dst = img.clone();


	color = img.clone();
	cvtColor(color, color, COLOR_GRAY2BGR);
	cvtColor(dst, dst, COLOR_GRAY2BGR);
	//circle(color, center, 5, Scalar(123, 255, 123), -1);

	// 1. 손바닥 중심부터 원을 그려나가며 내부 최대 원 찾기
	// 원 둘레가 검은 픽셀을 만나게 되면 내부 최대 원임
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
				// 체크하고 있는 곳 표시. 파란색
				color.at<Vec3b>(x, y)[0] = 255;
				color.at<Vec3b>(x, y)[1] = 120;
				color.at<Vec3b>(x, y)[2] = 50;

				// 원이 그려진 곳에 검은색 화소가 있다면
				if (img.at<uchar>(x, y) == 0)
				{
					stop = true; // while 탈출~~
					break; // for 루프 탈출~~
				}
			}
		}
		radius += 1; // 원 반지름 증가
	}



	// 2. 손가락 개수 세기: 반지름이 내부 최대 원의 2배인 원 둘레를 따라가며 검정, 흰색이 바뀌는 횟수 찾기
	// 최종 원 그리기

	// 외부 원: 내부 원보다 반지름 크게
	//radius = radius * 1.5;
	radius = radius * 2;

	int count = 0;
	int pre_x, pre_y;

	pre_x = (int)(cos(0 * PI / 180) * radius + center.y);
	pre_y = (int)(sin(0 * PI / 180) * radius + center.x);


	// 외부원 둘레를 따라가며 검정, 흰색이 바뀌는 부분의 횟수 카운트
	for (int theta = 1; theta < 360; theta++)
	{
		x = (int)(cos(theta * PI / 180) * radius + center.y);
		y = (int)(sin(theta * PI / 180) * radius + center.x);

		// pre_, pre_y가 화면 밖을 벗어나면 다음 루프 실행
		if (!(0 < pre_x && pre_x < img.rows && 0 < pre_y && pre_y < img.cols))
		{
			pre_x = x;
			pre_y = y;
			continue;
		}


		// x, y가 화면 밖을 안 벗어난다면
		if (0 < x && x < img.rows && 0 < y && y < img.cols)
		{
			// 체크하고 있는 곳 표시. 호박색
			//color.at<Vec3b>(x, y)[0] = 50;
			//color.at<Vec3b>(x, y)[1] = 120;
			//color.at<Vec3b>(x, y)[2] = 255;

			// // 체크하고 있는 곳 표시. 잔디색
			color.at<Vec3b>(x, y)[0] = 50;
			color.at<Vec3b>(x, y)[1] = 255;
			color.at<Vec3b>(x, y)[2] = 120;


			// 이전 화소와 값이 다르다면 카운트
			// 검정-> 흰색으로 바뀌거나 흰색-> 검정으로 바뀌는 부분 카운트
			if (img.at<uchar>(pre_x, pre_y) != img.at<uchar>(x, y))
			{
				// 카운트한 부분 표시. 핫핑크색
				circle(color, Point(y, x), 3, Scalar(120, 0, 255), -1);

				count++;
			}

		}
		pre_x = x;
		pre_y = y;

		// 내부원, 외부원, 카운트한 부분 표시
		imshow("circles", color);
	}

	printf("count: %d\n", count);



	// 손목을 포함하므로 다음의 계산식을 거쳐야 손가락 개수임
	int fingerCount = (count / 2) - 1;


	// 3. 텍스트로 보여주기
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

	// 결과 보여주기
	imshow("Detected Fingers", dst);

	return fingerCount;
}


// 손바닥으로 그림 그리기
void palmPaint(Mat& original, Point palmCenter, int fingerCount)
{
	Mat paper = original.clone();

	// 색깔 설정
	Scalar red(0, 0, 255);
	Scalar green(0, 255, 0);
	Scalar blue(255, 0, 0);
	Scalar white(255, 255, 255);
	Scalar black(0, 0, 0);
	static Scalar color(0, 0, 255);

	// 안내 문구
	string text0 = "0: erase all";
	string text1 = "1: erase the pixel";
	string text2 = "2: change color : Red";
	string text3 = "3: change color : Green";
	string text4 = "4: change color : Blue";
	string text5 = "5: draw";


	// 텍스트 설정
	int fontFace = FONT_HERSHEY_PLAIN;
	double fontScale = 2;
	int thickness = 2;
	int baseline = 0;
	baseline += thickness;

	Size textSize = getTextSize(text0, fontFace, fontScale, thickness, &baseline);
	Point textOrg((paper.cols - textSize.width) / 2, (paper.rows + textSize.height) / 1.2);

	// 현재 손바닥 중심 표시
	circle(paper, palmCenter, 3, black, -1);

	// 손가락 개수에 따른 동작
	switch (fingerCount)
	{
	case 0: // 모두 지우기
		original = white;

		textSize = getTextSize(text0, fontFace, fontScale, thickness, &baseline);
		textOrg = Point((paper.cols - textSize.width) / 2, (paper.rows + textSize.height) / 1.2);
		putText(paper, text0, textOrg, fontFace, fontScale, Scalar(200, 200, 70), thickness, 8);
		break;
	case 1: // 그 픽셀 지우기
		circle(paper, palmCenter, 10, black, -1);
		circle(original, palmCenter, 10, white, -1);

		textSize = getTextSize(text1, fontFace, fontScale, thickness, &baseline);
		textOrg = Point((paper.cols - textSize.width) / 2, (paper.rows + textSize.height) / 1.2);
		putText(paper, text1, textOrg, fontFace, fontScale, Scalar(200, 200, 70), thickness, 8);
		break;
	case 2: // R
		color = red;

		textSize = getTextSize(text2, fontFace, fontScale, thickness, &baseline);
		textOrg = Point((paper.cols - textSize.width) / 2, (paper.rows + textSize.height) / 1.2);
		putText(paper, text2, textOrg, fontFace, fontScale, Scalar(200, 200, 70), thickness, 8);
		break;
	case 3: // G
		color = green;

		textSize = getTextSize(text3, fontFace, fontScale, thickness, &baseline);
		textOrg = Point((paper.cols - textSize.width) / 2, (paper.rows + textSize.height) / 1.2);
		putText(paper, text3, textOrg, fontFace, fontScale, Scalar(200, 200, 70), thickness, 8);
		break;
	case 4: // B
		color = blue;

		textSize = getTextSize(text4, fontFace, fontScale, thickness, &baseline);
		textOrg = Point((paper.cols - textSize.width) / 2, (paper.rows + textSize.height) / 1.2);
		putText(paper, text4, textOrg, fontFace, fontScale, Scalar(200, 200, 70), thickness, 8);
		break;
	case 5: // 그 픽셀에 그리기
		circle(original, palmCenter, 3, color, -1);

		textSize = getTextSize(text5, fontFace, fontScale, thickness, &baseline);
		textOrg = Point((paper.cols - textSize.width) / 2, (paper.rows + textSize.height) / 1.2);
		putText(paper, text5, textOrg, fontFace, fontScale, Scalar(200, 200, 70), thickness, 8);
		break;
	default: // 안내문구

		// 기능 설명
		Size textSize0 = getTextSize(text0, fontFace, fontScale, thickness, &baseline);
		Size textSize1 = getTextSize(text1, fontFace, fontScale, thickness, &baseline);
		Size textSize2 = getTextSize(text2, fontFace, fontScale, thickness, &baseline);
		Size textSize3 = getTextSize(text3, fontFace, fontScale, thickness, &baseline);
		Size textSize4 = getTextSize(text4, fontFace, fontScale, thickness, &baseline);
		Size textSize5 = getTextSize(text5, fontFace, fontScale, thickness, &baseline);

		Point textOrg0((paper.cols - textSize2.width) / 2, (paper.rows + textSize0.height) / 1.2 - textSize0.height * 2 * 5);
		Point textOrg1((paper.cols - textSize2.width) / 2, (paper.rows + textSize1.height) / 1.2 - textSize1.height * 2 * 4);
		Point textOrg2((paper.cols - textSize2.width) / 2, (paper.rows + textSize2.height) / 1.2 - textSize2.height * 2 * 3);
		Point textOrg3((paper.cols - textSize2.width) / 2, (paper.rows + textSize3.height) / 1.2 - textSize3.height * 2 * 2);
		Point textOrg4((paper.cols - textSize2.width) / 2, (paper.rows + textSize4.height) / 1.2 - textSize4.height * 2 * 1);
		Point textOrg5((paper.cols - textSize2.width) / 2, (paper.rows + textSize5.height) / 1.2);

		putText(paper, text0, textOrg0, fontFace, fontScale, Scalar(200, 200, 70), thickness, 8);
		putText(paper, text1, textOrg1, fontFace, fontScale, Scalar(200, 200, 70), thickness, 8);
		putText(paper, text2, textOrg2, fontFace, fontScale, Scalar(200, 200, 70), thickness, 8);
		putText(paper, text3, textOrg3, fontFace, fontScale, Scalar(200, 200, 70), thickness, 8);
		putText(paper, text4, textOrg4, fontFace, fontScale, Scalar(200, 200, 70), thickness, 8);
		putText(paper, text5, textOrg5, fontFace, fontScale, Scalar(200, 200, 70), thickness, 8);


		// 크레딧
		string developer = "Developer : YEJIN-LILY, KimRiun, LeeJE20";
		fontScale = 1.5;
		thickness = 2;
		Size textSize_developer = getTextSize(developer, fontFace, fontScale, thickness, &baseline);
		Point textOrg_developer((paper.cols - textSize_developer.width) / 2, (paper.rows + textSize_developer.height) / 4);
		putText(paper, developer, textOrg_developer, fontFace, fontScale, Scalar(200, 70, 200), thickness, 8);

		fontScale = 3;
		thickness = 3;
		string title = "Source";
		Size textSize_title = getTextSize(title, fontFace, fontScale, thickness, &baseline);
		Point textOrg_title((paper.cols - textSize_title.width) / 2, (paper.rows + textSize_title.height) / 8);
		putText(paper, title, textOrg_title, fontFace, fontScale, Scalar(255, 180, 120), thickness, 8);


		break;
	}
	imshow("Paper", paper);
}
