#ifndef F_U_H_
#define F_U_H_

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <algorithm>

 using namespace std;
 using namespace cv;



 bool is_colored (const Mat& frame);
 vector<Rect> detect_faces (Mat frame, CascadeClassifier& face_cascade);
 void draw_face_rect(Mat& frame, const vector<Rect>& faces);
 int detect_eyes_in_face(Mat frame, const Rect& face, CascadeClassifier& eyes_cascade, Rect& left_eye, Rect& right_eye);
 void draw_eyes_rect(Mat& frame, Rect face, Rect left_eye, Rect right_eye);
 bool is_user_winking_analysis(vector<int>& wink_history, int num_of_detected_eyes);


 bool detect_mouth_in_face(Mat frame, const Rect& face, CascadeClassifier& mouth_cascade, Rect& mouth); // returns number of detected mouths in the given face
 void draw_mouth_rect(Mat& frame, Rect face, Rect mouth);
 bool is_user_smiling_analysis(Mat frame, Rect mouth, CascadeClassifier& smile_cascade);

 int predict_one_face(Mat face, const Ptr<FaceRecognizer>& model); // very high threshold
 //vector< pair<Rect, int> > recognize_and_pair(Mat frame, vector<Rect> faces, const Ptr<FaceRecognizer>& model);  // predict multiple faces in a frame
 void show_prediction(Mat& frame, pair<Rect, int> face_label_pair);

#endif
