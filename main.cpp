#include <cstdio>
#include "opencv2/highgui.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"

#define FOCAL_LENGTH 0	       // Focal length of both camereas
#define CAMERA_DIFFERENCE 0  // Distance between cameras

using namespace cv;

float findTarget(Mat &frame1, &frame2);
double calculateDistance(float d);

int main() {
	VideoCapture cap1(0);
	VideoCapture cap2(1);
	Mat frame1, frame2;

	if (!cap1.isOpened() || !cap2.isOpened()) return -1;

	while (cap1.read(frame1) && cap2.read(frame2)) {
		printf("%f\n", calculateDistance(findTarget(frame1, frame2)));

		// Esc to end
		if (char(waitKey(10)) == 27) {
			break;
		}
	}

	return 0;
}

float findTarget(Mat &frame1, Mat &frame2) {

}

double calculateDistance(float d) {
	return FOCAL_LENGTH * CAMERA_DIFFERENCE / d
}
