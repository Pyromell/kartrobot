import tkinter
import socket
import time
import pickle

from tkinter import *

pi_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
pi_socket.connect(("10.42.0.1", 8027))


class Interface(Frame):
    def __init__(self):
        self.root = Tk()
        self.buttonStartStop = Button(
            self.root,
            text="Send Start / Stop",
            command=sendStartStop,
        )
        self.buttonForward = Button(
            self.root,
            text="Send Forward",
            command=sendForward,
        )
        self.buttonBack = Button(
            self.root,
            text="Send Manual Toggle",
            command=sendBack,
        )
        self.buttonRight = Button(
            self.root,
            text="Send Right",
            command=sendRight,
        )
        self.buttonLeft = Button(
            self.root,
            text="Send Left",
            command=sendLeft,
        )
        self.buttonManualToggle = Button(
            self.root,
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
            self.root,
            height=12,
            width=40,
        )
        self.sensorTextBox.pack(expand=True)
        self.recieve()
        self.root.mainloop()

        def recieve():
            read_buf = pi_socket.recv(1028)
            # <++>
            pickle.loads(read_buf)
            self.after(1000, self.recieve)


def sendStartStop():
    pi_socket.send((0).to_bytes(3, 'big'))


def sendForward():
    pi_socket.send((1).to_bytes(3, 'big'))


def sendBack():
    pi_socket.send((2).to_bytes(3, 'big'))


def sendRight():
    pi_socket.send((3).to_bytes(3, 'big'))


def sendLeft():
    pi_socket.send((4).to_bytes(3, 'big'))


def sendManualToggle():
    pi_socket.send((5).to_bytes(3, 'big'))
