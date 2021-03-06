/*
 * Title:    main.cpp
 * Author:    Dilpreet S. Chana
 * Description:    Dual camera range finder
**/

#include <algorithm>
#include <cstdio>
#include <cmath>
#include <vector>
#include <initializer_list>
#include <iostream>
#include <opencv2/opencv.hpp>

// Camera properties (Microsoft HD 3000)
#define FOCAL_LENGTH 60
#define FLANGE_FOCAL_DISTANCE 14.0169
#define CAMERA_DIFFERENCE 95
#define LIGHT_SENSOR_WIDTH 17.3
#define LIGHT_SENSOR_HEIGHT 13

#define TILE_SIZE 2
#define MAX_DIST 40000
#define THRESHOLD 100

using namespace cv;
using namespace std;

// Global constants
CascadeClassifier face_cascade;

double calculateDistance(Point p1, Point p2, VideoCapture c);

int main() {
    VideoCapture cap1("../Data/desk_frame_left.jpg");
    VideoCapture cap2("../Data/desk_frame_right.jpg");
    //VideoCapture cap1(0);
    //VideoCapture cap2(1);
    Mat frame1, frame2, depth_map;
    Mat grey1, grey2, corner_map;
    Mat edge1, edge2;

    if (!cap1.isOpened()) {
        printf("Cap1 fucked\n");
        return -1;
    }

    if (!cap2.isOpened()) {
        printf("Cap2 fucked\n");
        return -1;
    }

    while (cap1.read(frame1) && cap2.read(frame2)) {
        // Corner detection
        cvtColor(frame1, grey1, COLOR_RGB2GRAY);
        cvtColor(frame2, grey2, COLOR_RGB2GRAY);

        std::vector<Point2f> corners;
        // TODO: Make this bitch work
        corner_map = Mat(frame1.rows, frame1.cols, CV_8UC1, Scalar(0));

        goodFeaturesToTrack(grey1, corner_map, 3000, 0.01, 7);
        goodFeaturesToTrack(grey2, corner_map, 3000, 0.01, 7);

        // TODO: Map corners points in a matrix

        for (auto c : corners) {
            circle(grey1, c, 2, Scalar(0, 255, 0), -1);
            circle(grey2, c, 2, Scalar(0, 255, 0), -1);
        }

        // Edge detection
        Canny(frame1, edge1, 50, 100);
        Canny(frame2, edge2, 50, 100);

        depth_map = Mat(frame1.rows, frame1.cols, CV_8UC2, Scalar(0, 0));

        for (int y = 0; y < edge2.rows - TILE_SIZE; y += TILE_SIZE) {
            for (int x = 0; x < edge2.cols - TILE_SIZE; x += TILE_SIZE) {
                Mat p1(edge2, Rect(x, y, TILE_SIZE, TILE_SIZE));

                for (int tx = x; tx < edge1.cols - TILE_SIZE; tx += TILE_SIZE) {
                    Mat p2(edge1, Rect(tx, y, TILE_SIZE, TILE_SIZE));

                    // Compare p1 and p2
                    Mat diff = p1 - p2;
                    int comp = 0;

                    for (int i = 0; i < diff.rows; i++) {
                        for (int j = 0; j < diff.cols; j++) {
                            comp += diff.at<uchar>(i, j) * diff.at<uchar>(i, j);
                        }
                    }

                    comp = sqrt(comp);

                    if (comp < THRESHOLD) {
                        double dist = calculateDistance(Point(x, y), Point(tx, y), cap1);
                        rectangle(depth_map, Rect(x, y, TILE_SIZE, TILE_SIZE), Scalar(dist / MAX_DIST * 255), FILLED);

                        break;
                    }
                }
            }
        }

        imshow("Cam1", frame1);
        imshow("Cam2", frame2);
        imshow("Edge1", edge1);
        imshow("Edge2", edge2);
        imshow("Grey1", grey1);
        imshow("Grey2", grey2);
        imshow("Corner map", corner_map);
        imshow("Depth Map", depth_map);

        // Esc to end
        while (char(waitKey(10)) != 27) {}
    }

    return 0;
}

double calculateDistance(Point p1, Point p2, VideoCapture c) {
    if ((p1.x == 0 && p1.y == 0) || (p2.x == 0 && p2.y == 0)) return 0;

    // Frames need to swap
    if (p1.x > p2.x) {
        Point tmp = p1;
        p1 = p2;
        p2 = tmp;
    }

    // Calculate deviation on camera sensor
    // Assumption: p1 contains data from left camera
    //             p2 contains data from right camera
    double s1 = (LIGHT_SENSOR_WIDTH / 2) * (p1.x - c.get(CAP_PROP_FRAME_WIDTH) / 2) / (c.get(CAP_PROP_FRAME_WIDTH) / 2);  // -DF
    double s2 = (LIGHT_SENSOR_WIDTH / 2) * (p2.x - c.get(CAP_PROP_FRAME_WIDTH) / 2) / (c.get(CAP_PROP_FRAME_WIDTH) / 2);  // EG

    return FLANGE_FOCAL_DISTANCE * CAMERA_DIFFERENCE / (s2 - s1);
}
