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
// 

// 영상을 프레임별로 계속 가져오기
Mat getImage(Mat img);

// 피부 검출 및 이진화
Mat skinDetection(Mat img);

// 손바닥 검출
Mat palmDetection(Mat img);

// 손가락 개수 세기
Mat countFinger(Mat img);





int main(int argc, char** argv)
{
	Mat image;

	image = imread("binaryGroup.bmp", 0); // Read the file
	if (!image.data)
	{
		cout << "Could not open or find the image" << endl;
		return -1;
	}

	namedWindow("Original Image");
	imshow("Original Image", image);


	//image = getImage(image);
	//namedWindow("webcam Image");
	//imshow("webcam Image", image);

	//image = skinDetection(image);
	//namedWindow("binary Image");
	//imshow("binary Image", image);

	image = palmDetection(image);
	namedWindow("Palm Image");
	imshow("Palm Image", image);

	//image = countFinger(image);
	//namedWindow("Finger Image");
	//imshow("Finger Image", image);



	waitKey(0);

	return 0;
}


// 손바닥 검출
Mat palmDetection(Mat img)
{
	Mat before;
	Mat after;

	bool allBlack = true;

	before = img.clone();



	
	while (allBlack)
	{

		erode(before, after, Mat());
		
		imshow("before", before);
		imshow("after", after);

		for (int j = 0; j < img.rows; j++)
		{
			//j열의 주소(nc개만큼) 가져오기
			uchar* data = after.ptr<uchar>(j);
			for (int i = 0; i < img.cols; i++)
			{
				if (data[i]) allBlack = false;
			}

		}
		


		waitKey(100);

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

			after.at<uchar>(row, col) = 255;
			return after;
		}

		allBlack = true;
		before = after.clone();
	}


}
