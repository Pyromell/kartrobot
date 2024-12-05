#!/bin/env python3
from copy import deepcopy
from enum import IntEnum
from math import floor
from pprint import pp
from queue import LifoQueue, Queue
from random import random
import datetime
import os
import struct
import pickle
import select
import serial
import socket
import sys
import time

from threading import Thread

DEBUG_STANDALONE_MODE: bool = False

NETWORK_PACKET_LEN: int = 11716

# INIT:

class Direction(IntEnum):
    NORTH = 0
    EAST = 1
    SOUTH = 2
    WEST = 3

class SquareState(IntEnum):
    UNKNOWN = 0
    EMPTY = 1
    WALL = 2
    ROBOT = 3
    START = 4

currentDirection: Direction = Direction.NORTH
autoMode: bool = False

mapData: list[list[SquareState]] = [
    [SquareState.UNKNOWN for _ in range(75)]
    for _ in range(75)
]
mapData[38][37] = SquareState.START

# NOTE: (x, y)
lastPosition: tuple[int, int] = (37, 38)
robotPosition: tuple[int, int] = (37, 38)

driverReady: bool = True
driver_ttyUSB: serial.Serial | None = None
sensor_ttyUSB: serial.Serial | None = None

try:
    driver_ttyUSB = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate=9600,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=0.1,
    )
    sensor_ttyUSB = serial.Serial(
        port='/dev/ttyUSB1',
        baudrate=9600,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=0.1,
    )
except serial.SerialException:
    print("DEBUG: UART serial objects not created")
    pass


def uart_send(ttyUSB: serial.Serial | None, data: bytes):
    if ttyUSB:
        try:
            ttyUSB.write(data)
        except serial.SerialException as e:
            print(f"serial error: {e}")

def uart_recv(ttyUSB: serial.Serial | None) -> bytes:
    if ttyUSB:
        readBuf: bytes = ttyUSB.read(256)
        return readBuf
    return b''

def get_interface_data(conn) -> int:
    ready = select.select([conn], [], [], 0)
    try:
        if ready[0]:
            data = conn.recv(64)
            if len(data) == 0:
                print('Disconnected')
                return -1
            print(len(data))
            return int.from_bytes(data, 'big')
        return -2
    except select.error:
        print("DISCONNECTED")

# 1 = börjat köra, 2 = klar
def getDriverData() -> bool:
    if DEBUG_STANDALONE_MODE:
        return True
    readBuf: bytes = uart_recv(driver_ttyUSB)
    print(readBuf)
    print("get driver data")
    if readBuf:
        print("read " + str(len(readBuf)) + " amount of bytes from DRIVER")
    if int.from_bytes(readBuf, 'big') == 0x0B:
        print("DRIVER IS READY")
        return True
    else:
        return False

fakeWalls: dict[tuple[int, int], bool] = {
    (38, 37): True,
    (39, 36): True,
    (38, 35): True,
    (37, 34): True,
    (38, 33): True,
    (37, 33): True,
    (36, 33): True,
    (35, 33): True,
    (34, 33): True,
    (33, 33): True,
    (32, 33): True,
    (31, 33): True,
    (31, 34): True,
    (31, 35): True,
    (31, 36): True,
    (31, 37): True,
    (31, 38): True,
    (31, 39): True,
    (31, 40): True,
    (32, 40): True,
    (33, 40): True,
    (34, 40): True,
    (35, 40): True,
    (36, 38): True,
    (36, 39): True,
    (37, 38): False, # "ingång"
    # (37, 38): True,
    
    # Köksön
    (34, 37): True,
    (33, 37): True,
    (33, 36): True,
    (33, 35): True,
    (34, 35): True,

}

def getSimulatedSensorData() -> list[int]:
    fakeSensorData = []
    for d in range(4):
        for distance in [1, 2]:
            delta: list[tuple[int, int]] = []
            direction = Direction(d)
            match currentDirection:
                case Direction.NORTH:
                    delta = [ (0, -1), (1, 0), (0, 1), (-1, 0) ]
                case Direction.EAST:
                    delta = [ (1, 0), (0, 1), (-1, 0), (0, -1) ]
                case Direction.SOUTH:
                    delta = [ (0, 1), (-1, 0), (0, -1), (1, 0) ]
                case Direction.WEST:
                    delta = [ (-1, 0), (0, -1), (1, 0), (0, 1) ]
            checkPos: tuple[int, int] = tuple(map(lambda t1, t2: t1 + t2 * distance, robotPosition, delta[d]))
            try:
                if fakeWalls[checkPos]:
                    fakeSensorData.append(10 + floor((distance-1)*50 + (random() * 10.0 - 5.0)))
                    break
            except KeyError:
                pass
            if distance == 2:
                fakeSensorData.append(255)
    return fakeSensorData


def flood(goal: tuple[int, int]) -> Direction:
    floodQueue: Queue[tuple[tuple[int, int], list[Direction]]] = Queue()
    floodVisited: set[tuple[int, int]] = set()
    floodQueue.put((robotPosition, []))

    while not floodQueue.empty():
        next_square, path_there = floodQueue.get()
        if next_square in floodVisited:
            continue
        floodVisited.add(next_square)
        for sq, direction in adjacentSquares(next_square):
            path_there.append(direction)
            if sq not in floodVisited and mapData[sq[1]][sq[0]] == SquareState.EMPTY:
                floodQueue.put((sq, deepcopy(path_there)))
                if sq == goal:
                    return path_there[0]
            path_there.pop()
    raise Exception("unreachable (flood)")
            


def adjacentSquares(pos: tuple[int, int]) -> list[tuple[tuple[int, int], Direction]]:
    global robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB, driverReady, currentDirection
    return [
        ((pos[0], pos[1] - 1), Direction.NORTH),
        ((pos[0] + 1, pos[1]), Direction.EAST),
        ((pos[0], pos[1] + 1), Direction.SOUTH),
        ((pos[0] - 1, pos[1]), Direction.WEST),
    ]

class Command(IntEnum):
    CMD_FORWARD = 1
    CMD_BACKWARD = 2
    CMD_TURNRIGHT = 3
    CMD_TURNLEFT = 4


def directionToCommand(direction: Direction) -> Command:
    global robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB, driverReady, currentDirection
    if direction < currentDirection:
        return Command.CMD_TURNLEFT
    elif direction > currentDirection:
        return Command.CMD_TURNRIGHT
    elif abs(direction - currentDirection) == 2:
        return Command.CMD_BACKWARD
    else:
        return Command.CMD_FORWARD

# Init queue
queue: LifoQueue[tuple[int, int]] = LifoQueue()
queue.put((robotPosition[0], robotPosition[1] - 1)) # start
visitedSquares: set[tuple[int, int]] = set()

# Hitta nästa ruta att åka till
def pathfindEmpty():
    global robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB, driverReady, currentDirection, queue
    if queue.empty():
        if robotPosition != (37, 37):
            queue.put((37, 37))
        else:
            moveToDirection(Direction.SOUTH)
            autoMode = False
            return
    next_square = queue.get()
    lastPosition = robotPosition
    while robotPosition != next_square:
        nextDirection: Direction = flood(next_square)
        moveToDirection(nextDirection)

def moveToDirection(nextDirection: Direction) -> None:
    global robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB, driverReady, currentDirection, queue
    while True:
        command: Command = directionToCommand(nextDirection)
        visitedSquares.add(robotPosition)
        sendCommandWithRetry(command.to_bytes(1, 'big'))
        while not driverReady:
            driverReady = getDriverData()
        if command == Command.CMD_FORWARD:
            match currentDirection:
                case Direction.NORTH:
                    robotPosition = (robotPosition[0], robotPosition[1] - 1)
                case Direction.EAST:
                    robotPosition = (robotPosition[0] + 1, robotPosition[1])
                case Direction.SOUTH:
                    robotPosition = (robotPosition[0], robotPosition[1] + 1)
                case Direction.WEST:
                    robotPosition = (robotPosition[0] - 1, robotPosition[1])
            break
        elif command == Command.CMD_BACKWARD:
            print("BACKWARD")
            match currentDirection:
                case Direction.NORTH:
                    robotPosition = (robotPosition[0], robotPosition[1] + 1)
                case Direction.EAST:
                    robotPosition = (robotPosition[0] - 1, robotPosition[1])
                case Direction.SOUTH:
                    robotPosition = (robotPosition[0], robotPosition[1] - 1)
                case Direction.WEST:
                    robotPosition = (robotPosition[0] + 1, robotPosition[1])
            break
        elif command == Command.CMD_TURNRIGHT:
            currentDirection = Direction((currentDirection + 1) % 4)
        elif command == Command.CMD_TURNLEFT:
            currentDirection = Direction((currentDirection - 1) % 4)

def addAdjacent():
    for adjSq, _ in adjacentSquares(robotPosition):
        if adjSq not in visitedSquares and mapData[adjSq[1]][adjSq[0]] == SquareState.EMPTY:
            queue.put(adjSq)


def getSensorData() -> list[int] | None:
    uart_send(sensor_ttyUSB, (253).to_bytes(1, 'big'))
    readBuf: bytes = uart_recv(sensor_ttyUSB)
    if readBuf:
        print("read " + str(len(readBuf)) + " amount of bytes from SENSOR")
        print("FR", int.from_bytes(readBuf[0:1], 'big', signed=False))
        print("FL", int.from_bytes(readBuf[1:2], 'big', signed=False))
        print("F", int.from_bytes(readBuf[2:3], 'big', signed=False))
        print("BR", int.from_bytes(readBuf[3:4], 'big', signed=False))
        print("BL", int.from_bytes(readBuf[4:5], 'big', signed=False))
        print("B", int.from_bytes(readBuf[5:6], 'big', signed=False))
        #return [
        #    int.from_bytes(readBuf[0:1], 'big', signed=False),
        #    int.from_bytes(readBuf[1:2], 'big', signed=False),
        #    int.from_bytes(readBuf[2:3], 'big', signed=False),
        #    int.from_bytes(readBuf[3:4], 'big', signed=False),
        #    int.from_bytes(readBuf[4:5], 'big', signed=False),
        #    int.from_bytes(readBuf[5:6], 'big', signed=False),
        #]

        return [
            int.from_bytes(readBuf[2:3], 'big', signed=False),
            int.from_bytes(readBuf[0:1], 'big', signed=False), 
            int.from_bytes(readBuf[5:6], 'big', signed=False), 
            int.from_bytes(readBuf[1:2], 'big', signed=False), 
         ]
    return None

# NOTE: Blocks for 0.2 second to get sensor data
# Split this up later

def updateMap(sensorData: list[int]) -> None:
    global robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB, driverReady, currentDirection
    delta = []
    match currentDirection:
        case Direction.NORTH:
            delta = [ (0, -1), (1, 0), (0, 1), (-1, 0) ]
        case Direction.EAST:
            delta = [ (1, 0), (0, 1), (-1, 0), (0, -1) ]
        case Direction.SOUTH:
            delta = [ (0, 1), (-1, 0), (0, -1), (1, 0) ]
        case Direction.WEST:
            delta = [ (-1, 0), (0, -1), (1, 0), (0, 1) ]
    for i in range(4):
        nextPos = tuple(map(lambda t1, t2: t1 + t2, robotPosition, delta[i]))
        nextPosX2 = tuple(map(lambda t1, t2: t1 + t2 * 2, robotPosition, delta[i]))
        if not sensorData:
            pass
        elif sensorData[i] <= 50:
            mapData[nextPos[1]][nextPos[0]] = SquareState.WALL
        elif 50 < sensorData[i] < 80:
            mapData[nextPos[1]][nextPos[0]] = SquareState.EMPTY
            mapData[nextPosX2[1]][nextPosX2[0]] = SquareState.WALL
        else:
            if mapData[nextPos[1]][nextPos[0]] != SquareState.START:
                mapData[nextPos[1]][nextPos[0]] = SquareState.EMPTY
    if mapData[lastPosition[1]][lastPosition[0]] != SquareState.START:
        mapData[lastPosition[1]][lastPosition[0]] = SquareState.EMPTY
    mapData[robotPosition[1]][robotPosition[0]] = SquareState.ROBOT


def sendSensorDataToInterface(conn, sensorData: list[int]) -> bool:
    global robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB, driverReady, currentDirection
    pickled_data = pickle.dumps({ 'sensors': sensorData, 'mapd': mapData})
    length: int = len(pickled_data)
    print("sending")
    try:
        conn.sendall(struct.pack("<I", length) + pickled_data)
    except ConnectionResetError:
        print("WARN: Connection reset")
        return False
    print("sending donne")
    return True

def sendCommandWithRetry(data: bytes) -> None:
    global driverReady
    print("command", data)
    if not driverReady:
        return
    uart_send(driver_ttyUSB, data)
    if not DEBUG_STANDALONE_MODE:
        while int.from_bytes(uart_recv(driver_ttyUSB), 'big') != 0x0A:
            uart_send(driver_ttyUSB, data)
    driverReady = False

def send_stop() -> None:
    global robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB, driverReady, currentDirection
    print('sending stop')
    uart_send(driver_ttyUSB, (0).to_bytes(1, 'big'))

def send_forward(short: bool) -> None:
    global robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB, driverReady, currentDirection
    print('sending fram', short)
    if short:
        sendCommandWithRetry((5).to_bytes(1, 'big'))
    else:
        sendCommandWithRetry((1).to_bytes(1, 'big'))
        lastPosition = robotPosition
        match currentDirection:
           case Direction.NORTH:
               robotPosition = (robotPosition[0], robotPosition[1] - 1)
           case Direction.EAST:
               robotPosition = (robotPosition[0] + 1, robotPosition[1])
           case Direction.SOUTH:
               robotPosition = (robotPosition[0], robotPosition[1] + 1)
           case Direction.WEST:
               robotPosition = (robotPosition[0] - 1, robotPosition[1])

def send_backward(short: bool) -> None:
    global robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB, driverReady, currentDirection
    print('sending back', short)
    if short:
        sendCommandWithRetry((6).to_bytes(1, 'big'))
    else:
        sendCommandWithRetry((2).to_bytes(1, 'big'))
        lastPosition = robotPosition
        match currentDirection:
           case Direction.NORTH:
               robotPosition = (robotPosition[0], robotPosition[1] + 1)
           case Direction.EAST:
               robotPosition = (robotPosition[0] - 1, robotPosition[1])
           case Direction.SOUTH:
               robotPosition = (robotPosition[0], robotPosition[1] - 1)
           case Direction.WEST:
               robotPosition = (robotPosition[0] + 1, robotPosition[1])

def turn_right(short: bool) -> None:
    global robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB, driverReady, currentDirection
    print('sending svang höger')
    if short:
        sendCommandWithRetry((7).to_bytes(1, 'big'))
    else:
        sendCommandWithRetry((3).to_bytes(1, 'big'))
        currentDirection = Direction((int(currentDirection) + 1) % 4)

def turn_left(short: bool) -> None:
    global robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB, driverReady, currentDirection
    print('sending svang vänster')
    if short:
        sendCommandWithRetry((8).to_bytes(1, 'big'))
    else:
        sendCommandWithRetry((4).to_bytes(1, 'big'))
        currentDirection = Direction((int(currentDirection) + 3) % 4)


def main() -> int:
    global robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB, driverReady, currentDirection, queue
    try:
        queue.put((robotPosition[0], robotPosition[1] - 1))
        uart_send(driver_ttyUSB, (255).to_bytes(1, 'big'))
        uart_send(sensor_ttyUSB, (255).to_bytes(1, 'big'))
        # NOTE: This depends on the format they send data in
        if int.from_bytes(uart_recv(driver_ttyUSB), 'big') == 1:
            driver_ttyUSB, sensor_ttyUSB = sensor_ttyUSB, driver_ttyUSB
            print("DEBUG: switched UART connections")
        elif int.from_bytes(uart_recv(sensor_ttyUSB), 'big') == 1:
            print("UART connections Should be fine?")
        else:
            print("Neither USB port sent identifier??")

        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        try:
            client_socket.bind(('0.0.0.0', 8027))
        except socket.error as message:
            print(message)
            sys.exit(8)
        client_socket.listen(9)

        # Börja med att köra framåt
        send_forward(False)

        while True:
            conn, address = client_socket.accept()
            print('Got new connection')
            while True:
                print("LOOP")
                interfaceCommand = get_interface_data(conn)
                match interfaceCommand:
                    case 0:
                        if not autoMode:
                            send_stop()
                        pass
                    case 1:
                        if not autoMode:
                            send_forward(False)
                    case 2:
                        if not autoMode:
                            send_backward(False)
                    case 3:
                        if not autoMode:
                            turn_right(False)
                    case 4:
                        if not autoMode:
                            turn_left(False)
                    case 6:
                        if not autoMode:
                            send_forward(True)
                    case 6:
                        if not autoMode:
                            send_backward(True)
                    case 7:
                        if not autoMode:
                            turn_right(True)
                    case 8:
                        if not autoMode:
                            turn_left(True)
                    case 9:
                        autoMode = not autoMode
                        continue
                    case 10:
                        return 0
                    case -1:
                        print("Broken network connection")
                        break
                if not driverReady:
                    driverReady = getDriverData()

                sensorData: list[int] | None = None
                if driverReady:
                    # sensorData = getSensorData()
                    if not DEBUG_STANDALONE_MODE:
                        sensorData = getSensorData()
                    else:
                        sensorData = getSimulatedSensorData()

                    updateMap(sensorData)
                    if autoMode and sensorData:
                        addAdjacent();
                        pathfindEmpty()
                sendSensorDataToInterface(conn, sensorData)

    except KeyboardInterrupt:
        if sensor_ttyUSB and sensor_ttyUSB.is_open:
            sensor_ttyUSB.close()
        if driver_ttyUSB and driver_ttyUSB.is_open:
            driver_ttyUSB.close()
        return 1


if __name__ == '__main__':
    sys.exit(main())

