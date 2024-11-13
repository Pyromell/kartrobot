import tkinter
import socket
import time
import pickle
import select
from enum import Enum
from struct import unpack

from tkinter import *

pi_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
pi_socket.connect(("10.42.0.1", 8027))
pi_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
pi_socket.setblocking(0)


class SquareState(Enum):
    UNKNOWN = 0
    EMPTY = 1
    WALL = 2


class Interface():
    buffer = b''

    def recieve(self):
        ready = select.select([pi_socket], [], [], 0.2)
        if ready[0]:
            self.buffer += pi_socket.recv(11688)
            print(self.buffer)
            try:
                messageData = pickle.loads(self.buffer)
                self.sensorTextBox.insert(
                    tkinter.END,
                    chars=str(messageData['sensors']),
                )
            except pickle.UnpicklingError:
                print("Incomplete message...")
                self.tk.after(1000, self.recieve)
                return
            self.buffer = self.buffer[11688:]
        else:
            print("No data...")
        self.tk.after(1000, self.recieve)

    def __init__(self):
        self.tk = Tk()
        self.buttonStartStop = Button(
            self.tk,
            text="Send Start / Stop",
            command=sendStartStop,
        )
        self.buttonForward = Button(
            self.tk,
            text="Send Forward",
            command=sendForward,
        )
        self.buttonBack = Button(
            self.tk,
            text="Send Manual Toggle",
            command=sendBack,
        )
        self.buttonRight = Button(
            self.tk,
            text="Send Right",
            command=sendRight,
        )
        self.buttonLeft = Button(
            self.tk,
            text="Send Left",
            command=sendLeft,
        )
        self.buttonManualToggle = Button(
            self.tk,
            text="Send Manual Toggle",
            command=sendManualToggle,
        )
        self.buttonStartStop.pack()
        self.buttonForward.pack()
        self.buttonBack.pack()
        self.buttonRight.pack()
        self.buttonLeft.pack()
        self.buttonManualToggle.pack()
        self.sensorTextBox = tkinter.Text(
            self.tk,
            height=12,
            width=40,
        )
        self.sensorTextBox.pack(expand=True)
        self.recieve()
        self.tk.mainloop()


def sendStartStop():
    print("Sending Start / Stop")
    pi_socket.send((0).to_bytes(3, 'big'))


def sendForward():
    print("Sending Forward")
    pi_socket.send((1).to_bytes(3, 'big'))


def sendBack():
    print("Sending Back")
    pi_socket.send((2).to_bytes(3, 'big'))


def sendRight():
    print("Sending Right")
    pi_socket.send((3).to_bytes(3, 'big'))


def sendLeft():
    print("Sending Left")
    pi_socket.send((4).to_bytes(3, 'big'))


def sendManualToggle():
    print("Sending Manual Toggle")
    pi_socket.send((5).to_bytes(3, 'big'))


interFace = Interface()
