import tkinter
import socket
import time

from tkinter import *

pi_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
pi_socket.connect(("10.42.0.1", 8027))

root = Tk()


def sendByte():
    pi_socket.send(b'Hello, world')


turn_on = Button(root, text="Send byte", command=sendByte)
turn_on.pack()


def send011():
    pi_socket.send((0).to_bytes(3, 'big'))
    pi_socket.send((1).to_bytes(3, 'big'))
    pi_socket.send((2).to_bytes(3, 'big'))
    pi_socket.send((3).to_bytes(3, 'big'))
    pi_socket.send((4).to_bytes(3, 'big'))
    pi_socket.send((5).to_bytes(3, 'big'))


button011 = Button(root, text="Send 011", command=send011)
button011.pack()

root.mainloop()
