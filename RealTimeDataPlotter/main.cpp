#define _USE_MATH_DEFINES

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include <thread>
#include <chrono>
#include <mutex>
#include "crlserial.hpp"

#define BAUD 115200
#define  DATA_NUM 2
#define PORT TEXT("COM3")

#pragma comment(lib, "ws2_32.lib")

/*----グローバル変数の宣言----*/
std::vector<double> t;
std::vector<double> val;
bool stop_flag = false; //プログラム全体の終了フラグ
int recvd_cnt = 0;

void SerialCom();


int main() {
    /*--------表示処理関連の変数--------*/
    std::vector<double> x;
    std::vector<double> y;
    int data_cnt = 0;
    /*------シリアル通信関連の変数------*/
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



    //ここの処理を現在のデータ数を監視してそれを超えない値を読み出す処理n+1のデータがストックされてるときnの値を読み出す
    /*----1ms毎に実行----*/
    auto start_time = std::chrono::high_resolution_clock::now();
    auto interval = std::chrono::milliseconds(1);
    int write_cnt = 0;//出力データのインデックス
    while (!stop_flag) {
        if (write_cnt > 1000000) stop_flag = true;
        auto current_time = std::chrono::steady_clock::now();
        if (current_time - start_time >= interval) {
            start_time = current_time;
            if (recvd_cnt > write_cnt){
                std::stringstream ss;
                ss << std::fixed << std::setprecision(3) << t[write_cnt] << "," << val[write_cnt];
                std::string sendData;
                sendData = ss.str();
                sendto(sock, sendData.c_str(), sizeof(data), 0, (struct sockaddr *) &addr, sizeof(addr));
                std::cout << sendData.c_str()<< std::endl;
                write_cnt += 1;//データを読み込むごとに書き出した最新データのインデックスを更新
            }
        }
    }
    /*------------------*/

    serialComThread.join();

    return 0;
}

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


