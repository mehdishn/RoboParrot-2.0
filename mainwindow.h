#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Qtcore> //the timer to refresh th widget
#include <QtSerialPort/QSerialPort>
#include <QSound>
#include <QDebug>
#include <QThread>
#include <QString>
#include <QtNetwork>
#include <QTcpServer>
#include <QTcpSocket>
#include <string>
#include <QProcess>
#include <QFile>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtMultimedia/QMediaPlayer>
#include "./CV_Sources/camera.h"
#include "utility_functions.h"
#include "audiorecorder.h"

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
        QTimer *Ack_timer, *Speech, timer, take_face_pics_timer, tcp_server_timer, *curtain_STOP_signal, *overhead_cam_signal;
        QString Com_str, Com_str_E;
        QSerialPort *port = new QSerialPort;
        QSerialPort *port_E = new QSerialPort;
        QSerialPort *Port_Curtain = new QSerialPort;
        QString Command;
        bool interception = 0, wings_open = 0, time_out = 0, black_frame_flag = 0;
        char data[7];
        int Motor1_dir = 0, Motor2_dir = 0, Ack = 0;
        QProcess *myProcess= new QProcess;

        void ManualSend(QString Command_number);
        void Send_quote(QString sr3);

        explicit MainWindow(string p_id = "nil", QWidget *parent = 0);
        ~MainWindow();

        bool ready_for_face = true;
        int random_counter = 1;
        AudioRecorder recorder;
        bool is_user_data_available = false;
        string patient_id = "nil";

        VideoCapture overhead_cap;
        VideoWriter overhead_video, main_camera_video;
        string overhead_command;

        QTcpServer *server;
        QTcpSocket *socket;

private slots: // related to TCP server
    void new_tcp_connection();
    void read_data_from_socket();
    void handle_tcp_command(string cmd);

private slots:
    void Stop_Curtain();
    void Close_Curtain();
    void overhead_cam_func();

    void handle_random();

    void wait_it();

    void handle_command(char& );

    void random_could_not_detect();

    void Speech_Periodical();

    void Ack_time_out();

    void print();

    void main_vision_func();

    void read_from_tcp_server(QByteArray data);

    void onCurrentChanged(int index);
    void print_mouth();
    void on_Send_clicked();
    void on_SendCommand_clicked();

    void on_Motor1_Dial_valueChanged(int value);
    void on_Motor_dir_clicked();
    void on_Motor2_dial_valueChanged(int value);
    void on_Motor2_dir_clicked();
    void on_Motor1_stop_clicked();
    void on_Motor2_stop_clicked();
    void on_eye_open_clicked();
    void on_eye_close_clicked();
    void on_eye_blink_clicked();
    void on_mouth_open_clicked();
    void on_mouth_close_clicked();
    void on_mouth_open_close_clicked();
    void on_delay_mouth_valueChanged(const QString &arg1);
    void on_move_right_clicked();
    void on_move_left_clicked();
    void on_wings_open_clicked();
    void on_wings_close_clicked();
    void on_start_position_clicked();
    void start_position();

    void on_Salam_clicked();
    void on_Bale_clicked();
    void on_Khoobi_clicked();
    void on_Nakheir_clicked();
    void on_Merci_clicked();
    void on_Khodahafez_clicked();
    void on_Afarin_clicked();
    void on_Khande_clicked();
    void on_Love_You_clicked();

    void on_Connect_Com_clicked();
    void on_Connect_Com_E_clicked();
    void on_Dis_E_clicked();
    void on_Dis_Com_clicked();

    void on_FD_toggled(bool checked);
    void on_ShapeD_toggled(bool checked);
    void on_ColorD_toggled(bool checked);
    void on_FR_LBP_toggled(bool checked);
    void on_FR_FF_toggled(bool checked);

    void plug_and_play();
    void on_StopStartup_clicked();

    void on_manual_shape_color_int_clicked();
    void on_Manual_Color_clicked();
    void on_Manual_Shape_clicked();

    void on_Take_new_face_pictures_clicked();

    void on_Save_taken_pictures_clicked();

    void on_Train_FF_Model_clicked();

    void on_Train_LBP_Model_clicked();

    void on_Sleep_clicked();

    void on_Snore_clicked();

    void on_Bia_Bazi_clicked();

    void on_BiainBaManBazi_clicked();

    void on_BiaPisheMan_clicked();

    void on_KojaRafti_clicked();

    void on_KojaRaftiBia_clicked();

    void on_BebinGhafasamo_clicked();

    void on_MikhamBekhabamKhasteShodam_clicked();

    void on_ParrotSound1_clicked();

    void on_ParrotSound2_clicked();

    void on_MusicBozi_clicked();

    void on_Close_The_Curtain_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // end of header guard
