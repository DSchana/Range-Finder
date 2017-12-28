#include "opencv2/highgui.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;

int main() {
	VideoCapture cap1(0);
	VideoCapture cap2(1);
	Mat frame1, frame2;

	if (!cap1.isOpened() || !cap2.isOpened()) return -1;

	while (cap1.read(frame1) && cap2.read(frame2)) {
		// Esc to end
		if (char(waitKey(10)) == 27) {
			break;
		}
	}

	return 0;
}
