#include "camera.h"

int count_diff_pixels (Mat in1, Mat in2)
{
    Mat diff, grey;
    cvtColor(in1, in1, CV_BGR2GRAY);
    cvtColor(in2, in2, CV_BGR2GRAY);
    absdiff(in1, in2, diff);
    //cvtColor(diff, grey, CV_BGR2GRAY);
    return countNonZero(diff);
}

double get_diff_percent(const Mat& prev, const Mat& current, const Mat& next, Mat& result, double rows, double cols) // returns the diff percent
{
    //double diff = count_diff_pixels(img1, img2);

    Mat d1, d2, grey;

    absdiff(prev, next, d1);
    absdiff(current, next, d2);
    bitwise_and(d1, d2, result);
    threshold(result, result, 20, 255, CV_THRESH_BINARY);
    cvtColor(result, grey, CV_BGR2GRAY);

    double diff = countNonZero(grey);
    return (diff / (rows * cols)) * 100;
}

////////////////////////////////////////

camera::camera(int cam_number) : cap(cam_number), camera_number(cam_number)
{
    //cap.open(camera_number);
    face_cascade_name = "haarcascade_frontalface_alt.xml";
    eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
    mouth_cascade_name = "haarcascade_mcs_mouth.xml";
    smile_cascade_name = "haarcascade_smile.xml";

    if (!face_cascade.load(face_cascade_name) || !eyes_cascade.load(eyes_cascade_name) || !mouth_cascade.load(mouth_cascade_name) || !smile_cascade.load(smile_cascade_name))
      throw face_cascade_not_loaded_exception();

    cap.set(CV_CAP_PROP_FPS, FPS);
    cap.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);

    if (!cap.isOpened())
       throw video_stream_wont_open_exception();

    LBP_model = createLBPHFaceRecognizer();
    LBP_model->load(LBP_model_path);
    FF_model = createFisherFaceRecognizer();
    FF_model->load(FF_model_path);

    ifstream label_file;
    label_file.open(current_face_label_path.c_str(), ios::in);
    label_file >> current_face_label;
    label_file.close();
}

void camera::show_frame()
{
    if (frame.empty())
      throw empty_frame_captured_exception();

    imshow(stream_name, frame);
}

Mat camera::get_frame()
{
    if (frame.empty())
      throw empty_frame_captured_exception();

    return frame;
}

string camera::get_window_name()
{
    return stream_name;
}

//////////////////////////////////////////////////////  --- face_detect:

void camera::draw_faces()
{
    if (frame.empty())
      throw empty_frame_captured_exception();

    draw_face_rect(frame, faces);
}

bool camera::is_there_a_face()
{
    cap >> frame;
    if (frame.empty())
      throw empty_frame_captured_exception();

    faces = detect_faces(frame, face_cascade);

    return faces.size();
}

int camera::get_number_of_faces ()
{
    if (frame.empty())
      throw empty_frame_captured_exception();

    return faces.size ();
}

bool camera::detect_eyes()
{
    if (frame.empty())
        throw empty_frame_captured_exception();

    if (faces.size() != 1)
        return false;

    num_of_detected_eyes = detect_eyes_in_face(frame, faces[0], eyes_cascade, left_eye, right_eye);

    if (num_of_detected_eyes == 2)
        return true;
    else
        return false;
}

void camera::draw_eyes()
{
    if (frame.empty())
        throw empty_frame_captured_exception();

    draw_eyes_rect(frame, faces[0], left_eye, right_eye);
}

bool camera::is_user_winking()
{
    detect_eyes();
    return is_user_winking_analysis(wink_history, num_of_detected_eyes);
}

bool camera::detect_mouth()
{
    if (frame.empty())
        throw empty_frame_captured_exception();

    if (faces.size() != 1)
        return false;

   return detect_mouth_in_face(frame, faces[0], mouth_cascade, mouth);
}

void camera::draw_mouth()
{
    if (frame.empty())
        throw empty_frame_captured_exception();

    draw_mouth_rect(frame, faces[0], mouth);
}

bool camera::is_user_smiling()
{
    if (!detect_mouth())
        return false;
    else
        return is_user_smiling_analysis(frame, mouth, smile_cascade);
}

///////////////////////////////////////////////////////  --- face rec:

string camera::take_face_pictures(const int number_of_desired_faces) // return values: 1. success 2.more_than_one_face_detected 3.no_face_detected
{
    int no_face_flag = 0, too_much_face_flag = 0, num_of_gathered_faces = 0;

    for (int i = 0; i < number_of_desired_faces * 3; i++)
    {
        if (is_there_a_face())
        {
            if (faces.size() == 1)
            {
                Mat face_resized(face_size, face_size, frame.type());
                Mat faceRect = frame(faces[0]);
                resize(faceRect, face_resized, face_resized.size(), 0 , 0);

                //candidate_images.push_back(frame(faces[0]));
                if (is_colored(face_resized))
                    cvtColor(face_resized, face_resized, CV_BGR2GRAY);

                candidate_images.push_back(face_resized);
                num_of_gathered_faces++;
            }
            else
                too_much_face_flag++;
        }
        else
            no_face_flag++;
    }

    if (num_of_gathered_faces >= number_of_desired_faces)
        return "success";

    // on failure:
    for (int i = 0; i < num_of_gathered_faces; i++)
        candidate_images.pop_back();

    if (no_face_flag < too_much_face_flag)
        return "more_than_one_face_detected";
    else
        return "no_face_detected";
}

string camera::take_candidate_images_for_learning() // call this function 4 times to gather the reqiured training data
{
    // states:  0.ready -- 1.frontal -- 2.right_tilt -- 3.left_tilt -- 4.done

    if (learning_state == 4)
    {
        learning_state = 0;
        candidate_images.clear();
        // then we can go on ...
    }

    else //if (learning_state == 0 - 3)
    {
        string answer = take_face_pictures(10);
        if (answer != "success") // on failure:
            return answer;
        else // on success:
        {
            learning_state++;
            return answer;
        }
    }

    if (learning_state == 4)
        return "success";
}

void camera::check_gathered_faces()
{
    for (int i = 0; i < candidate_images.size(); i++)
    {
        string a ="gathered_faces";
        imshow(a, candidate_images[i]);
        waitKey(); // doesnt quite work in Qt
        //int c = waitKey();
        //while((char)c != 'c');        //if ((char)c == 'c' || c == 27)
        // should pass the frame directly to Qt main to work
    }
}

bool camera::train_LBP_model_based_on_one_person(int face_label) // build and load a model
{
    vector<int> labels(candidate_images.size(), face_label);

    //LBP_model->train(candidate_images, labels);
    LBP_model->update(candidate_images, labels);
    LBP_model->save(LBP_model_path);

    cout << "Model trained successfully" << endl;

    return true;
}

void camera::read_csv(vector<Mat>& images, vector<int>& labels, char separator)
{
    ifstream file(csv_path.c_str(), ifstream::in);
    if (!file)
    {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line, path, classlabel;
    while (getline(file, line))
    {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if (!path.empty() && !classlabel.empty())
        {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}

void camera::save_trained_faces_of_one_person(int face_label)// if face_label is not provided, current_face_label is used
{
    // save taken candidate_images with the given label to disk and add to csv
    // if label is not provided increase current_face_label then use it
    if (face_label == -1)
    {
        increase_current_face_label();
        face_label = current_face_label;
    }

    for (int i = 0; i < candidate_images.size(); i++)
    {
        string filename = path_to_images + convert_number_to_string(face_label) + "_" + convert_number_to_string(i) + ".jpg";
        imwrite(filename, candidate_images[i]);
    }

    ofstream csv_dataset;
    csv_dataset.open(csv_path.c_str(), ios::app | ios::out);
    for (int i = 0; i < candidate_images.size(); i++)
    {
        string filename = path_to_images + convert_number_to_string(face_label) + "_" + convert_number_to_string(i) + ".jpg";
        csv_dataset << filename << ";" << convert_number_to_string(face_label) << endl;
    }
    csv_dataset.close();
    //candidate_images.clear();
}

void camera::train_LBP_model()
{
    vector<Mat> face_images;
    vector<int> labels;
    read_csv(face_images, labels);

    LBP_model->train(face_images, labels);
    FF_model->set("threshold", 40);
    LBP_model->save(LBP_model_path);
}

void camera::train_FF_model()
{
    vector<Mat> face_images;
    vector<int> labels;
    read_csv(face_images, labels);

    FF_model->train(face_images, labels);
    //FF_model->set("threshold", 46);
    FF_model->save(FF_model_path);
}

int camera::predict_face(string _directive) // safe
{
    int label = 0;
    if (is_there_a_face())
    {
        if (faces.size() == 1)
        {
            if (_directive == "LBP")
                label = predict_one_face(frame(faces[0]), LBP_model);
            else if (_directive == "FF")
            {
                Mat face_resized(face_size, face_size, frame.type());
                resize(frame(faces[0]), face_resized, face_resized.size(), 0, 0);
                label = predict_one_face(face_resized, FF_model);
            }
            else
                throw bad_face_model_directive_exception();
        }
        else
            return -2; //"more_than_one_face_detected";
    }
    else
        return -3; // "no_face_detected";

    return label;
}

void camera::draw_prediction_result(int label)
{
    // no face detected(-3) or multiple detected(-2)
    // label = -1 --> detected face cannot be recognized
    if (label >= -1)
    {
        pair<Rect, int> face_label_pair(faces[0], label);
        show_prediction(frame, face_label_pair);
    }

    return;
}

void camera::increase_current_face_label()
{
    current_face_label++;
    ofstream label_file;
    label_file.open(current_face_label_path.c_str(), ios::out | ios::trunc);
    label_file << current_face_label << endl;
    label_file.close();
}

int camera::get_current_face_label()
{
    return current_face_label;
}

//////////////////////////////////////////////////////  --- color:

bool camera::is_it_dark_outside()
{
    cap >> frame;
    if (frame.empty())
        throw empty_frame_captured_exception();

    return check_darkness(frame);
}

void camera::detect_color()
{
    cap >> frame;
    if (frame.empty())
      throw empty_frame_captured_exception();

    color = get_the_color(frame);
}

bool camera::detect_the_best_color ()
{
    const int num_of_frames_to_check = 30;
    int num_of_frames = num_of_frames_to_check;

    enum COLOR {BLACK, WHITE, PURPLE, PINK, ORANGE, BROWN, YELLOW, RED, BLUE, GREEN};
    vector<int> color_reps(10 , 0);

    Mat prev_frame, new_frame, result;
    cap >> prev_frame;
    cap >> frame;
    if (frame.empty())
        throw empty_frame_captured_exception();

    for(num_of_frames; num_of_frames > 0; num_of_frames--)
    {
        cap >> new_frame;

        double diff = get_diff_percent(prev_frame, frame, new_frame, result, frame.rows, frame.cols);
        if (diff > 10)
            break;

        prev_frame = frame.clone();
        frame = new_frame.clone();

        int c = waitKey(1);
        if ((char)c == 'c' || c == 27)
            break;
    }

    imshow("shape_in_sub_frame", frame);
    waitKey(30);

    for (num_of_frames; num_of_frames > 0; num_of_frames--)
    {
        cap >> new_frame;
        // declaring 3/5 ROI for detecting shapes
        Mat sub_frame = new_frame(Rect(frame.cols / 2 - frame.cols / 4 , frame.rows / 2 - frame.rows / 4, frame.cols / 2, frame.rows / 2));

        string detected_color = get_the_color(sub_frame);

        if (detected_color == "black")
            color_reps[BLACK]++;
        else if (detected_color == "white")
            color_reps[WHITE]++;
        else if (detected_color == "purple")
            color_reps[PURPLE]++;
        else if (detected_color == "pink")
            color_reps[PINK]++;
        else if (detected_color == "orange")
            color_reps[ORANGE]++;
        else if (detected_color == "brown")
            color_reps[BROWN]++;
        else if (detected_color == "yellow")
            color_reps[YELLOW]++;
        else if (detected_color == "red")
            color_reps[RED]++;
        else if (detected_color == "blue")
            color_reps[BLUE]++;
        else if (detected_color == "green")
            color_reps[GREEN]++;
    }

    int max_idx = 0;

    for(int i = 0; i < 10; i++)
    {
        if (color_reps[max_idx] < color_reps[i])
            max_idx = i;
    }

    if (color_reps[max_idx] < 5)
    {
        color = "could_not_detect";
        return false;
    }
    else
    {
        if (max_idx == 0)
            color = "black";
        else if (max_idx == 1)
            color = "white";
        else if (max_idx == 2)
            color = "purple";
        else if (max_idx == 3)
            color = "pink";
        else if (max_idx == 4)
            color = "orange";
        else if (max_idx == 5)
            color = "brown";
        else if (max_idx == 6)
            color = "yellow";
        else if (max_idx == 7)
            color = "red";
        else if (max_idx == 8)
            color = "blue";
        else if (max_idx == 9)
            color = "green";
    }

    return true;
}

void camera::print_color ()
{
    if (frame.empty())
      throw empty_frame_captured_exception();

    color_utility::print_color (frame, color);
}

string camera::get_detected_color ()
{
    if (frame.empty())
      throw empty_frame_captured_exception();

    return color;
}

//////////////////////////////////////////////////////  --- shapes:

void camera::detect_shapes ()
{
    cap >> frame;
    if (frame.empty())
      throw empty_frame_captured_exception();

    circles = detect_circles(frame);
    triangles = findTriangles(frame);
    squares = findSquares(frame);
}

void camera::detect_shapes(Mat my_frame)
{
    //cap >> frame;
    if (my_frame.empty())
        throw empty_frame_captured_exception();

    circles = detect_circles(my_frame);
    triangles = findTriangles(my_frame);
    squares = findSquares(my_frame);
}

void camera::draw_shapes ()
{
    if (frame.empty())
      throw empty_frame_captured_exception();

    draw_circles(frame, circles);
    draw_objects(frame, triangles);
    draw_objects(frame, squares);
}

void camera::draw_shapes(Mat my_frame)
{
    if (my_frame.empty())
        throw empty_frame_captured_exception();

    draw_circles(my_frame, circles);
    draw_objects(my_frame, triangles);
    draw_objects(my_frame, squares);
}

void camera::print_shapes ()
{
    if (frame.empty())
      throw empty_frame_captured_exception();

    shape_utility::print_shapes (frame, squares, triangles, circles);
}

bool camera::detect_the_best_shape () // this method takes a while to proceed
{
    // find a moving blob
    // 1. use ROI of 3/5 scale
    // 2. count_pixel_diff > num_of_pixels/x
    // 3. give it a time period and count the most appearance
    // 4. return 1.nothing detected
                // 2.multiple detected with very similar appearances
                // 3.
    const int num_of_frames_to_check = 30;
    int num_of_frames = num_of_frames_to_check;

    Mat prev_frame, new_frame, result;
    cap >> prev_frame;
    cap >> frame;
    if (frame.empty())
        throw empty_frame_captured_exception();

    for(num_of_frames; num_of_frames > 0; num_of_frames--)
    {
        cap >> new_frame;

        double diff = get_diff_percent(prev_frame, frame, new_frame, result, frame.rows, frame.cols);
        if (diff > 10)
            break;

        //string text = convert_number_to_string(diff);
        //putText(new_frame, text.c_str(), cvPoint(100, 100), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
        //imshow("!!", new_frame);
        //imshow("test", sub_frame);
        //imshow("diff", result);


        prev_frame = frame.clone();
        frame = new_frame.clone();

        int c = waitKey(1);
        if ((char)c == 'c' || c == 27)
            break;
    }

    imshow("shape_in_sub_frame", frame);
    waitKey(30);
    int num_of_squares = 0, num_of_triangles = 0, num_of_circles = 0;
    for (num_of_frames; num_of_frames > 0; num_of_frames--)
    {
        cap >> new_frame;
        // declaring 3/5 ROI for detecting shapes
        Mat sub_frame = new_frame(Rect(frame.cols / 5, frame.rows / 5, frame.cols / 5 * 3, frame.rows / 5 * 3));

        detect_shapes(sub_frame);
        //draw_shapes(sub_frame);
        //imshow("shape_in_sub_frame", sub_frame);
        //waitKey();

        num_of_squares += squares.size();
        num_of_triangles += triangles.size();
        num_of_circles += circles.size();
    }

    cout <<"triangles = " << num_of_triangles << endl << "circles = " << num_of_circles << endl << "squares = " << num_of_squares << endl;


    int max_num = max(num_of_triangles, max(num_of_circles, num_of_squares));
    if (num_of_triangles >= 2 && num_of_circles < 2 && num_of_squares < 2)
        shape = "triangle";
    if (max_num < 4)
    {
        shape = "could_not_detect";
        return false;
    }
    else
    {
        if (num_of_circles > num_of_squares &&  num_of_circles > num_of_triangles)
            shape = "circle";
        if (num_of_squares > num_of_circles &&  num_of_squares > num_of_triangles)
            shape = "square";
        if (num_of_triangles > num_of_squares &&  num_of_triangles > num_of_circles)
            shape = "triangle";
    }


    return true;
}

string camera::get_detected_shape (string _directive) // directives: 1.default (robust shape passing) ,  2.debug
{
    if (_directive == "default")
    {
        if (shape == "" || shape == "could_not_detect" || shape == "could_not_choose")
            return "";
        else
            return shape;
    }
    else if (_directive == "debug")
    {
        return shape;
    }
    else throw bad_shape_directive_exception();
}


bool camera::get_the_best_shape_and_its_color()
{
    const int num_of_frames_to_check = 30;
    int num_of_frames = num_of_frames_to_check;

    enum COLOR {BLACK, WHITE, PURPLE, PINK, ORANGE, BROWN, YELLOW, RED, BLUE, GREEN};
    vector<int> square_color_reps(10 , 0);
    vector<int> circle_color_reps(10 , 0);
    vector<int> triangle_color_reps(10 , 0);

    Mat prev_frame, new_frame, result;
    cap >> prev_frame;
    cap >> frame;
    if (frame.empty())
        throw empty_frame_captured_exception();

    for(num_of_frames; num_of_frames > 0; num_of_frames--)
    {
        cap >> new_frame;

        double diff = get_diff_percent(prev_frame, frame, new_frame, result, frame.rows, frame.cols);
        if (diff > 10)
            break;

        prev_frame = frame.clone();
        frame = new_frame.clone();

        int c = waitKey(1);
        if ((char)c == 'c' || c == 27)
            break;
    }

    imshow("shape_in_sub_frame", frame);
    waitKey(30);
    int num_of_squares = 0, num_of_triangles = 0, num_of_circles = 0;
    for (num_of_frames; num_of_frames > 0; num_of_frames--)
    {
        cap >> new_frame;
        // declaring 3/5 ROI for detecting shapes
        Mat sub_frame = new_frame(Rect(frame.cols / 5, frame.rows / 5, frame.cols / 5 * 3, frame.rows / 5 * 3));

        detect_shapes(sub_frame);
        //draw_shapes(sub_frame);
        //imshow("shape_in_sub_frame", sub_frame);
        //waitKey();

        num_of_squares += squares.size();
        num_of_triangles += triangles.size();
        num_of_circles += circles.size();

        // color of shapes:
        if(squares.size() > 0)
        {
            string detected_color = get_square_color(sub_frame, squares[0]);

            if (detected_color == "black")
                square_color_reps[BLACK]++;
            else if (detected_color == "white")
                square_color_reps[WHITE]++;
            else if (detected_color == "purple")
                square_color_reps[PURPLE]++;
            else if (detected_color == "pink")
                square_color_reps[PINK]++;
            else if (detected_color == "orange")
                square_color_reps[ORANGE]++;
            else if (detected_color == "brown")
                square_color_reps[BROWN]++;
            else if (detected_color == "yellow")
                square_color_reps[YELLOW]++;
            else if (detected_color == "red")
                square_color_reps[RED]++;
            else if (detected_color == "blue")
                square_color_reps[BLUE]++;
            else if (detected_color == "green")
                square_color_reps[GREEN]++;
        }

        if(circles.size() > 0)
        {
            string detected_color = get_circle_color(sub_frame, circles[0]);

            if (detected_color == "black")
                circle_color_reps[BLACK]++;
            else if (detected_color == "white")
                circle_color_reps[WHITE]++;
            else if (detected_color == "purple")
                circle_color_reps[PURPLE]++;
            else if (detected_color == "pink")
                circle_color_reps[PINK]++;
            else if (detected_color == "orange")
                circle_color_reps[ORANGE]++;
            else if (detected_color == "brown")
                circle_color_reps[BROWN]++;
            else if (detected_color == "yellow")
                circle_color_reps[YELLOW]++;
            else if (detected_color == "red")
                circle_color_reps[RED]++;
            else if (detected_color == "blue")
                circle_color_reps[BLUE]++;
            else if (detected_color == "green")
                circle_color_reps[GREEN]++;
        }

        if(triangles.size() > 0)
        {
            string detected_color = get_triangle_color(sub_frame, triangles[0]);

            if (detected_color == "black")
                triangle_color_reps[BLACK]++;
            else if (detected_color == "white")
                triangle_color_reps[WHITE]++;
            else if (detected_color == "purple")
                triangle_color_reps[PURPLE]++;
            else if (detected_color == "pink")
                triangle_color_reps[PINK]++;
            else if (detected_color == "orange")
                triangle_color_reps[ORANGE]++;
            else if (detected_color == "brown")
                triangle_color_reps[BROWN]++;
            else if (detected_color == "yellow")
                triangle_color_reps[YELLOW]++;
            else if (detected_color == "red")
                triangle_color_reps[RED]++;
            else if (detected_color == "blue")
                triangle_color_reps[BLUE]++;
            else if (detected_color == "green")
                triangle_color_reps[GREEN]++;
        }
    }

    cout <<"triangles = " << num_of_triangles << endl << "circles = " << num_of_circles << endl << "squares = " << num_of_squares << endl;


    int max_num = max(num_of_triangles, max(num_of_circles, num_of_squares));

    // final color detection part:
    int square_max_idx = 0;
    int circle_max_idx = 0;
    int triangle_max_idx = 0;

    for(int i = 0; i < 10; i++)
    {
        if (square_color_reps[square_max_idx] < square_color_reps[i])
            square_max_idx = i;
    }

    for(int i = 0; i < 10; i++)
    {
        if (circle_color_reps[circle_max_idx] < circle_color_reps[i])
            circle_max_idx = i;
    }

    for(int i = 0; i < 10; i++)
    {
        if (triangle_color_reps[triangle_max_idx] < triangle_color_reps[i])
            triangle_max_idx = i;
    }

    string square_color,circle_color, triangle_color;
    if (square_max_idx == 0)
        square_color = "black";
    else if (square_max_idx == 1)
        square_color = "white";
    else if (square_max_idx == 2)
        square_color = "purple";
    else if (square_max_idx == 3)
        square_color = "pink";
    else if (square_max_idx == 4)
        square_color = "orange";
    else if (square_max_idx == 5)
        square_color = "brown";
    else if (square_max_idx == 6)
        square_color = "yellow";
    else if (square_max_idx == 7)
        square_color = "red";
    else if (square_max_idx == 8)
        square_color = "blue";
    else if (square_max_idx == 9)
        square_color = "green";
    /////////////////////////////////////////////
    if (circle_max_idx == 0)
        circle_color = "black";
    else if (circle_max_idx == 1)
        circle_color = "white";
    else if (circle_max_idx == 2)
        circle_color = "purple";
    else if (circle_max_idx == 3)
        circle_color = "pink";
    else if (circle_max_idx == 4)
        circle_color = "orange";
    else if (circle_max_idx == 5)
        circle_color = "brown";
    else if (circle_max_idx == 6)
        circle_color = "yellow";
    else if (circle_max_idx == 7)
        circle_color = "red";
    else if (circle_max_idx == 8)
        circle_color = "blue";
    else if (circle_max_idx == 9)
        circle_color = "green";
    /////////////////////////////////////////////
    if (triangle_max_idx == 0)
        triangle_color = "black";
    else if (triangle_max_idx == 1)
        triangle_color = "white";
    else if (triangle_max_idx == 2)
        triangle_color = "purple";
    else if (triangle_max_idx == 3)
        triangle_color = "pink";
    else if (triangle_max_idx == 4)
        triangle_color = "orange";
    else if (triangle_max_idx == 5)
        triangle_color = "brown";
    else if (triangle_max_idx == 6)
        triangle_color = "yellow";
    else if (triangle_max_idx == 7)
        triangle_color = "red";
    else if (triangle_max_idx == 8)
        triangle_color = "blue";
    else if (triangle_max_idx == 9)
        triangle_color = "green";

    //////////////////////////////////////
    if (num_of_triangles >= 2 && num_of_circles < 2 && num_of_squares < 2)
    {
        shape = "triangle";
        color = triangle_color;
    }
    else if (max_num < 4)
    {
        shape = "could_not_detect";
        color = "could_not_detect";
        return false;
    }
    else
    {
        if (num_of_circles > num_of_squares &&  num_of_circles > num_of_triangles)
        {
            shape = "circle";
            color = circle_color;
        }
        if (num_of_squares > num_of_circles &&  num_of_squares > num_of_triangles)
        {
            shape = "square";
            color = square_color;
        }
        if (num_of_triangles > num_of_squares &&  num_of_triangles > num_of_circles)
        {
            shape = "triangle";
            color = triangle_color;
        }
    }

    return true;
}
