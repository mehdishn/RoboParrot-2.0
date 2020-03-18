#include "shape_utility.h"

void find_objects (const Mat& image, vector<vector<Point> >& objects, int corners) // can detect both squares and triangle based on the given directive
{
    objects.clear();
    vector<vector<Point> > contours;
    vector<Point> approx;
    Mat src_gray;
    cvtColor(image, src_gray, CV_BGR2GRAY);
    GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2);
    Canny(src_gray, src_gray, 150, 60);
    //imshow("blur", src_gray);
    //waitKey(1);
    findContours(src_gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    // test each contour
    for (size_t i = 0; i < contours.size(); i++)
    {
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

        if (approx.size() == corners &&
            fabs(contourArea(Mat(approx))) > 1000 &&
            isContourConvex(Mat(approx)))
        {
            objects.push_back(approx);
        }
    }
}

vector<Vec3f> detect_circles (const Mat& frame) // returns number of circles detected
{
  Mat src_gray;
  cvtColor(frame, src_gray, CV_BGR2GRAY);
  // Reduce the noise so we avoid false circle detection
  GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2);
  vector<Vec3f> circles;
  // Apply the Hough Transform to find the circles
  HoughCircles( src_gray, circles, CV_HOUGH_GRADIENT, 1, src_gray.rows/8, 150, 80, 0, 0 );
  return circles;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vector<vector<Point> > findTriangles (const Mat& image)
{
    vector<vector<Point> > triangles;
    find_objects(image, triangles, 3);

    return triangles;
}

vector<vector<Point> > findSquares (const Mat& image)
{
    vector<vector<Point> > squares;
    find_objects(image, squares, 4);

    return squares;
}

void shape_utility::print_shapes (Mat& img, const vector<vector<Point> >& squares, const vector<vector<Point> >& triangles, const vector<Vec3f>& circles)
{
    if (squares.size() > 0)
       putText(img, "Square", cvPoint(0, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(100, 60, 50), 1, CV_AA);
    if (triangles.size() > 0)
        putText(img, "Triangle", cvPoint(0, 65), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(100, 60, 50), 1, CV_AA);
    if (circles.size() > 0)
        putText(img, "Circle", cvPoint(0, 100), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(100, 60, 50), 1, CV_AA);
}

void draw_circles (Mat& frame, vector<Vec3f> circles)
{
  // Draw the circles detected
  for( size_t i = 0; i < circles.size(); i++ )
  {
      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      // circle center
      circle( frame, center, 3, Scalar(0,255,0), -1, 8, 0 );
      // circle outline
      circle( frame, center, radius, Scalar(0,0,255), 3, 8, 0 );

      string the_color = get_circle_color(frame, circles[i]);
      print_color_at_coordinates(frame, the_color, circles[i][0], circles[i][1]);
   }
}

void draw_objects (Mat& image, const vector<vector<Point> >& objects ) // other obj other than circle
{
    for( size_t i = 0; i < objects.size(); i++ )
    {
        if(i%4 == 0 && objects[i].size() == 4)
        {
            string the_color = get_square_color(image, objects[i]);
            print_color_at_coordinates(image, the_color, objects[i][0].x, objects[i][0].y);
        }
        else if(i%4 == 0 && objects[i].size() == 3)
        {
            string the_color = get_triangle_color(image, objects[i]);
            print_color_at_coordinates(image, the_color, objects[i][0].x - 7, objects[i][0].y - 7);
        }
        const Point* p = &objects[i][0];
        int n = (int)objects[i].size();
        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, CV_AA);
    }
}
