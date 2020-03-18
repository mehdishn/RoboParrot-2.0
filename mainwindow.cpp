#include "mainwindow.h"
#include "ui_mainwindow.h"


#define main_camera_def;
const int main_camera_check = 1;
const int main_camera_record_stream = 0;

// edit this to control modules
const int overhead_cam_check = 0;
const int overhead_camera_number = 1;

const int speech_check = 0;
const int curtain_check = 1;
const int robot_body_check = 0;
const int tcp_server_check = 1;
const int plug_and_play_check = 1;
const int face_rec_check = 0;

#ifdef main_camera_def
    camera cam1(0);
#endif

QString sounds_address = "C:\\Users\\RoboLab\\Desktop\\Sounds\\";
//QString sounds_address = "C:\\Users\\Mehdi\\Desktop\\Projects - Master\\RoboParrot\\Sounds\\";
// add frontal face cascades next to .exe
// modify com ports in plug and play function if needed

int Shape_Det = -1, Color_Det = -1, Face_Det = -1, Face_Rec_FF = -1, Face_Rec_LBP = -1;
int predicted_label = -1;

void MainWindow::main_vision_func()
{
    if(main_camera_record_stream)
    {
        try
        {
            Mat frame = cam1.get_frame();
            main_camera_video.write(frame);
        }
        catch (empty_frame_captured_exception ex1)
        {
           qDebug()<<"empty_frame_captured_exception\n";
        }
    }
    try
    {
        if(is_even(Color_Det))
            cam1.detect_color();
        if(is_even(Shape_Det))
            cam1.detect_shapes();
        if(is_even(Face_Rec_LBP))
            predicted_label = cam1.predict_face("LBP");
        else if(is_even(Face_Rec_FF))
            predicted_label =  cam1.predict_face("FF");
        else if(is_even(Face_Det))
            cam1.is_there_a_face();

        if(is_even(Color_Det))
          cam1.print_color();
        if(is_even(Shape_Det))
          cam1.draw_shapes();
        if(is_even(Face_Rec_FF) || is_even(Face_Rec_LBP))
          cam1.draw_prediction_result(predicted_label);
        else if(is_even(Face_Det))
               cam1.draw_faces();

         if(is_even(Color_Det) || is_even(Face_Det) || is_even(Shape_Det) || is_even(Face_Rec_FF) || is_even(Face_Rec_LBP))
         {
              cam1.show_frame();
         }
         else
               cv::destroyAllWindows();
    }
    catch (empty_frame_captured_exception ex1)
    {
       qDebug()<<"empty_frame_captured_exception\n";
    }
    catch (face_cascade_not_loaded_exception ex2)
    {
       qDebug()<<"face_cascade_not_loaded_exception\n"; // does not work properly ?!!!
    }
    catch (video_stream_wont_open_exception ex3)
    {
        qDebug()<<"video_stream_wont_open_exception\n";
    }
    catch (...)
    {
        qDebug()<<"other_exceptions\n";
    }
}

void MainWindow::read_from_tcp_server(QByteArray data)
{
    //QByteArray data;
    //server.dataReceived(data);

    string str = data.constData();
    printf("%s\n", str.c_str());
    printf("hello\n");
}

void MainWindow::new_tcp_connection()
{
    // need to grab the socket
    socket = server->nextPendingConnection();
    socket->setReadBufferSize(2048);
    connect(socket, SIGNAL(readyRead()), SLOT(read_data_from_socket()));
}

void MainWindow::read_data_from_socket()
{
    QByteArray data;
    string str_data;

    if (socket->bytesAvailable())
    {
        data = socket->readAll();
        str_data = data.constData();
        qDebug() << "incoming socket data: " << data;
        socket->write("Got it.");
        socket->flush();
        socket->waitForBytesWritten(50);
        socket->close();

        handle_tcp_command(str_data);
    }
    else
    {
        socket->write("could not receive bytes");
        socket->flush();
        socket->waitForBytesWritten(50);
        socket->close();
    }
}

void MainWindow::handle_tcp_command(string cmd)
{
    //      list of voice commands:
    // salam, khubi, afarin, bale, bia_bazi, biayn_bazi, bia_pisham, koja_rafti, koja_rafti_bia
    // bebin_ghafasamo, mikham_bekhabam_khaste_shodam, khande
    // khodahafez, iloveyou, mersi, nakheyr

    //       vision commands:
    // detect_shape_and_color, detect_color, detect_shape

    //      movement commands:
    // blink, eyes_open, eyes_close, move

    //       other commands:
    // sleep, snore, Parrot_sound1, parrot_sound2, music_bozi

    if(cmd == "close_curtain") // a signal issued from the server before kiling this task
        Close_Curtain();


    else if(cmd == "salam")
        on_Salam_clicked();
    else if(cmd == "khubi")
        on_Khoobi_clicked();
    else if(cmd == "afarin")
        on_Afarin_clicked();
    else if(cmd == "bale")
        on_Bale_clicked();
    else if(cmd == "bia_bazi")
        on_Bia_Bazi_clicked();


    else if(cmd == "khande")
        on_Khande_clicked();
    else if(cmd == "khodahafez")
        on_Khodahafez_clicked();
    else if(cmd == "iloveyou")
        on_Love_You_clicked();
    else if(cmd == "mersi")
        on_Merci_clicked();
    else if(cmd == "nakheyr")
        on_Nakheir_clicked();


    else if(cmd == "biayn_bazi")
        on_BiainBaManBazi_clicked();
    else if(cmd == "bia_pisham")
        on_BiaPisheMan_clicked();
    else if(cmd == "koja_rafti")
        on_KojaRafti_clicked();
    else if(cmd == "koja_rafti_bia")
        on_KojaRaftiBia_clicked();
    else if(cmd == "bebin_ghafasamo")
        on_BebinGhafasamo_clicked();
    else if(cmd == "mikham_bekhabam_khaste_shodam")
        on_MikhamBekhabamKhasteShodam_clicked();




    // vision commands:

    else if(cmd == "detect_shape")
        on_Manual_Shape_clicked();
    else if(cmd == "detect_color")
        on_Manual_Color_clicked();
    else if(cmd == "detect_shape_and_color")
        on_manual_shape_color_int_clicked();


    // movement commands:

    else if(cmd == "blink")
        on_eye_blink_clicked();
    else if(cmd == "eyes_open")
        on_eye_open_clicked();
    else if(cmd == "eyes_close")
        on_eye_close_clicked();
    else if(cmd == "move")
        on_start_position_clicked();



    else if(cmd == "snore")
        on_Snore_clicked();
    else if(cmd == "sleep")
        on_Sleep_clicked();


    else if(cmd == "parrot_sound1")
        on_ParrotSound1_clicked();
    else if(cmd == "parrot_sound2")
        on_ParrotSound2_clicked();
    else if(cmd == "music_bozi")
        on_MusicBozi_clicked();


    else //interactive_commands:
    {
        string appended = cmd + ".wav";
        Send_quote(appended.c_str());
    }

    // setting signals overhead camera based on commands
    if(overhead_cam_check)
    {
        // issue signal to overhead camera
        black_frame_flag = 1;
        overhead_command = cmd;

        // log the given command
        ofstream log_file("command_log.txt", ios::app);
        log_file << cmd << endl;

        //timestamp

        log_file.close();
    }

    // reseting random_counter in order to not interrupt between commnads
    random_counter = 1;

    // call emotion detection based on action
}

MainWindow::MainWindow(string p_id, QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), patient_id(p_id) // the constructor
{
    Ack_timer = new QTimer(this);
    Speech = new QTimer(this);
    Ack_timer->setSingleShot(true);
    connect(Ack_timer,SIGNAL(timeout()),this,SLOT(Ack_time_out()));

    // overhead camera initialization:
    if(overhead_cam_check)
    {
        overhead_cap.open(overhead_camera_number);
        if (!overhead_cap.isOpened())
        {
            qDebug() << "Error opening overhead video stream or file";
        }


        string stream_name = "out.avi";
        if(patient_id != "nil")
            stream_name = patient_id + ".avi";

        int frame_width = overhead_cap.get(CV_CAP_PROP_FRAME_WIDTH);
        int frame_height = overhead_cap.get(CV_CAP_PROP_FRAME_HEIGHT);
        overhead_video.open(stream_name, CV_FOURCC('M', 'J', 'P', 'G'), 6, Size(frame_width, frame_height), true);
        overhead_cam_signal = new QTimer(this);
        connect(overhead_cam_signal, SIGNAL(timeout( )), this, SLOT(overhead_cam_func()));
        overhead_cam_signal->start(140);
    }

    //speech:
    if(speech_check)
    {
        connect(Speech,SIGNAL(timeout()),this,SLOT(Speech_Periodical()));
        Speech->setInterval(1000);
    }

    // curtain:
    if(curtain_check)
    {
        curtain_STOP_signal = new QTimer(this);
        connect(curtain_STOP_signal,SIGNAL(timeout()),this,SLOT(Stop_Curtain()));
        curtain_STOP_signal->setSingleShot(true);
    }

    ui->setupUi(this);
    time_out = false;
    for(int i=1;i<61;i++)
        ui->Com->addItem("COM"+ QString::number(i));
    for(int i=1;i<61;i++)
        ui->Com_E->addItem("COM"+ QString::number(i));

    if(tcp_server_check)
    {
        server = new QTcpServer(this);
        connect(server, SIGNAL(newConnection()), this, SLOT(new_tcp_connection()));
        if(!server->listen(QHostAddress::Any, 9999))
        {
            qDebug() << "Server could not start";
        }
        else
        {
            qDebug() << "Server started!";
        }
    }

    if(plug_and_play_check)
    {
        plug_and_play();
    }

    if(face_rec_check)
    {
        is_user_data_available = cam1.get_current_face_label() + 1;
    }

    if(main_camera_check)
    {
        // init main camera video stream recording
        if(main_camera_record_stream)
        {
            string stream_name = "out_main_cam.avi";
            if(patient_id != "nil")
                stream_name = patient_id +  "_main_cam.avi";
            qDebug() << stream_name.c_str();

            Mat frame = cam1.get_frame();
            main_camera_video.open(stream_name, CV_FOURCC('M', 'J', 'P', 'G'), 5, Size(frame.cols, frame.rows), true);
        }

        connect(&timer, SIGNAL(timeout( )), this, SLOT(main_vision_func()));
        timer.start(500);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Stop_Curtain()
{
    // send stop signal to the corelated port

    Port_Curtain->putChar('S');
    Port_Curtain->putChar('S');

    // Turn LED ON
}

void MainWindow::Close_Curtain()
{
    // send stop signal to the corelated port

    Port_Curtain->putChar('S');
    Port_Curtain->putChar('S');

    Port_Curtain->putChar('D');
    Port_Curtain->putChar('D');

    Port_Curtain->putChar('A'); // turn LED OFF

    curtain_STOP_signal->start(11000);

    // Turn LED ON
}

void MainWindow::overhead_cam_func()
{
    Mat frame;
    overhead_cap >> frame;
    overhead_video.write(frame);
    //imshow("Frame", frame);

    /*  for making black frames*/
    if(black_frame_flag)
    {
        black_frame_flag = false;

        for (int i = 0; i < 10; i++)
        {
            Mat img(frame.rows, frame.cols, CV_8UC3, Scalar(0, 0, 0));
            putText(img, overhead_command, Point(10, 130), FONT_HERSHEY_SCRIPT_SIMPLEX, 2, Scalar::all(255), 3);
            overhead_video.write(img);
            //imshow("Frame", img);
        }
    }
}

void MainWindow::onCurrentChanged(int index)
{
    int currentIndex = ui->tabWidget->currentIndex();
    qDebug("currentChanged(%d), currentIndex() = %d", index, currentIndex);
}
void MainWindow::ManualSend(QString Command_number)
{
    qint64 bytesWritten = port->write(Command_number.toStdString().c_str());
    if(bytesWritten == -1)
        qDebug()<<"Error occured in sending.";
    else
        qDebug()<<"OK.";
}
void MainWindow::on_SendCommand_clicked()
{
    ManualSend(ui->Command->text());
    qDebug()<<port->readAll();
}

void MainWindow::on_Motor1_Dial_valueChanged(int value)
{
    qDebug()<<ui->Motor1_Dial->value();
    if(Motor1_dir % 2 == 0)
    {
        port->putChar(0x01);
        char myChar = (unsigned char)(ui->Motor1_Dial->value());
        port->putChar(myChar);
    }
    else
    {
         port->putChar(0x02);
         char myChar = (unsigned char)(ui->Motor1_Dial->value());
         port->putChar(myChar);
    }
}
void MainWindow::on_Motor_dir_clicked()
{
    ++Motor1_dir;
}
void MainWindow::on_Motor2_dial_valueChanged(int value)
{
    qDebug()<<ui->Motor2_dial->value();
    if(Motor2_dir % 2 == 0)
    {
        port->putChar(0x03);
        char myChar = (unsigned char)(ui->Motor2_dial->value());
        port->putChar(myChar);
    }
    else
    {
        port->putChar(0x04);
        char myChar = (unsigned char)(ui->Motor2_dial->value());
        port->putChar(myChar);
    }
}
void MainWindow::on_Motor2_dir_clicked()
{
        ++Motor2_dir;
}
void MainWindow::on_Motor1_stop_clicked()
{
    port->putChar(0x05);
}
void MainWindow::on_Motor2_stop_clicked()
{
    port->putChar(0x06);
}
void MainWindow::on_eye_open_clicked()
{
    port->putChar(0x0D);
}
void MainWindow::on_eye_close_clicked()
{
    port->putChar(0x0E);
}
void MainWindow::on_eye_blink_clicked()
{
    port->putChar(0x20);
    port->putChar(0x0D);
}
void MainWindow::on_mouth_open_clicked()
{
    port->putChar(0x11);
}
void MainWindow::on_mouth_close_clicked()
{
    port->putChar(0x0C);
}
void MainWindow::on_mouth_open_close_clicked()
{
    port->putChar(0x0B);
}
void MainWindow::on_delay_mouth_valueChanged(const QString &arg1)
{
    port->putChar(0x12);
    char myChar = (unsigned char)(ui->delay_mouth->value());
    port->putChar(myChar);
}
void MainWindow::on_move_right_clicked()
{
    if(!interception)
        start_position();
    port->putChar(0x01);
    port->putChar(0xFF);
    Ack_timer->start(1700);
    wait_it();
    port->putChar(0x05);
    interception = false;
}
void MainWindow::on_move_left_clicked()
{
    if(!interception)
        start_position();
    port->putChar(0x01);
    port->putChar(0xFF);
    Ack_timer->start(800);
    wait_it();
    port->putChar(0x05);
    interception = false;
}
void MainWindow::on_wings_open_clicked()
{
    if(!wings_open)
    {
        if(!interception)
        {
            start_position();
        }
        Ack_timer->start(100);
        wait_it();
        port->putChar(0x02);
        port->putChar(0xFF);
        Ack_timer->start(256);
        wait_it();
        port->putChar(0x05);
        interception = false;
        wings_open = true;
    }
}
void MainWindow::on_wings_close_clicked()
{
    if(wings_open)
    {
        Ack_timer->start(300);
        wait_it();
        port->putChar(0x01);
        port->putChar(0xFF);
        Ack_timer->start(256);
        wait_it();
        port->putChar(0x05);
        interception = true;
        wings_open = false;
    }
}
void MainWindow::start_position()
{
    port->putChar(0x01);
    port->putChar(0xFF);
    interception = true;
    wings_open = false;
    Ack_timer->start(3000);
    wait_it();
}
void MainWindow::on_start_position_clicked()
{
    start_position();
}

void MainWindow::on_StopStartup_clicked()
{
    port_E->close();
    port->close();
    Ack_timer->stop();
    Speech->stop();
    //timer.stop();
}

void MainWindow::on_Send_clicked()
{
    Speech->start();
}

void MainWindow::Ack_time_out()
{
    qDebug()<<"Time outed.";
    time_out = true;
}

// manual sound play back buttons
void MainWindow::on_Salam_clicked()
{
   Send_quote("Salam.wav");
}
void MainWindow::on_Bale_clicked()
{
    Send_quote("Bale.wav");
}
void MainWindow::on_Khoobi_clicked()
{
    Send_quote("Xoobi.wav");
}
void MainWindow::on_Nakheir_clicked()
{
    Send_quote("Naxeyr.wav");
}
void MainWindow::on_Merci_clicked()
{
    Send_quote("Merci.wav");
}
void MainWindow::on_Khodahafez_clicked()
{
    Send_quote("Khodafez.wav");
}
void MainWindow::on_Afarin_clicked()
{
    Send_quote("Afarin.wav");
}
void MainWindow::on_Khande_clicked()
{
   Send_quote("Khande.wav");
}
void MainWindow::on_Love_You_clicked()
{
    Send_quote("DoosetDaram.wav");
}

void MainWindow::on_Connect_Com_clicked()
{
    Com_str = ui->Com->currentText();
    port->setPortName(Com_str);
    bool status = port->open(QIODevice::ReadWrite);
    if (status){
        port->setBaudRate(4800,QSerialPort::AllDirections);
        port->setDataBits(QSerialPort::Data8);
        port->setStopBits(QSerialPort::OneStop);
        port->setFlowControl(QSerialPort::NoFlowControl);
        port->setParity(QSerialPort::NoParity);
    }
    else
        port->close();

    connect(port,SIGNAL(readyRead()),this,SLOT(print_mouth()));
}
void MainWindow::on_Dis_Com_clicked()
{
    port->close();
}
void MainWindow::print_mouth()
{
    char data_new[7];
    qint64 lineLength = port->readLine(data_new,7);
    if(lineLength != -1)
    {
        if(data_new[0] == 'T')
            Send_quote("Afarin.wav");
    }
}
void MainWindow::on_Connect_Com_E_clicked()
{
    Com_str_E = ui->Com_E->currentText();
    port_E->setPortName(Com_str_E);
    bool status = port_E->open(QIODevice::ReadWrite);
    if (status){
        port_E->setBaudRate(9600,QSerialPort::AllDirections);
        port_E->setDataBits(QSerialPort::Data8);
        port_E->setStopBits(QSerialPort::OneStop);
        port_E->setFlowControl(QSerialPort::NoFlowControl);
        port_E->setParity(QSerialPort::NoParity);
    }
    else
    {
        port_E->close();
        return;
    }

    connect(port_E,SIGNAL(readyRead()),this,SLOT(print()));
}
void MainWindow::on_Dis_E_clicked()
{
    port_E->close();
}
void MainWindow::print()
{
    qint64 lineLength = port_E->readLine(data,7);
    if(lineLength != -1)
    {
        Ack = 1;
        qDebug()<<"Recieved: " << data;
        Ack_timer->stop();
    }
}

void MainWindow::on_FD_toggled(bool checked)
{
    Face_Det++;
}
void MainWindow::on_ShapeD_toggled(bool checked)
{
    Shape_Det++;
}
void MainWindow::on_ColorD_toggled(bool checked)
{
    Color_Det++;
}
void MainWindow::on_FR_LBP_toggled(bool checked)
{
    Face_Rec_LBP++;
}

void MainWindow::on_FR_FF_toggled(bool checked)
{
    Face_Rec_FF++;
}

void MainWindow::plug_and_play()
{
    if(overhead_cam_check)
    {
        ofstream log_file("command_log.txt", ios::app);
        log_file << "----------------------------------" << endl;
        log_file.close();
    }
    if(curtain_check)
    {
        //Arduino Port
        qDebug() <<"in Curtain check...";
        Port_Curtain->setPortName("Com4"); // change it
        Port_Curtain->open(QIODevice::ReadWrite);
        Port_Curtain->setBaudRate(9600,QSerialPort::AllDirections);
        Port_Curtain->setDataBits(QSerialPort::Data8);
        Port_Curtain->setStopBits(QSerialPort::OneStop);
        Port_Curtain->setFlowControl(QSerialPort::NoFlowControl);
        Port_Curtain->setParity(QSerialPort::NoParity);
        connect(Port_Curtain,SIGNAL(readyRead()),this,SLOT(print()));

        Port_Curtain->putChar('U');
        Port_Curtain->putChar('U');
        Port_Curtain->putChar('B'); // turn LED ON
        curtain_STOP_signal->start(20000);
    }


    if(speech_check)
    {
        // Easy VR
        port_E->setPortName("Com3"); // change it
        port_E->open(QIODevice::ReadWrite);
        port_E->setBaudRate(9600,QSerialPort::AllDirections);
        port_E->setDataBits(QSerialPort::Data8);
        port_E->setStopBits(QSerialPort::OneStop);
        port_E->setFlowControl(QSerialPort::NoFlowControl);
        port_E->setParity(QSerialPort::NoParity);
        connect(port_E,SIGNAL(readyRead()),this,SLOT(print()));

        Ack_timer->start(1000);
        wait_it();
    }

    if(robot_body_check)
    {
        // ROBOT Body
        port->setPortName("Com8");
        port->open(QIODevice::ReadWrite);
        port->setBaudRate(4800,QSerialPort::AllDirections);
        port->setDataBits(QSerialPort::Data8);
        port->setStopBits(QSerialPort::OneStop);
        port->setFlowControl(QSerialPort::NoFlowControl);
        port->setParity(QSerialPort::NoParity);

        Ack_timer->start(2000);
        wait_it();
        start_position();
        on_eye_open_clicked();
        Ack_timer->start(1600);
        wait_it();
        on_mouth_open_close_clicked();
        Ack_timer->start(1600);
        wait_it();
        on_eye_blink_clicked();
        Ack_timer->start(1600);
        wait_it();
        on_Send_clicked();
        on_eye_open_clicked();
        Ack_timer->start(1600);
        wait_it();
        on_eye_open_clicked();
        Ack_timer->start(1600);
        wait_it();
    }
}

void MainWindow::Send_quote(QString str3)
{
    QSound::play(sounds_address + str3);
    qDebug() <<"in Send_Quote...";
    Ack_timer->start(350);
    wait_it();
    on_mouth_open_clicked();
    Ack_timer->start(1200);
    wait_it();
    on_mouth_close_clicked();
}

void MainWindow::wait_it()
{
    do
    {
      qApp->processEvents();
    }
    while(Ack_timer->isActive()) ;
}

void MainWindow::random_could_not_detect()
{
    int a = rand() % 2 ;
    switch (a)
    {
    case 0:
        Send_quote("could_not_see.wav");
        break;
    case 1:
        Send_quote("could_not_understand.wav");
        break;
    }
}

void MainWindow::handle_command(char& in)
{
    // List of valid commands:
    // A: Salam
    // B: chetori
    // C: in che rangie
    // D: esmet chie
    // E: shekl o rang o begu
    // F: khodahafez
    // G: Harkat kon
    // H: are
    // I: cheshmak bezan
    // J: bekhab --shift
    // K: behand
    // L: yad begir tooti

    int random_case = rand();

    if(in == 'A') // salam
    {
        if(is_user_data_available)
        {
            int prediction = cam1.predict_face("LBP"); //returns -1 if more_than_one_face_detected, -2 if no_face_detected
            if(prediction == 0)
            {
                Send_quote("Salam.wav"); // remove this and okay latter part
                Ack_timer->start(400);
                wait_it();
                Send_quote("username.wav"); // remove this and okay latter part
            }
            else
            Send_quote("Salam.wav"); // remove this and okay latter part
        }
        else
            Send_quote("Salam.wav"); // remove this and okay latter part

        /*random_case = random_case % 3;
        switch(random_case)
        {
            case 0:
                Send_quote("Salam.wav");
                break;
        }*/
    }
    else if(in == 'B') // chetori
    {
        Send_quote("Merci.wav"); // remove this and okay latter part
    }
    else if(in == 'C') // in che rangie
    {
           Send_quote("show_card.wav");
           Ack_timer->start(1000);
           wait_it();
           if(cam1.detect_the_best_color())
           {
               string strrr = cam1.get_detected_color();
               qDebug() << strrr.c_str();
               strrr += ".wav";
               Send_quote(strrr.c_str());
           }
           else
           {
               qDebug() << "could_not_detect_color!!!!!!!!!!!";
               random_could_not_detect();
           }
    }
    else if(in == 'D') // esmet chie
         Send_quote("Tooti.wav");
    else if(in == 'E') // shekl o rango begu
    {
        Send_quote("show_card.wav");
        Ack_timer->start(1000);
        wait_it();
        if(cam1.get_the_best_shape_and_its_color())
        {
            string str_shape = cam1.get_detected_shape();
            qDebug() << str_shape.c_str();
            string str_color = cam1.get_detected_color();
            qDebug() << str_color.c_str();
            string strrr = str_shape + str_color + ".wav";
            Send_quote(strrr.c_str());
        }
        else
        {
            qDebug() << "could_not_detect_color!!!!!!!!!!!";
             random_could_not_detect();
        }
    }
    else if(in == 'F') // khodahafez
    {
        ready_for_face = true;
        Send_quote("Khodafez.wav");
    }
    else if(in == 'G') // harkat kon
    {
       random_case = random_case % 5;
       if(random_case == 0)
       {
           Send_quote("bashe.wav");
           Ack_timer->start(500);
           wait_it();
       }
       else if(random_case == 1)
       {
           Send_quote("chashm.wav");
           Ack_timer->start(500);
           wait_it();
       }

       on_start_position_clicked(); // cases 3 and 4 have no verbal response

       if(random_case == 2) // after moving
       {
           Send_quote("khube.wav");
           Ack_timer->start(500);
           wait_it();
       }
    }
    else if(in == 'H') // are
    {
        // for verfication
        // call a function or change a variable
    }
    else if(in == 'I') // chechmak bezan
    {
        random_case = random_case & 30;

        on_eye_blink_clicked();
        Ack_timer->start(200);
        wait_it();
        on_eye_open_clicked();

        if(random_case < 10) // after moving
        {
            Send_quote("khube.wav");
            Ack_timer->start(500);
            wait_it();
        }
    }
    else if(in == 'J') // bekhab
    {
        random_case = random_case % 3;

        if(!cam1.is_it_dark_outside()) // check the brightness
        {
            if(random_case == 0)
                Send_quote("che_vaghte_khabe_roshane.wav");
            else
                Send_quote("khabam_nemiad_hava_roshane.wav");
        }
        else
        {
            if(random_case == 0)
            {
                Send_quote("bashe.wav");
                Ack_timer->start(500);
                wait_it();
            }
            else if(random_case == 1)
            {
                Send_quote("chashm.wav");
                Ack_timer->start(500);
                wait_it();
            }
            // case 2 is no verbal response

            on_eye_close_clicked();
            Send_quote("Snore.wav");
            Ack_timer->start(3500);
            wait_it();
            Send_quote("Snore.wav");
            Ack_timer->start(3500);
            wait_it();
            on_eye_open_clicked();
        }
    }
    else if(in == 'K') // bekhand
    {
        Send_quote("Khande.wav");
    }
    else if(in == 'L') // yad begir tooti
    {
        Send_quote("hold_still.wav");
        Ack_timer->start(1000);
        wait_it();
        on_Take_new_face_pictures_clicked();
        on_Save_taken_pictures_clicked();
        on_Train_LBP_Model_clicked();
        Send_quote("learning_done.wav");
        Ack_timer->start(1500);
        wait_it();
        Send_quote("what_to_call_u.wav");
        Ack_timer->start(600);
        wait_it();
        // turn LED on to speak
        recorder.toggleRecord();
        Ack_timer->start(2000);
        wait_it();
        recorder.toggleRecord();
        is_user_data_available = true;
    }
}

void MainWindow::handle_random()
{
//    if(random_counter > 20)
//    {
//        on_start_position_clicked();
//        Ack_timer->start(2000);
//        wait_it();
//    }
    if(random_counter > 28)
    {
        int random_case = rand() % 12;  // generate random value

        if(random_case > 7)
            on_start_position_clicked();
        else
            on_eye_blink_clicked();


        ready_for_face = true;  // since the robot is not being used, make it greet forthcoming users again
        random_counter = random_counter / 5;    // somehow decrease the counter


        switch(random_case){
        case 0:
            Send_quote("Parrot_bird sounds_2.wav");
            //Send_quote("BiaBazi.wav");
            //Send_quote("mexican-red-parrot-2.wav");
            //NA
            break;
        case 1:
            Send_quote("BiainBaManBazi.wav");
            //Send_quote("blue-amazon-macaws.wav");
            break;
        case 2:
            Send_quote("BiaPisheMan.wav");
            //Send_quote("Parrot_bird sounds_2.wav");
            break;
        case 3:
            Send_quote("BiaBazi.wav");
            //Send_quote("mexican-red-parrot-2.wav");
            //NA
            break;
        case 4:
            Send_quote("KojaRafti.wav");
            //Send_quote("Bird_Chirp_Mexican_Red.wav");
            break;
        case 5:
            //Send_quote("KojaRaftiBiaPisheMan.wav");
            Send_quote("Parrot_bird sounds_2.wav");
            break;
        case 6:
            Send_quote("Bird_Chirp_Mexican_Red.wav");
            break;
        case 7: // repeat
            Send_quote("Bird_Chirp_Mexican_Red.wav");
            break;
        case 8:
            Send_quote("Parrot_bird sounds_2.wav");
            break;
        case 9: // repeat
            Send_quote("Parrot_bird sounds_2.wav");
            break;
        case 10:
            Send_quote("Parrot_bird sounds_2.wav");
            break;
        case 11: // repeat
            Send_quote("Parrot_bird sounds_2.wav");
            break;
        }
    }
}

void MainWindow::Speech_Periodical()
{
    /*port_E->putChar('x');
    Ack_timer->start(50);
    wait_it();
    */

    /*
    // making the LED connected to Pin 106 blink
    port_E->write("qGB");
    Ack_timer->start(300);
    wait_it();
    port_E->write("qGA");
    Ack_timer->start(30);
    wait_it();
    */

    // start voice recognition
    port_E->write("dB");
    qDebug()<<"Bego:";
    // Beep(528,100);
    Ack_timer->start(5000);

    bool first_check_flag = false;
    if(ready_for_face) // double checking face detection for greeting forthcoming user
        if(cam1.is_there_a_face())
            first_check_flag = true;
    if(first_check_flag)
        if(cam1.is_there_a_face())
        {
            Send_quote("Salam.wav");
            ready_for_face = false;
            random_counter = 1;
        }
    first_check_flag = false;

    wait_it();
    handle_random();

    if(data[0] == 't')
    {
        random_counter += 3;
        qDebug()<<"EasyVR timeout.";
    }
    else if(data[0] == 'e')
    {
        random_counter ++;
        qDebug()<<"EasyVR Error.";
    }
    else if(data[0] == 'r')
    {
        random_counter = 1;
        port_E->putChar(' ');
        Ack_timer->stop();
        Ack_timer->start(100);
        wait_it();
        handle_command(data[0]);
    }
}

void MainWindow::on_manual_shape_color_int_clicked() // // manual shape + color interrupt
{
    random_counter = 1;
    Send_quote("show_card.wav");
    Ack_timer->start(1000);
    wait_it();
    if(cam1.get_the_best_shape_and_its_color())
    {
        string str_shape = cam1.get_detected_shape();
        qDebug() << str_shape.c_str();
        string str_color = cam1.get_detected_color();
        qDebug() << str_color.c_str();
        string strrr = str_shape + str_color + ".wav";
        Send_quote(strrr.c_str());
    }
    else
    {
        qDebug() << "could_not_detect_color!!!!!!!!!!!";
        random_could_not_detect();
    }
}

void MainWindow::on_Manual_Color_clicked() // manual color interrupt
{
    random_counter = 1;
    Send_quote("show_card.wav");
    Ack_timer->start(1000);
    wait_it();
    if(cam1.detect_the_best_color())
    {
        string strrr = cam1.get_detected_color();
        qDebug() << strrr.c_str();
        strrr += ".wav";
        Send_quote(strrr.c_str());
    }
    else
    {
        qDebug() << "could_not_detect_color!!!!!!!!!!!";
        random_could_not_detect();
    }
}

void MainWindow::on_Manual_Shape_clicked()
{
    Send_quote("show_card.wav");
    Ack_timer->start(1000);
    wait_it();
    if(cam1.detect_the_best_shape())
    {
        string strrr = cam1.get_detected_shape();
        qDebug() << strrr.c_str();
        strrr += ".wav";
        Send_quote(strrr.c_str());
    }
    else
        qDebug() << "could_not_detect!!!!!!!!!!!";
}

void MainWindow::on_Take_new_face_pictures_clicked()
{
    for (int i = 0; i < 4; i++)
    {
        string answer = cam1.take_candidate_images_for_learning(); // call this function 4 times to gather the reqiured training data
        // wait till you get 4 "success" -- possible errors: "more_than_one_face_detected" or "no_face_detected"
        qDebug() << answer.c_str();

        if(i == 1)
        {
            Send_quote("tilt_head.wav");
            Ack_timer->start(1400);
            wait_it();
        }

        if (answer != "success")
        {
            i--;
            Send_quote("cant_see_come_forth.wav");

            take_face_pics_timer.setSingleShot(true);
            take_face_pics_timer.start(1200);
            while(take_face_pics_timer.remainingTime());
            qDebug() << "starting again";
        }
    }
    //Send_quote("Bale.wav");
    qDebug() << "___gathered_everything___";
}

void MainWindow::on_Save_taken_pictures_clicked()
{
    cam1.save_trained_faces_of_one_person();
    cout << " saved successfuly" << endl;
}

void MainWindow::on_Train_FF_Model_clicked()
{
    //cam1.train_FF_model();
    //recorder.toggleRecord();
    Send_quote("username.wav");
}

void MainWindow::on_Train_LBP_Model_clicked()
{
    cam1.train_LBP_model();
    cout << " Model trained successfuly" << endl;
    // say : yad gereftam
}

void MainWindow::on_Sleep_clicked()
{
    on_eye_close_clicked();
    Send_quote("Snore.wav");
    on_eye_close_clicked();
    Ack_timer->start(5000);
    wait_it();
    on_eye_blink_clicked();
    on_eye_open_clicked();
}

void MainWindow::on_Snore_clicked()
{
    Send_quote("Snore.wav");
}

void MainWindow::on_Bia_Bazi_clicked()
{
    Send_quote("BiaBazi1.wav");
}

void MainWindow::on_BiainBaManBazi_clicked()
{
    Send_quote("BiainBaManBazi.wav");
}

void MainWindow::on_BiaPisheMan_clicked()
{
    Send_quote("BiaPisheMan.wav");
}

void MainWindow::on_KojaRafti_clicked()
{
    Send_quote("KojaRafti.wav");
}

void MainWindow::on_KojaRaftiBia_clicked()
{
    Send_quote("KojaRaftiBiaPisheMan.wav");
}

void MainWindow::on_BebinGhafasamo_clicked()
{
    Send_quote("BebinGhafasKhoshgeliDaram.wav");
}

void MainWindow::on_MikhamBekhabamKhasteShodam_clicked()
{
    Send_quote("MikhamBekhabam.wav");
}

void MainWindow::on_ParrotSound1_clicked()
{
    Send_quote("Bird_Chirp_Mexican_Red.wav");
}

void MainWindow::on_ParrotSound2_clicked()
{
    Send_quote("Parrot_bird sounds_2.wav");
}

void MainWindow::on_MusicBozi_clicked()
{
    Send_quote("music-bozi.wav");
}

void MainWindow::on_Close_The_Curtain_clicked()
{
    Close_Curtain();
}
