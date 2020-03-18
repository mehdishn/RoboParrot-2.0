#ifndef C_U_H_
#define C_U_H_

#include <opencv2/core/core.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
 using namespace std;
 using namespace cv;

//const int sub_frame_scale = 40; //color_detection_ROI -->for example 50 percent
//#define show_sub_frame
//#define show_rgb

class bad_intensity_directive_exception{};


void print_color_at_coordinates(Mat& img, string text, int x, int y);

string get_triangle_color(Mat& frame, vector<Point> triangle);
string get_square_color(Mat& frame, vector<Point> square);
string get_circle_color(Mat& frame, Vec3f circle);
string get_shape_color (Mat&, vector<CvPoint>&);

bool check_darkness(Mat frame);

string get_the_color (Mat&);
    namespace color_utility
    {
        void print_color(Mat& img, string);
    }

#endif
