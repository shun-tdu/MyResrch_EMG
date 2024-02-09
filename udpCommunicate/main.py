import sys
import socket
import threading
from random import randint
import time
import numpy as np
import pyqtgraph as pg
from PyQt5 import QtWidgets, QtCore
from queue import Queue

# グローバル変数
recv_time = []  # 時間データ
recv_sensor_1 = []  # センサーデータ
data_queue = Queue()  # スレッド間通信のキュー


class UdpRecv:  # UPD通信クラス
    def __init__(self):
        # サーバーの情報
        self.listen_ip = '127.0.0.1'  # IPアドレス
        self.listen_port = 12345  # ポート番号
        self.udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.udp_socket.bind((self.listen_ip, self.listen_port))

    # 受信関数
    def recv(self):
        print(f"UDPソケットを {self.listen_ip}:{self.listen_port} でリッスン中...")
        while True:
            try:
                data, addr = self.udp_socket.recvfrom(8192)
                cleaned_data = data.decode().rstrip('\x00')  # null文字を除去
                data_list = [float(x) for x in cleaned_data.split(',')]  # float型に変換
                data_queue.put((data_list[0], data_list[1]))
                # print(data_list[0], data_list[1],recv_data_count)
                self.udp_socket.setblocking(False)
            except BlockingIOError:
                pass  # 例外処理
            finally:
                self.udp_socket.setblocking(True)  # 例外処理


class PrintData:
    def __init__(self):
        self.print_count = 0

    def debug(self):
        while True:
            print("debug")

    def print_data(self):
        global recv_time, recv_sensor_1
        while True:
            if not data_queue.empty():
                # このブロックはデータが受信されると実行されるから実行回数とprint_countは一致
                t, data_1 = data_queue.get()
                recv_time.append(t)
                recv_sensor_1.append(data_1)
                print(recv_time[self.print_count], recv_sensor_1[self.print_count], self.print_count)
                self.print_count += 1


class PlotWindow(QtWidgets.QMainWindow):  # リアルタイムプロットクラス
    def __init__(self):
        super().__init__()
        self.smp = 1000
        self.displayTime = 3
        self.plotting_data_num = self.smp * self.displayTime  # 1度にプロットするデータ数
        self.print_count = 0

        # グラフ描画処理
        self.plot_graph = pg.PlotWidget()  # ウィジェットの作成
        self.setCentralWidget(self.plot_graph)
        self.plot_graph.setBackground("w")  # 背景の設定
        pen = pg.mkPen(color=(0, 100, 255))  # ペンの設定
        self.plot_graph.setTitle("Val_1 vs Time", color="b", size="20pt")  # タイトルの設定
        styles = {"color": "blue", "font-size": "18px"}  # フォントスタイルの設定
        self.plot_graph.setLabel("left", "Sensor_1 [V]", **styles)  # y軸ラベルの設定
        self.plot_graph.setLabel("bottom", "Time [s]", **styles)  # x軸ラベルの設定
        self.plot_graph.addLegend()  # 凡例の設定
        self.plot_graph.showGrid(x=True, y=True)  # グリッドの設定
        self.plot_graph.setYRange(-1, 1)  # y軸レンジ設定
        self.time = list(0 for _ in range(self.plotting_data_num))  # x軸の設定
        self.temperature = [0 for _ in range(self.plotting_data_num)]  # y軸の設定
        # Get a line reference
        self.line = self.plot_graph.plot(
            self.time,
            self.temperature,
            name="Sensor_1_Val",
            pen=pen
        )

    # def update_plot(self):
    #     self.time = self.time[1:]  # timeの最初の要素を削除
    #     self.time.append(self.time[-1] + 1)  # timeの最後に最後の要素+1を代入
    #     self.temperature = self.temperature[1:]  # temperatureの最初の要素を削除
    #     self.temperature.append(randint(20, 40))  # temperatureの最後の要素にランダムなデータを追加
    #     self.line.setData(self.time, self.temperature)

    def add_data(self):
        global recv_time, recv_sensor_1
        while True:
            if not data_queue.empty():
                # データを受信する毎に時間とデータをグローバル変数に格納
                # 配列の更新はおｋ
                t, data_1 = data_queue.get()
                recv_time.append(t)  # 時間をグローバル変数に格納
                recv_sensor_1.append(data_1)  # データをグローバル変数に格納
                self.time = self.time[1:]  # timeの最初の要素を削除
                self.time.append(t)
                self.temperature = self.temperature[1:]
                self.temperature.append(data_1)
                self.line.setData(self.time, self.temperature)
                # print(self.time,self.temperature)
                # print(recv_time[self.print_count], recv_sensor_1[self.print_count], self.print_count)
                self.print_count += 1


app = QtWidgets.QApplication([])

# クラスのインスタンス化
udp_recv = UdpRecv()
plot_window = PlotWindow()
# スレッドの初期化
recv_thread = threading.Thread(target=udp_recv.recv)
plot_thread = threading.Thread(target=plot_window.add_data)

plot_window.show()

# スレッドの開始
recv_thread.start()
plot_thread.start()

app.exec()

# スレッドの終了待ち
recv_thread.join()
plot_thread.join()
