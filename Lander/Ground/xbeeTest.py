#! /usr/bin/python3

import xbee
import serial
import time


ser = serial.Serial('/dev/ttyUSB0', 9600)


# data is returned as a list of dictionaries
def handle_message(data):
	"Called whenever the XBee receives data. Async is lit"
	print(data)


# Use an XBee 802.15.4 device
xbeeInst = xbee.XBee(ser, callback=handle_message)



while True:
	try:
		echo = input()
		xbeeInst.send(echo, frame='A', command='MY', parameter=None)
		time.sleep(0.01)
	except KeyboardInterrupt:
		break


xbeeInst.halt()
ser.close()