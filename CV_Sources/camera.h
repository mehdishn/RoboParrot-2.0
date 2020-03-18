#ifndef CAM_H_
#define CAM_H_

#include <utility>
#include <fstream>
#include <sstream>
#include "face_utility.h"
#include "shape_utility.h"
#include "color_utility.h"
#include "num_to_str.h"

const int WIDTH = 450;
const int HEIGHT = 350;
const int FPS = 5;

const int face_size = 150;

class empty_frame_captured_exception{};
class face_cascade_not_loaded_exception{};
class video_stream_wont_open_exception{};
class bad_shape_directive_exception{};
class bad_face_model_directive_exception{};

class camera
{
public:
    camera(int cam_number);
    void show_frame ();
    Mat get_frame ();
    string get_window_name ();

public: // face_detect:
    bool is_there_a_face ();
    void draw_faces ();
    int get_number_of_faces ();
    bool detect_eyes ();
    void draw_eyes ();
    bool is_user_winking ();

    bool detect_mouth();
    void draw_mouth();
    bool is_user_smiling();

public: // face rec
    string take_candidate_images_for_learning(); // call this function 4 times to gather the reqiured training data
    // wait till you get 4 "success" -- possible errors: "more_than_one_face_detected" or "no_face_detected"
    void check_gathered_faces();

    void save_trained_faces_of_one_person(int face_label = -1); // if face_label is not provided, current_face_label is used
    void train_LBP_model();
    void train_FF_model();

    bool train_LBP_model_based_on_one_person(int face_label = 0); // gets the label for the facc you want to train, and the model you want to use
    int predict_face(string _directive); // returns label on success -- returns -1 if more_than_one_face_detected, -2 if no_face_detected
    // _directives: 1.FF 2.LBP
    void draw_prediction_result(int label);

    int get_current_face_label();
public: // color:
    void detect_color ();
    bool detect_the_best_color ();
    void print_color ();
    string get_detected_color ();
    bool is_it_dark_outside();
public: // shapes:
    void detect_shapes();
    void detect_shapes (Mat);
    void draw_shapes();
    void draw_shapes(Mat);
    void print_shapes ();
    bool detect_the_best_shape ();
    string get_detected_shape (string _directive = "default"); // directives: 1.default (robust shape passing) ,  2.debug
    bool get_the_best_shape_and_its_color();

private:
    Mat frame;
    VideoCapture cap;
    int camera_number;
    string stream_name = "Video_Stream_" + convert_number_to_string(camera_number) + "!";
private: // face_detect
    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;
    CascadeClassifier mouth_cascade;
    CascadeClassifier smile_cascade;
    Rect left_eye;
    Rect right_eye;
    vector<Rect> faces;
    string face_cascade_name;
    string eyes_cascade_name;
    string mouth_cascade_name;
    string smile_cascade_name;
    int num_of_detected_eyes;
    vector<int> wink_history;
    Rect mouth;
private: // face recognition
    string current_face_label_path = "./current_face_label";
    int current_face_label;
    void increase_current_face_label();

    int learning_state = 0; // states:  0.ready -- 1.frontal -- 2.right_tilt -- 3.left_tilt -- 4.done
    vector<Mat> candidate_images;
    string csv_path = "./dataset.csv";
    string LBP_model_path = "./LBP_model";
    string FF_model_path = "./FF_model";
    Ptr<FaceRecognizer> LBP_model;
    Ptr<FaceRecognizer> FF_model;
    string path_to_images = "./face_rec_data/";
    string take_face_pictures(const int number_of_desired_faces);
    void read_csv(vector<Mat>& images, vector<int>& labels, char separator = ';');
private: // color
    string color;
private: // shapes
    vector<Vec3f> circles;
    vector<vector<Point> > squares;
    vector<vector<Point> > triangles;
    string shape = "";
};


#endif
