import tkinter
import socket
import time
import pickle
import select
from enum import Enum
from struct import unpack
from pprint import pp

from tkinter import *

from typing import Optional

# pi_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# pi_socket.connect(("10.42.0.1", 8027))
# pi_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
# pi_socket.setblocking(0)

previousPos = (37, 37)


while (True):
    try:
        pi_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # pi_socket.settimeout(3)
        pi_socket.connect(("10.42.0.1", 8027))
        pi_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        pi_socket.setblocking(0)
        # pi_socket.send((0).to_bytes(1, 'big'))
        break
    except:
        print("Anslut till kartrobot07...")


class SquareState(Enum):
    UNKNOWN = 0
    EMPTY = 1
    WALL = 2
    ROBOT = 3
    START = 4
    VISITED = 5


class Interface():
    buffer = b''
    command_queue: Optional[bytes] = None

    def recieve(self):
        ready = select.select([pi_socket], [], [], 0)
        if ready[0]:
            self.buffer += pi_socket.recv(1024)
            try:
                messageData = pickle.loads(self.buffer)
                self.sensorTextBox.insert(
                    tkinter.END,
                    chars=str(messageData['sensors']),
                )
                for rownumber, row in enumerate(messageData['mapd']):
                    for colnumber, cell in enumerate(row):
                        match cell:
                            case SquareState.UNKNOWN:
                                pass
                                # self.canvas.create_rectangle(
                                #     colnumber * 8,
                                #     rownumber * 8,
                                #     (colnumber+1) * 8,
                                #     (rownumber+1) * 8,
                                #     fill='grey',
                                # )
                            case SquareState.EMPTY:
                                self.canvas.create_rectangle(
                                    colnumber * 8,
                                    rownumber * 8,
                                    (colnumber+1) * 8,
                                    (rownumber+1) * 8,
                                    fill='white',
                                )
                            case SquareState.WALL:
                                self.canvas.create_rectangle(
                                    colnumber * 8,
                                    rownumber * 8,
                                    (colnumber+1) * 8,
                                    (rownumber+1) * 8,
                                    fill='black',
                                )
                            case SquareState.ROBOT:
                                self.canvas.create_rectangle(
                                    colnumber * 8,
                                    rownumber * 8,
                                    (colnumber+1) * 8,
                                    (rownumber+1) * 8,
                                    fill='blue',
                                )
                                self.positionText.delete(1.0, END)
                                self.positionText.insert(
                                    END, str(colnumber)+','+str(rownumber))
                            case SquareState.START:
                                self.canvas.create_rectangle(
                                    colnumber * 8,
                                    rownumber * 8,
                                    (colnumber+1) * 8,
                                    (rownumber+1) * 8,
                                    fill='lime',
                                )
                if self.command_queue:
                    pi_socket.sendall(self.command_queue)
                    self.command_queue = None
                else:
                    pi_socket.sendall((255).to_bytes(8, 'big'))
            except pickle.UnpicklingError:
                print("Incomplete message...", len(self.buffer))
                self.tk.after_idle(self.recieve)
                return
            self.buffer = self.buffer = b''
        else:
            print("No data...")
        self.tk.after(1, self.recieve)

    def sendStartStop(self):
        print("Sending Start / Stop")
        self.command_queue = (0).to_bytes(8, 'big')

    def sendForward(self):
        print("Sending Forward")
        self.command_queue = (1).to_bytes(8, 'big')

    def sendBack(self):
        print("Sending Back")
        self.command_queue = (2).to_bytes(8, 'big')

    def sendRight(self):
        print("Sending Right")
        self.command_queue = (3).to_bytes(8, 'big')

    def sendLeft(self):
        print("Sending Left")
        self.command_queue = (4).to_bytes(8, 'big')

    def sendForwardShort(self):
        print("Sending Forward Short")
        self.command_queue = (5).to_bytes(8, 'big')

    def sendBackShort(self):
        print("Sending Back Short")
        self.command_queue = (6).to_bytes(8, 'big')

    def sendRightShort(self):
        print("Sending Right")
        self.command_queue = (7).to_bytes(8, 'big')

    def sendLeftShort(self):
        print("Sending Left")
        self.command_queue = (7).to_bytes(8, 'big')

    def sendManualToggle(self):
        print("Sending Manual Toggle")
        self.command_queue = (9).to_bytes(8, 'big')

    def reset(self):
        print("Exiting")
        self.command_queue = (10).to_bytes(8, 'big')

    def keyHandler(self, event):
        print(event.char, event.keysym, event.keycode)
        match event.char:
            case 'w':
                self.sendForwardShort()
            case 's':
                self.sendBackShort()
            case 'a':
                self.sendLeftShort()
            case 'd':
                self.sendRightShort()
            case 'r':
                exit()

    def __init__(self):
        self.tk = Tk()
        self.buttonFrame = Frame(
            self.tk,
            bg='aquamarine',
        )
        self.buttonStartStop = Button(
            self.buttonFrame,
            text="Send Start / Stop",
            command=self.sendStartStop,
        )
        self.buttonForward = Button(
            self.buttonFrame,
            text="Send Forward",
            command=self.sendForward,
        )
        self.buttonBack = Button(
            self.buttonFrame,
            text="Send Backwards",
            command=self.sendBack,
        )
        self.buttonRight = Button(
            self.buttonFrame,
            text="Send Right",
            command=self.sendRight,
        )
        self.buttonLeft = Button(
            self.buttonFrame,
            text="Send Left",
            command=self.sendLeft,
        )
        self.buttonManualToggle = Button(
            self.buttonFrame,
            text="Send Manual Toggle",
            command=self.sendManualToggle,
        )

        # Short commands

        self.buttonForwardShort = Button(
            self.buttonFrame,
            text="Send Forward Short",
            command=self.sendForwardShort,
        )
        self.buttonBackShort = Button(
            self.buttonFrame,
            text="Send Backwards Short",
            command=self.sendBackShort,
        )
        self.buttonRightShort = Button(
            self.buttonFrame,
            text="Send Right Short",
            command=self.sendRightShort,
        )
        self.buttonLeftShort = Button(
            self.buttonFrame,
            text="Send Left Short",
            command=self.sendLeftShort,
        )

        self.positionText = Text(
            self.buttonFrame, bg='white', width=8, height=1)

        self.canvas = tkinter.Canvas(self.tk, bg='grey', width=600, height=600)
        for colnumber in range(75):
            for rownumber in range(75):
                self.canvas.create_rectangle(
                    colnumber * 8,
                    rownumber * 8,
                    (colnumber+1) * 8,
                    (rownumber+1) * 8,
                    fill='grey',
                )

        self.canvas.grid(row=0, column=0)

        self.buttonFrame.grid(row=0, column=1)
        self.buttonStartStop.pack()
        self.buttonForward.pack()
        self.buttonBack.pack()
        self.buttonRight.pack()
        self.buttonLeft.pack()
        self.buttonManualToggle.pack()

        self.buttonForwardShort.pack()
        self.buttonBackShort.pack()
        self.buttonRightShort.pack()
        self.buttonLeftShort.pack()

        self.positionText.pack()
        self.sensorTextBox = tkinter.Text(
            self.buttonFrame,
            height=12,
            width=40,
            bg='black',
            fg='lime',
        )
        self.sensorTextBox.pack()

        self.tk.bind("<Key>", self.keyHandler)

        self.tk.after(1, self.recieve)
        self.tk.mainloop()


interFace = Interface()
