#!/bin/env python3
from copy import deepcopy
from enum import IntEnum
from math import floor
from pprint import pp
from queue import LifoQueue, Queue
from random import random
import datetime
import os
import pickle
import select
import serial
import socket
import struct
import sys
import time

from threading import Thread

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

try:
    client_socket.bind(('0.0.0.0', 8027))
except socket.error as message:
    print(message)
    sys.exit(8)

client_socket.listen(9)
conn, address = client_socket.accept()
class SquareState(IntEnum):
    UNKNOWN = 0
    EMPTY = 1
    WALL = 2
    ROBOT = 3
    START = 4

sensorData = [42, 42, 42, 42]
mapData: list[list[SquareState]] = [
    [SquareState.UNKNOWN for _ in range(75)]
    for _ in range(75)
]
pickled_data = pickle.dumps({ 'sensors': sensorData, 'mapd': mapData})

while True:
    print("??")
    time.sleep(1.0)
    length: int = len(pickled_data)
    conn.sendall(struct.pack('>I', length) + pickled_data)
