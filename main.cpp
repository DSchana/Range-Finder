/*
 * Title:	main.cpp
 * Author:	Dilpreet S. Chana
 * Description:	Dual camera range finder
**/

#include <cstdio>
#include <cmath>
#include <vector>

#include "opencv2/highgui.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"

// Camera properties (Microsoft HD 3000)
#define FOCAL_LENGTH 60
#define FLANGE_FOCAL_DISTANCE 14.0169
#define CAMERA_DIFFERENCE 95
#define LIGHT_SENSOR_WIDTH 17.3
#define LIGHT_SENSOR_HEIGHT 13

#define FACE_CASCADE "data/haarcascade_frontalface_alt.xml"

using namespace cv;
using namespace std;

// Global constants
CascadeClassifier face_cascade;

double calculateDeviation(Mat& frame1, Mat& frame2, VideoCapture c);
Point findPattern(Mat& frame);
double calculateDistance(float d);

int main() {
	VideoCapture cap1(0);
	VideoCapture cap2(1);
	Mat frame1, frame2;

	if (!cap1.isOpened()) {
		printf("Cap1 fucked\n");
		return -1;
	}

	if (!cap2.isOpened()) {
		printf("Cap2 fucked\n");
		return -1;
	}

	if (!face_cascade.load(FACE_CASCADE)) {
		printf("Cascade fucked\n");
		return -1;
	}

	while (cap1.read(frame1) && cap2.read(frame2)) {
		printf("%f mm\n", calculateDistance(calculateDeviation(frame1, frame2, cap1)));

		imshow("Cam1", frame1);
		imshow("Cam2", frame2);

		// Esc to end
		if (char(waitKey(10)) == 27) {
			break;
		}
	}

	return 0;
}

double calculateDeviation(Mat& frame1, Mat& frame2, VideoCapture c) {
	Point p1 = findPattern(frame1);
	Point p2 = findPattern(frame2);

	if ((p1.x == 0 && p1.y == 0) || (p2.x == 0 && p2.y == 0)) return 0;

	// Frames need to swap
	if (p1.x > p2.x) {
		Point tmp = p1;
		p1 = p2;
		p2 = tmp;
	}

	// Calculate deviation on camera sensor
	// Assumption: p1 contains data from left camera
	//	       p2 contains data from right camera
	double s1 = (LIGHT_SENSOR_WIDTH / 2) * (p1.x - c.get(CV_CAP_PROP_FRAME_WIDTH) / 2) / (c.get(CV_CAP_PROP_FRAME_WIDTH) / 2);  // -DF
	double s2 = (LIGHT_SENSOR_WIDTH / 2) * (p2.x - c.get(CV_CAP_PROP_FRAME_WIDTH) / 2) / (c.get(CV_CAP_PROP_FRAME_WIDTH) / 2);  // EG

	return s2 - s1;
}

Point findPattern(Mat& frame) {
	// Perhaps find faces for round 1
	vector<Rect> faces;
	Mat frame_gray, frame_gray_blur;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	GaussianBlur(frame_gray, frame_gray_blur, Size(3, 3), 0, 0);

	Rect main_face = Rect(0, 0, 0, 0);

	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(80, 80));

	for (size_t i = 0; i < faces.size(); i++) {
		if (faces[i].width * faces[i].height > main_face.width * main_face.height) {
			main_face.x = faces[i].x;
			main_face.y = faces[i].y;
			main_face.width = faces[i].width;
			main_face.height = faces[i].height;
		}
	}

	rectangle(frame, main_face, Scalar(155, 155, 155), 5);

	return Point(main_face.x + main_face.width / 2, main_face.y + main_face.height / 2);
}

double calculateDistance(float d) {
	return FLANGE_FOCAL_DISTANCE * CAMERA_DIFFERENCE / d;
}
