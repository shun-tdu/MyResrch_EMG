//
// Created by tahar on 2024/01/29.
//

#ifndef REALTIMEDATAPLOTTER_UDP_CONNECTION_H
#define REALTIMEDATAPLOTTER_UDP_CONNECTION_H

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")

//class udp_connection{
//public:
//    UDPSender(){
//        if(WSAStartup(MAKEWORD(2,2),&wsaData) != 0){
//            std::cerr << "WinSock2の初期化に失敗しました"<<std::endl;
//            std::exit(EXIT_FAILURE);
//        }
//        udpSocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
//        if(udpSocket == INVALID_SOCKET){
//            std::cerr << "ソケットの作成に失敗しました" << std::endl;
//            WSACleanup();
//            std::exit(EXIT_FAILURE);
//        }
//    }
//    ~UDPSender(){
//        closesocket(udpSocket);
//        WSACkeanup();
//    }
//    void sendMassage(const char* ipAddress, int port, const char* message){
//        sockaddr_in serverAddress;
//        std::
//
//    }
//private:
//    SOCKET udpSocket;
//    WSADATA wsaData;
//};



#endif //REALTIMEDATAPLOTTER_UDP_CONNECTION_H
