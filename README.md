# HandGestureDetection

## Specification
* OpenCV 4.4.0
* Microsoft Visual C++ 2019

## Structure
1. 영상을 프레임별로 계속 가져오기(Mat getImage(Mat img))
   - 웹캠
   - 미디안 필터(잡음 제거)
2. 피부 검출(Mat skinDetection(Mat img)
   - RGB -> HSV로 컬러 공간 변환
   - inRange()로 피부영역(두 값 사이) 검출 
   - 피부는 흰색, 배경은 검정으로 이진화
   - 열림 연산(잡음제거)
3. 손바닥 검출(ROI) Point palmDetection(Mat img)
   - *침식으로 손바닥만 남기기
   - 손바닥 중심 알아내기
4. 손 개수 세기 void countFinger(Mat img, Point palm)
   - 원 그리기
   - 개수 출력하기(cmd창)


## Plan
1, 2, ppt: 예진
3, 시연: 정은
4, 메인 합치기: 경륜

일요일 밤 10시까지: 코딩 1차 검사<br>
일요일 밤 12: 00까지 코딩 마감 (합치기까지)<br>
시연 영상: 코딩 마감 후 (정은)<br>
월요일 밤까지 ppt
