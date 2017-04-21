#!/usr/bin/env python3
from json import loads
import numpy as np
import serial
import matplotlib.pyplot as plt
from threading import Thread
from queue import Queue

que = Queue()

def loader():
    with serial.Serial("/dev/ttyACM0", 9600) as ser:
        while True:
            line = ser.readline()
            data = loads(line)
            que.put(data)

thr = Thread(target=loader)
thr.start()

titles = ["Battery", "Lat", "Lon", "Lux", "Pressure", "Temperature", "Humidity", "Ax", "Ay", "Az", "Gx", "Gy", "Gz", "Mx", "My", "Mz", "Qw", "Qx", "Qy", "Qz"]
times, lux, pres, temp, humd, bat, lat, lon = [], [], [], [], [], [], [], []
acc, gyro, mag, quat = np.zeros(3), np.zeros(3), np.zeros(3), np.zeros(4)

plt.ion()

while True:
    data = que.get()
    if isinstance(data, dict):
        data = [data]
    if not isinstance(data, list):
        print(data)
        continue
    for json in data:
        if not isinstance(json, dict):
            print(json)
            continue
        times = np.append(times, json["Time"])
        bat = np.append(bat, json["Battery"])
        lat = np.append(lat, json["GPS"][0])
        lon = np.append(lon, json["GPS"][1])
        lux = np.append(lux, json["TSL"]["Lux"])
        pres = np.append(pres, json["BME"]["Pressure"])
        temp = np.append(pres, json["BME"]["Temperature"])
        humd = np.append(pres, json["BME"]["Humidity"])
        acc = np.vstack(acc, json["MPU"]["Accelerometer"])
        gyro = np.vstack(gyro, json["MPU"]["Gyrometer"])
        mag = np.vstack(mag, json["MPU"]["Magnetometer"])
        quat = np.vstack(quat, json["MPU"]["Orientation"])
    datas = [bat, lat, lon, lux, pres, temp, humd, acc[1:, 0], acc[1:, 1], acc[1:, 2], gyro[1:, 0], gyro[1:, 1], gyro[1:, 2], mag[1:, 0], mag[1:, 1], mag[1:, 2], quat[1:, 0], quat[1:, 1], quat[1:, 2], quat[1:, 3]]
    for i, d, t in zip(range(20), datas, titles):
        plt.subplot(5, 4, i)
        plt.title(t)
        plt.plot(times, d)
    plt.pause(0.05)
