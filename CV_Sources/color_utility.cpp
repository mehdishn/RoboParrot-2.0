#include "color_utility.h"

vector <int> get_pixel_intensity (const Mat& img, const int col, const int row, string _directive = "single_channel") //other _dir = "rgb"
{
    if (_directive == "single_channel")
    {
        vector <int> color_value;
        Scalar intensity = img.at<uchar>(row, col);
        color_value.push_back(intensity.val[0]);
        return color_value;
    }

    Vec3b intensity = img.at<Vec3b>(row, col);
    uchar blue = intensity.val[0];
    uchar green = intensity.val[1];
    uchar red = intensity.val[2];

    if(_directive == "rgb")
    {
        vector <int> rgb;
        rgb.push_back(red);
        rgb.push_back(green);
        rgb.push_back(blue);
        return rgb;
    }

    throw  bad_intensity_directive_exception();
}

bool is_in_range (const int x, const int up_threshold, const int down_threshold)
{
    return (x > down_threshold) && (x < up_threshold);
}

void get_pixel_rgb_values (const Mat& img, const int col, const int row, int& red, int& green, int& blue)
{
    vector <int> rgb;
    rgb = get_pixel_intensity(img, col, row, "rgb");
    red = rgb[0], green = rgb[1], blue = rgb[2];
}

int calculate_rgb_mean(int r, int g, int b)
{
    return (r + g + b) / 3;
}

int calculate_rgb_mean(int a, int b)
{
    return (a + b) / 2;
}

bool is_white(const int red, const int green, const int blue)
{
    return (red > 155) && (green > 155) && (blue > 155) && (abs(red - green) < 17) && (abs(red - blue) < 17) && (abs(green - blue) < 17);
}

bool is_black(const int red, const int green, const int blue)
{
    if ((red < 60) && (green < 60) && (blue < 60))
        return true;
    else if (calculate_rgb_mean(red, green, blue) < 70 && (abs(red - green) < 10) && (abs(red - blue) < 10) && (abs(green - blue) < 10))
        return true;
    return false;
}

bool is_red (const int red, const int green, const int blue) // determines whether a pixel is red
{
    return (red > (calculate_rgb_mean(green, blue) + 45));
}

bool is_yellow(const int red, const int green, const int blue)
{
    if (abs(red - green) > 60 ) // dont name reds, yellow!
        return false;
    else
        return (red > 120) && (green > 90) && (blue < red - 40) && (calculate_rgb_mean(red, green) > 110);
}

bool is_orange(const int red, const int green, const int blue)
{
    if ((abs(red - green) < 100)) // it is not orange
        return false;
    //if(red > calculate_rgb_mean(green, blue) + 100) // it is red
        //return false;
    if(red > (calculate_rgb_mean(green, blue) + 90) &&  green < blue) // it is not orange so it is red
        return false;
    else if ((red > 125) && (green > 70) && (blue < 130) && (blue < (red - 45)) && green > (blue + 10))
        return true;
    else if (green > (blue + 30) && red > (blue + 45))
        return true;

    return false;
}

bool is_brown(const int red, const int green, const int blue)
{
    return (red < 160) && (red > 70) && (green > 40) && (green < 105)  && (blue < 80) && (blue < (red - 24)) && (calculate_rgb_mean(red, green, blue) < 107);
}

bool is_purple(const int red, const int green, const int blue)
{
    if ((red > 150) && (red > (calculate_rgb_mean(green, blue) + 50))) // it is red
        return false;
    return (red > 100) && (green > 40) && (blue > 90) && (calculate_rgb_mean(red, green, blue) < 150);
}

bool is_pink(const int red, const int green, const int blue)
{
    if ((calculate_rgb_mean(red, green, blue) < 145) && (calculate_rgb_mean(red, green) < 130)) // its not pink
        return false;
    else if (red > (calculate_rgb_mean(green, blue) + 90) || red > green + 50) // its red
        return false;
    else if (blue > calculate_rgb_mean(green, red)) // its blue
        return false;
    else
        return (red > 140) && (green > 85) && (blue > 100);
}

bool is_green (const int red, const int green, const int blue)
{
    if ((red < 48) && (green > 65) && (blue < green + 35))
        return true;
    if (green > (red + 40) && green > (blue - 10))
        return true;
    return false;
}

bool is_blue (const int red, const int green, const int blue)
{
    if ((calculate_rgb_mean(red, green) > 100) && (blue > (calculate_rgb_mean(green, red) + 50)))
        return true;
    else return (blue > (calculate_rgb_mean(green, red) + 35));
}

Vec3i get_mean_rgb(const Mat& img)
{
    Vec3i counter_3d (0);

    for(int col = 0; col < img.cols; col += 3)
        for(int row = 0; row < img.rows; row += 3)
        {
            int r, g, b;
            get_pixel_rgb_values (img, col, row, r, g, b);
            counter_3d[0] += r;
            counter_3d[1] += g;
            counter_3d[2] += b;
        }

    counter_3d[0] = counter_3d[0] / ((img.cols / 3) * (img.rows / 3));
    counter_3d[1] = counter_3d[1] / ((img.cols / 3) * (img.rows / 3));
    counter_3d[2] = counter_3d[2] / ((img.cols / 3) * (img.rows / 3));
    return counter_3d;
}

string detect_rgb_color(const int r, const int g, const int b)
{
    if(is_green(r, g, b))
        return "green";
    else if(is_black(r, g, b))
        return "black";
    else if(is_white(r, g, b))
        return "white";
    //else if(is_brown(r, g, b))
        //return "brown";
    else if (is_orange(r, g, b))
        return "orange";
    else if(is_pink(r, g, b))
        return "pink";
    else if(is_purple(r, g, b))
        return "purple";
    else if (is_yellow(r, g, b))
        return "yellow";
    else if(is_blue(r, g, b))
        return "blue";
    else if(is_red(r, g, b))
        return "red";
    else return "nothing!";
}

void print_rgb_for_debug(Mat& img, int r, int g, int b)
{
    putText(img, to_string(r).c_str(), cvPoint(100, 150), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 20, 150), 1, CV_AA);
    putText(img, to_string(g).c_str(), cvPoint(100, 170), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 20, 150), 1, CV_AA);
    putText(img, to_string(b).c_str(), cvPoint(100, 190), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 20, 150), 1, CV_AA);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void color_utility::print_color(Mat& img, string text)
{
    putText(img, text.c_str() , cvPoint(100,100), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,20,150), 1, CV_AA);
    //rectangle(img, Point(img.cols / 2 - frame_length, img.rows / 2 - frame_length), Point(img.cols / 2 + frame_length, img.rows / 2 + frame_length), Scalar(255, 0, 255));
}

void print_color_at_coordinates(Mat& img, string text, int x, int y)
{
    putText(img, text.c_str() , cvPoint(x, y), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 0), 1, CV_AA);
    //rectangle(img, Point(img.cols / 2 - frame_length, img.rows / 2 - frame_length), Point(img.cols / 2 + frame_length, img.rows / 2 + frame_length), Scalar(255, 0, 255));
}

string get_the_color(Mat& frame)
{
    // create subframe
    Mat sub_frame;
    sub_frame = frame(Rect(frame.cols / 2 - frame.cols / 4 , frame.rows / 2 - frame.rows / 4, frame.cols / 2, frame.rows / 2));
    imshow("!!", sub_frame);

    // get mean rgb
    Vec3i mean_rgb = get_mean_rgb(sub_frame);
    //the only non-const part:
    print_rgb_for_debug(frame, mean_rgb[0], mean_rgb[1], mean_rgb[2]);

    // detetct color using a LUT
    string detected_color = detect_rgb_color(mean_rgb[0], mean_rgb[1], mean_rgb[2]);

    return detected_color;
}

string get_circle_color(Mat& frame, Vec3f circle)
{
    int red  = 0, green = 0, blue = 0, r, g, b;
    int center_x = circle[0], center_y = circle[1], radius = circle[2];

    //get_pixel_rgb_values(img, col, row, r, g, b);

    for(int i = center_x ; i < center_x + radius; i++)
    {
        get_pixel_rgb_values(frame, i, center_y, r, g, b);
        red += r;
        green += g;
        blue += b;
    }

    for(int i = center_x ; i > center_x - radius; i--)
    {
        get_pixel_rgb_values(frame, i, center_y, r, g, b);
        red += r;
        green += g;
        blue += b;
    }

    red = red / (radius * 2);
    green = green / (radius * 2);
    blue = blue / (radius * 2);

    //print_rgb_for_debug(frame, red, green, blue);

    // detetct color using a LUT
    string detected_color = detect_rgb_color(red, green, blue);

    return detected_color;
}

string get_square_color(Mat& frame, vector<Point> square)
{
    int red  = 0, green = 0, blue = 0, r, g, b;
    int mid_left_x = (square[0].x + square[1].x) / 2 , mid_left_y = (square[0].y + square[1].y) / 2;
    int mid_right_x = (square[2].x + square[3].x) / 2 , mid_right_y = (square[2].y + square[3].y) / 2;

    //string temp = to_string(square.size());
    //print_color_at_coordinates(frame, temp, mid_right_x, mid_right_y);
    //print_color_at_coordinates(frame, temp, mid_left_x, mid_left_y);
    //return "";

    //get_pixel_rgb_values(img, col, row, r, g, b);

    for(int i = mid_left_x ; i < mid_left_x + 6; i++)
        for(int j = mid_left_y ; j < mid_left_y + 6; j++)
    {
        get_pixel_rgb_values(frame, i, j, r, g, b);
        red += r;
        green += g;
        blue += b;
    }

    red = red / (36);
    green = green / (36);
    blue = blue / (36);

    //print_rgb_for_debug(frame, red, green, blue);

    // detetct color using a LUT
    string detected_color = detect_rgb_color(red, green, blue);

    return detected_color;
}

string get_triangle_color(Mat& frame, vector<Point> triangle)
{
    int red  = 0, green = 0, blue = 0, r, g, b;
    int approx_center_x = (triangle[0].x + triangle[1].x + triangle[2].x) / 3;
    int approx_center_y = (triangle[0].y + triangle[1].y + triangle[2].y) / 3;

    for(int i = approx_center_x ; i < approx_center_x + 6; i++)
        for(int j = approx_center_y ; j < approx_center_y + 6; j++)
    {
        get_pixel_rgb_values(frame, i, j, r, g, b);
        red += r;
        green += g;
        blue += b;
    }

    red = red / (36);
    green = green / (36);
    blue = blue / (36);

    //print_rgb_for_debug(frame, red, green, blue);

    // detetct color using a LUT
    string detected_color = detect_rgb_color(red, green, blue);

    return detected_color;
}

bool check_darkness(Mat frame)
{
    int limit = min(frame.cols, frame.rows);
    int red = 0, green = 0, blue = 0, r, g, b;
    for (int i = 0; i < limit; i++)
    {
        get_pixel_rgb_values(frame, i, i, r, g, b);
        red += r;
        green += g;
        blue += b;
    }

    red = red / limit;
    green = green / limit;
    blue = blue / limit;
    int mean = (red + green + blue) / 3;

    cout << mean << endl;
    return (mean < 64);
}
