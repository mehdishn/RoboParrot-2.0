#ifndef S_U_H_
#define S_U_H_

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "color_utility.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;


    const int thresh = 30;



    vector<Vec3f> detect_circles(const Mat& frame);
    vector<vector<Point> > findTriangles(const Mat& image);
    vector<vector<Point> > findSquares(const Mat& image);

    void draw_circles(Mat& frame, vector<Vec3f> circles);
    void draw_objects(Mat& image, const vector<vector<Point> >& objects); // other obj other than circle

    namespace shape_utility
    {
        void print_shapes (Mat& img, const vector<vector<Point> >& squares, const vector<vector<Point> >& triangles, const vector<Vec3f>& circles);
    }
#endif
