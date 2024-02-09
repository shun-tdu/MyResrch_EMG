#ifndef __CRL_SERIAL_COM_20150311__
#define __CRL_SERIAL_COM_20150311__


#include <stdlib.h>
#include <tchar.h>
#include <process.h>
#include <direct.h>
#include <windows.h>

class crlSerial {

    LPCTSTR m_com;
    int m_baud;
    HANDLE m_handle;
    bool active_flg;

public:
    crlSerial(LPCTSTR com_name, int baud_rate=115200){
        m_handle = init(com_name, baud_rate);
        if (m_handle == INVALID_HANDLE_VALUE ) {
            std::cerr << "#error: open COM port for seral com. failed!  \""<< com_name << "\" (HANDLE: " << m_handle  << ")";
            std::cerr << " @crlSerial::init()"<< std::endl;
        } else {
            std::cout << "#debug: open COM port for seral com. succeeded!  \""<< com_name << "\"  (HANDLE: " << m_handle << ")";
            std::cout << " @crlSerial::init()"<< std::endl;
        }
    };
    //	Check for serial set up
    bool check(void) {
        bool e = true;
        if(active_flg != true) {
            //std::cerr << "#error: active_flg is false! @crlSerial::check()" << std::endl;
            e = false;
        }
        if (m_handle == INVALID_HANDLE_VALUE ) {
            //std::cerr << "#error: m_handle is incorrect!";
            //std::cerr << " @crlSerial::check()"<< std::endl;
            e = false;
        }
        return e;
    }


    // double型 N個のシリアルデータ読み込み
    // ※", "で区切ること
    int read_sci(int N, double *dat) {
        int n = 0; // read num
        char sBuf[1];
        char sDatBuf[64];
        int i=0;
        unsigned long nn;

        while(1) {

            if(ReadFile(m_handle, sBuf, 1, &nn, 0 ) != 0) { // シリアルポートに対する読み込み
                if ( nn==1 ) {
                    if ( sBuf[0]==',' || sBuf[0]==10 || sBuf[0]==13) {
                        if(n >= N) {
                            break;
                        }
                        sscanf(sDatBuf, "%lf", &(dat[n]));
                        //std::cout << "#debug: dat["<<n<<"]: " << dat[n] << std::endl;
                        i=0;
                        n++;
                    } else {
                        sDatBuf[i] = sBuf[0];
                        //std::cout << "#sDat["<<i<<"]: " << sDatBuf[i] << std::endl;
                        i++;
                    }
                    if ( sBuf[0]==10 || sBuf[0]==13 ) { // '\r'や'\n'を受信すると文字列を閉じる
                        break;
                    }
                }
            } else {
                break;
            }
        }
        return n;
    }

    // double型 8個のシリアルデータ読み込み
    // ※", "で区切ること
    int read_sci8(double *dat) {
        int n = -1; // read num
        char sBuf[1];
        char str[128];
        int i=0;
        unsigned long nn;
        while(1) {
            if(ReadFile(m_handle, sBuf, 1, &nn, 0 ) != 0) { // シリアルポートに対する読み込み
                n++;
                if ( nn==1 ) {
                    if ( sBuf[0]==10 || sBuf[0]==13 ) { // '\r'や'\n'を受信すると文字列を閉じる
                        if ( i!=0 ) {
                            str[i] = '\0';
                            i=0;
                            //printf("[%d] %s\n",n, str);
                            sscanf(str, "%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf\r\n",
                                   &dat[0], &dat[1], &dat[2], &dat[3], &dat[4], &dat[5], &dat[6], &dat[7]);
                            break;
                        }
                    } else {
                        str[i] = sBuf[0];
                        i++;
                    }
                }
            } else {
                break;
            }
        }
        return n;
    }

private:
    //	シリアルポートの初期化
    HANDLE init(LPCTSTR com_name, int baud_rate) {

        DCB dcb;
        HANDLE h;
        m_com = com_name;
        m_baud = baud_rate;
        active_flg = false;
        // クリエイトしたシリアルポート（ファイル）のファイルハンドラを返す
        h = CreateFile(com_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h == INVALID_HANDLE_VALUE ) {
            printf("#error: invarid_handle_value: open port %s\n", (char *)com_name);
            return (HANDLE)false;
        }

        /* ----------------------------------------------
            シリアルポートの状態操作
        ---------------------------------------------- */
        GetCommState(h, &dcb ); // シリアルポートの状態を取得
        dcb.BaudRate = baud_rate;
        dcb.ByteSize = 8;
        dcb.Parity   = NOPARITY;
        dcb.StopBits = ONESTOPBIT;
        SetCommState(h, &dcb ); // シリアルポートの状態を設定
        active_flg = true;
        return h;
    }
};

#endif