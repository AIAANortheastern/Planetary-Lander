import asyncio
import aiohttp, aiohttp.web
import serial
from xbee import XBee
import json


class Receiver(aiohttp.web.Application):
    def __init__(self):
        super().__init__()
        self.router.add_get('/', self.handler)
        self.serial_port = serial.Serial('/dev/ttyUSB0', 9600)
        self.xbee = XBee(self.serial_port, callback=self.recv)
        self.buffer = []

    async def handler(self, request):
        ws = aiohttp.web.WebSocketResponse()
        await ws.prepare(request)
        self.new_socket(ws)
        async for msg in ws:
            if msg.type == aiohttp.WSMsgType.ERROR:
                print('ws connection closed with exception %s' % ws.exception())
            elif msg.type == aiohttp.WSMsgType.TEXT:
                if msg.data == 'close':
                    await ws.close()
                else:
                    print(msg.data)
                    ws.send_str(msg.data + '/answer')
        return ws

    def recv(self, packet):
        packet = self.parse(packet)
        for ws in self['websockets']:
            ws.send_json(packet)
        self.buffer.append(packet)
        print(packet)

    def parse(self, raw_packet):
        return json.loads(raw_packet)

    def new_socket(self, ws):
        for packet in self.buffer:
            ws.send_json(packet)

    def end(self):
        self.xbee.halt()
        self.serial_port.close()

if __name__ == "__main__":
    aiohttp.web.run_app(Receiver())
