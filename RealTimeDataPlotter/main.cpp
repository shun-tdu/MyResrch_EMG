#define _USE_MATH_DEFINES

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include <thread>
#include <chrono>
#include <mutex>
#include "crlserial.hpp"
#include "unsupported/Eigen/FFT"
#include "Eigen/Eigen"

#define BAUD 115200
#define  DATA_NUM 2
#define PORT TEXT("COM3")

#pragma comment(lib, "ws2_32.lib")

/*----グローバル変数の宣言----*/
std::vector<double> t;  //Eigenのベクトルで宣言が必要 ← pushbackが使えない
std::vector<double> val;//Eigenのベクトルで宣言が必要
std::vector<double> *t_ptr = &t;
std::vector<double> *val_ptr = &val;
//    Eigen::VectorXd y_ifft(n), amp(n), f(n);//EigenClassベクトルの作成
//    Eigen::VectorXcd y_fft(n);//EigenClass複素ベクトルの作成

bool stop_flag = false; //プログラム全体の終了フラグ
int recvd_cnt = 0;

/*----関数の前宣言----*/
void SerialCom();//シリアル通信関数

int main() {
    /*------UDP通信関連の変数------*/
    double data[DATA_NUM];
    double SmpFreq = 1000;
    double SmpTime = 1 / SmpFreq;

    int sock;
    struct sockaddr_in addr;
    WSAData wsaData;
    struct timeval tv;

    WSACleanup();
    WSAStartup(MAKEWORD(2, 0), &wsaData);
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");


    /*----スレッドの開始----*/
    std::thread serialComThread(SerialCom);

    //0.5秒ごとに1024個のデータをfft(50ms) → 512個の周波数領域データを送信 →

    auto start_time = std::chrono::high_resolution_clock::now();
    auto interval = std::chrono::milliseconds(1);
    int write_cnt = 0;  //出力データのインデックス

    /*----1ms毎に実行----*/
    while (!stop_flag) {
        if (write_cnt > 1000000) stop_flag = true;
        auto current_time = std::chrono::steady_clock::now();
        if (current_time - start_time >= interval) {
            start_time = current_time;
            //読み出しが未定義メモリを超えない領域
            if (recvd_cnt > write_cnt) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(3) << t[write_cnt] << "," << val[write_cnt];
                std::string sendData;
                sendData = ss.str();

                //sizeof(data)をsendData.size()に変更した
                sendto(sock, sendData.c_str(), sendData.size(), 0, (struct sockaddr *) &addr, sizeof(addr));
                std::cout << sendData.c_str() << std::endl;
                write_cnt += 1;
            }
        }
    }

    /*----スレッドの終了待ち----*/
    serialComThread.join();

    return 0;
}


//int main() {
//    //Eigenを用いたFFT
//    double dt = 0.001;           //サンプリング周期
//    size_t n = 1024;             //データ数
//    double df = 1 / (dt * n);   //周波数周期
//    double f0 = 100, f1 = 300;    //テスト用周波数設定
//    bool stop_flag = false;
//    double writeTime = 0.1;
//    double currentTime = 0.0;
//
//    Eigen::VectorXd t(n), y(n), y_ifft(n), amp(n), f(n);//EigenClassベクトルの作成
//    Eigen::VectorXcd y_fft(n);//EigenClass複素ベクトルの作成
//
//    //仮データ作成
//    for (int i = 0; i < n; i++) {
//        f(i) = df * i;
//        t(i) = dt * i;
//        y(i) = 2 * std::sin(2 * M_PI * f0 * t(i)) + std::sin(2 * M_PI * f1 * t(i));
//    }
//
//    //FFT
//    Eigen::FFT<double> fft;
//    fft.fwd(y_fft, y);
//    amp = (y_fft / (n / 2)).cwiseAbs();
//
//    //CSV書き込み処理
//    std::ofstream dataFile("../../data.csv");
//    for (int i = 0; i < n / 2; i++) {
//        //横軸周波数、縦軸振幅
//        dataFile << f[i] << "," << amp[i] << std::endl;
//        std::cout << f[i] << "," << amp[i] << std::endl;
//    }
//}


//シリアル通信関数
void SerialCom() {
    crlSerial sc(PORT, BAUD);
    double recv_data[DATA_NUM];
    //起動チェック
    if (sc.check() == true) {
        printf("#debug: open serial port [%s]: succeeded!\n", PORT);
    } else {
        printf("#error: open serial failed!\n");
        printf("#error: port: %s\n", PORT);
        stop_flag = true;
    }
    //シリアル通信で受信したデータをxとvalに格納し続ける(1msごとに更新)
    while (!stop_flag) {
        if (sc.check() == true) {
            sc.read_sci(DATA_NUM, recv_data);
            t.push_back(recv_data[0]);
            val.push_back(recv_data[1]);
            recvd_cnt++;//データを読み込むごとに読み込んだ最新データのインデックスを更新
        }
    }
}

//明示的に一部のtとvalを切り取ってFFT
void RealTimeFFT(std::vector<double> *t_ptr, std::vector<double> *val_ptr) {

}


