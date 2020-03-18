#include "face_utility.h"

bool is_colored (const Mat& frame)
{
  if (frame.channels() == 3)
    return true;
  else return false;
}

 vector<Rect> detect_faces (Mat frame, CascadeClassifier& face_cascade)
 {
   vector<Rect> faces;

   if(is_colored(frame))
      cvtColor(frame, frame, CV_BGR2GRAY);
   face_cascade.detectMultiScale(frame, faces, 1.2, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

   return faces;
 }

 void draw_face_rect(Mat& frame, const vector<Rect>& faces)
 {
     for (int i = 0; i < faces.size(); i++)
         rectangle(frame, faces[i], Scalar(255, 0, 255));
 }

 int detect_eyes_in_face(Mat frame, const Rect& face, CascadeClassifier& eyes_cascade, Rect& left_eye, Rect& right_eye) // returns 2 on success, otherwise returns number of found eyes
 {
     vector<Rect> eyes;
     if (is_colored(frame))
         cvtColor(frame, frame, CV_BGR2GRAY);

    Mat faceROI = frame(face); //-- In the face, detect eyes

    eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(25, 25));

    if (eyes.size() != 2)
        return eyes.size();
    else
    {
        left_eye = eyes[0];
        right_eye = eyes[1];
    }

    return eyes.size();
 }

 void draw_eyes_rect(Mat& frame, Rect face, Rect left_eye, Rect right_eye) //done, u can make it circle if u want
 {
    /*Point centerR(face.x + right_eye.x + right_eye.width*0.5, face.y + right_eye.y + right_eye.height*0.5);
    int radius = cvRound((left_eye.width + left_eye.height)*0.25);
    circle(frame, centerR, radius, Scalar(255, 0, 255), 4, 8, 0);*/

     Mat faceROI = frame(face);
     rectangle(faceROI, left_eye, Scalar(255, 0, 255));
     rectangle(faceROI, right_eye, Scalar(255, 0, 255));
 }

 int count_number_of_x_in_vec(const vector<int>& vec, int starting_idx, int number)
 {
     int counter = 0;
     for (int i = starting_idx; i < vec.size(); i++)
     {
         if (vec[i] == number)
             counter++;
         else
            break;
     }
     return counter;
 }

 void print_vec(const vector<int>& vec)
 {
     for (int i = 0; i < vec.size(); i++)
     {
         cout << vec[i] << " ";
     }

     cout << endl;
 }

 bool is_user_winking_analysis(vector<int>& wink_history, int num_of_detected_eyes)
 {
     const int two_upper_limit = 12, two_lower_limit = 3, one_upper_limit = 7, one_lower_limit = 1;
     vector<int> state_counter(3, 0);
     int state_idx = 0; // indicating current state
     int number_of_zeroes;
     const int size_of_history = 14;

     if (wink_history.size() >= size_of_history) // if it is larger than x then new values are added to the back and old ones get poped from the front
         wink_history.erase(wink_history.begin());

     wink_history.push_back(num_of_detected_eyes); // new values are always added to the back

     // find and remove zeroes from vector to avoid error -- up to three zeroes will get omitted
     number_of_zeroes = count(wink_history.begin(), wink_history.end(), 0);
     if (number_of_zeroes < 4)
         for (int i = 0; i < number_of_zeroes; i++)
            wink_history.erase(find(wink_history.begin(), wink_history.end(), 0));

     if (wink_history.size() < size_of_history) // there are not enough values yet
         return false;

     //print_vec(wink_history);

    // start sequence detection:
    // state 0:
    state_counter[state_idx] = count_number_of_x_in_vec(wink_history, 0, 2);
    //cout << "num of S0 = " << state_counter[state_idx] << endl;
    if (state_counter[state_idx] > two_lower_limit && state_counter[state_idx] < two_upper_limit)
        state_idx++;
    else
        return false;
    // state 1:
    state_counter[state_idx] = count_number_of_x_in_vec(wink_history, state_counter[0], 1);
    //cout << "num of S1 = " << state_counter[state_idx] << endl;
    if (state_counter[state_idx] > one_lower_limit && state_counter[state_idx] < one_upper_limit)
        state_idx++;
    else
        return false;
    // state 3:
    state_counter[state_idx] = count_number_of_x_in_vec(wink_history, state_counter[0] + state_counter[1], 2);
    //cout << "num of S2 = " << state_counter[state_idx] << endl;
    if (state_counter[state_idx] > two_lower_limit && state_counter[state_idx] < two_upper_limit)
    {
        //cout << state_counter[0] << "  " << state_counter[1] << "  " << state_counter[2] << endl;
        wink_history.clear();
        return true;
    }
    else
        return false;
 }


 bool detect_mouth_in_face(Mat frame, const Rect& face, CascadeClassifier& mouth_cascade, Rect& mouth) // returns number of detected mouths in the given face
 {
     vector<Rect> mouths;
     if (is_colored(frame))
         cvtColor(frame, frame, CV_BGR2GRAY);

     Mat faceROI = frame(face);

     mouth_cascade.detectMultiScale(faceROI, mouths, 1.1, 60, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

     //cout << mouths.size() << endl;
     if (mouths.size() != 1)
         return false;
     else
         mouth = mouths[0];

     return true;
 }

 void draw_mouth_rect(Mat& frame, Rect face, Rect mouth)
 {
     Mat faceROI = frame(face);
     rectangle(faceROI, mouth, Scalar(200, 0, 200));
 }

 bool is_user_smiling_analysis(Mat frame, Rect mouth, CascadeClassifier& smile_cascade)
 {
     vector<Rect> smiles;
     if (is_colored(frame))
         cvtColor(frame, frame, CV_BGR2GRAY);

     Mat mouthROI = frame(mouth); //-- In the mouth, detect smile

     smile_cascade.detectMultiScale(mouthROI, smiles, 1.1, 6, 0 | CV_HAAR_SCALE_IMAGE, Size(15, 15));

     cout << " num of smiles in one mouth : " << smiles.size() << endl;

     if (smiles.size() > 0)
         return true;
     else
         return false;
 }


/*vector< pair<Rect, int> > recognize_and_pair(Mat frame, vector<Rect> faces, const Ptr<FaceRecognizer>& model)  // Process face by face:
{
    cvtColor(frame, frame, CV_BGR2GRAY);
    vector< pair<Rect, int> > pairs;

    for (int i = 0; i < faces.size(); i++)
    {
        Mat face = frame(faces[i]); // Crop the face from the image
        //resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC)
        // Now perform the prediction:
        int prediction = model->predict(face);
        pair<Rect, int> paired(faces[i], prediction);
        pairs.push_back(paired);
    }

    return pairs;
}*/

 int predict_one_face(Mat face, const Ptr<FaceRecognizer>& model)
{
    if (is_colored(face))
        cvtColor(face, face, CV_BGR2GRAY);

    int label;
    double confidence; // confidence/distance  = 10.0 --> the lower the threshold the more confident the prediction (minimum = 0.0)
    model->predict(face, label, confidence);

    // it will return a label in any case
    // make it return -1 if it is not close enough to threshold
    if (confidence > 60)
        label = -1;

    cout << confidence << endl;

    return label;
}

void show_prediction(Mat& frame, pair<Rect, int> face_label_pair)
{
    // Create the text we will annotate the box with:
    string box_text = format("ID: %d", face_label_pair.second);
    // Calculate the position for annotated text (make sure we don't put illegal values in there)
    int pos_x = max(face_label_pair.first.tl().x - 10, 0);
    int pos_y = max(face_label_pair.first.tl().y - 10, 0);
    // And now put it into the image:
    putText(frame, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0), 2.0);

    rectangle(frame, face_label_pair.first, Scalar(255, 0, 255)); // draw face rect
}
