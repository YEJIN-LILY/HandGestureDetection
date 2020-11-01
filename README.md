# Source
HandGestureDetection Project

<br>
Last Update: 2020.11.01 19:00 <br>

---

## Specification
* OpenCV 4.4.0
* Microsoft Visual C++ 2019

## Structure
1. __영상을 프레임별로 계속 가져오기 <br>__
   Mat getImage(const Mat& img)
   - 웹캠
   - 좌우 반전
   - 미디안 필터(잡음 제거)

2. 피부 검출 및 이진화 <br>
   Mat skinDetection(const Mat& image, int minCr = 133, int maxCr = 173, int minCb = 77, int maxCb = 127)
   - RGB -> YCbCr로 컬러 공간 변환
   - 피부영역(두 값 사이) 검출 
   - 피부는 흰색, 배경은 검정으로 이진화
   - 열림, 닫힘 연산(잡음제거)

3. 손바닥 중심 검출 <br>
   Point palmDetection(Mat img)
   - 침식으로 손바닥 중심 알아내기

4. 손가락 개수 세기 <br>
   void countFinger(Mat img, Point palm)
   - 손바닥 중심부터 원을 그려나가며 내부 최대 원 찾기
   - 손가락 개수 세기: 반지름이 내부 최대 원의 2배인 원 둘레를 따라가며 검정, 흰색이 바뀌는 횟수 찾기
   - 텍스트로 보여주기

5. 손가락 개수 안정적으로 세기
   - 최근 20회 중 최빈값으로 fingerCount 업데이트
   - queue 사용
   
6. 손바닥으로 그림 그리기 <br>
   void palmPaint(Mat& paper, Point palmCenter, int fingerCount)
   - 현재 위치 검은 점으로 표시
   - 디폴트 색은 Red
   - 0: 모두 지우기
   - 1: 해당 픽셀 지우기
      + 지우개 반지름 10
   - 2: 색깔 Red로 바꾸기
   - 3: 색깔 Green으로 바꾸기
   - 4: 색깔 Blue로 바꾸기
   - 5: 그리기
      + 반지름 3
   - 그 외: 기능 설명 및 크레딧 보여주기

## Plan
1, 2, ppt: YEJIN-LILY<br>
3, 5, 6, 메인 합치기: LeeJE20<br>
4, 시연 영상: KimRiun<br>

일요일 밤 10시까지: 코딩 1차 검사<br>
일요일 밤 12: 00까지 코딩 마감 (합치기까지)<br>
월요일 밤까지 ppt

## Result
Click below image to watch video! <br>
[![Go to see Source Program](http://img.youtube.com/vi/-6FC3nyXvvU/0.jpg)](https://youtu.be/-6FC3nyXvvU) 

