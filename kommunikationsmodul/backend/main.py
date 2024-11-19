#!/bin/env python3
import socket
import sys
import time
import os
import select
import serial
from enum import IntEnum
from pprint import pp
from random import random
from queue import LifoQueue
import pickle

class Direction(IntEnum):
    NORTH = 0
    EAST = 1
    SOUTH = 2
    WEST = 3

currentDirection = Direction.NORTH

class SquareState(IntEnum):
    UNKNOWN = 0
    EMPTY = 1
    WALL = 2
    ROBOT = 3
    START = 4


mapData: list[list[bytes]] = [
    [SquareState.UNKNOWN for _ in range(75)]
    for _ in range(75)
]
mapData[38][37] = SquareState.START

# NOTE: (x, y)
lastPosition: tuple[int, int] = (37, 37)
robotPosition: tuple[int, int] = (37, 37)

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

interface_ready_for_data = False


driver_ttyUSB = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=9600,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_TWO,
    timeout=0.01,
)
sensor_ttyUSB = serial.Serial(
    port='/dev/ttyUSB1',
    baudrate=9600,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_TWO,
    timeout=0.01,
)

try:
    client_socket.bind(('0.0.0.0', 8027))
except socket.error as message:
    print(message)
    sys.exit()

client_socket.listen(9)


def uart_send(ttyUSB: serial.Serial, data: bytes):
    try:
        ttyUSB.write(data)
    except serial.SerialException as e:
        print(f"serial error: {e}")

def uart_recv(ttyUSB) -> bytes:
   read_buf = ttyUSB.read(256)
   return read_buf

# NOTE: Blocks for 0.2 second trying to read data
def get_interface_data(conn) -> int:
    ready = select.select([conn], [], [], 0.01)
    try:
        if ready[0]:
            data = conn.recv(64)
            if len(data) == 0:
                print('Disconnected')
                return -1
            return int.from_bytes(data, 'big')
    except select.error:
        print("DISCONNECTED")
        sys.exit(1)

# NOTE: Blocks for 0.2 second to get driver data
def get_driver_data() -> bool:
    read_buf = uart_recv(driver_ttyUSB)
    if read_buf:
        print("read " + str(len(read_buf)) + " amount of bytes from DRIVER")
    # FIXME
    return True

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

has_printed = False

def get_sensor_data() -> list[int]:
    global has_printed
    fakeSensorData = []
    for d in range(4):
        for distance in [1, 2]:
            delta = (0, 0)
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
            checkPos = tuple(map(lambda t1, t2: t1 + t2 * distance, robotPosition, delta[d]))
            if not has_printed:
                print(checkPos)
            try:
                if fakeWalls[checkPos]:
                    fakeSensorData.append(10 + (distance-1)*50 + (random() * 10.0 - 5.0))
                    break
            except KeyError:
                pass
            if distance == 2:
                fakeSensorData.append(255)
    has_printed = True
    return fakeSensorData

queue: LifoQueue[tuple[int, int]] = LifoQueue()
visitedSquares: set[tuple[int, int]] = set()
def pathfind():
    while not queue.empty():
        # next_square = queue.pop_back()
        # lista ut hur åka till next_square
        # visitedSquares.add(next_square)
        # skicka kommando
        # kolla intilligande väggar/empty
        # lägg till alla empty på queue (om ej i visitedSquares)
        # for adjacentSquare(nextSquare):
        #     if not visitedSquares.has(adjacentSquare) and adjacentSquare == SquareState.EMPTY:
        #         queue.push_back(adjacentSquare)
        pass


#def get_sensor_data() -> list[int] | None:
#    read_buf = uart_recv(sensor_ttyUSB)
#    if read_buf:
#        print("read " + str(len(read_buf)) + " amount of bytes from SENSOR")
#        # TODO: Convert to list[int] return read_buf
#    return None

# NOTE: Blocks for 0.2 second to get sensor data
# Split this up later
def update_map(sensorData: list[int]) -> None:
    global currentDirection
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
        if sensorData[i] <= 50:
            mapData[nextPos[1]][nextPos[0]] = SquareState.WALL
        elif 50 < sensorData[i] < 80:
            mapData[nextPos[1]][nextPos[0]] = SquareState.EMPTY
            mapData[nextPosX2[1]][nextPosX2[0]] = SquareState.WALL
        else:
            if mapData[nextPos[1]][nextPos[0]] != SquareState.START:
                mapData[nextPos[1]][nextPos[0]] = SquareState.EMPTY
    # TODO: Don't replace START
    mapData[lastPosition[1]][lastPosition[0]] = SquareState.EMPTY
    mapData[robotPosition[1]][robotPosition[0]] = SquareState.ROBOT
    return []


def send_sensor_data_to_interface(conn, sensorData) -> bool:
    global interface_ready_for_data
    pickled_data = pickle.dumps({ 'sensors': sensorData, 'mapd': mapData})
    try:
        ready = select.select([], [conn], [], 0.01)
        if interface_ready_for_data and ready[1]:
            conn.sendall(pickled_data)
            interface_ready_for_data = False
    except ConnectionResetError:
        print("WARN: Connection reset")
        return False
    return True


def main() -> int:
    global interface_ready_for_data, robotPosition, currentDirection
    try:
        while True:
            conn, address = client_socket.accept()
            interface_ready_for_data = True
            print('Got new connection')
            while True:
                 interfaceCommand = get_interface_data(conn)
                 match interfaceCommand:
                     case 0:
                         print('sending Start/Stop!')
                         uart_send(driver_ttyUSB, (0).to_bytes(8, 'big'))
                         interface_ready_for_data = True
                     case 1:
                         print('sending fram')
                         uart_send(driver_ttyUSB, (1).to_bytes(8, 'big'))
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
                         interface_ready_for_data = True
                     case 2:
                         print('sending back')
                         uart_send(driver_ttyUSB, (2).to_bytes(8, 'big'))
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
                         interface_ready_for_data = True
                     case 3:
                         print('sending svang höger')
                         uart_send(driver_ttyUSB, (3).to_bytes(8, 'big'))
                         currentDirection = (int(currentDirection) + 1) % 4
                         interface_ready_for_data = True
                     case 4:
                         print('sending svang vänster')
                         uart_send(driver_ttyUSB, (4).to_bytes(8, 'big'))
                         currentDirection = (int(currentDirection) + 3) % 4
                         interface_ready_for_data = True
                     case 5:
                         print('sending manuell / autonom')
                         uart_send(driver_ttyUSB, (5).to_bytes(8, 'big'))
                         interface_ready_for_data = True
                     case 255:
                         interface_ready_for_data = True
                     case -1:
                         print("Broken network connection")
                         break
                 driverReady = get_driver_data()
                 sensorData = get_sensor_data()
                 if driverReady:
                     if sensorData:
                         update_map(sensorData)
                 send_sensor_data_to_interface(conn, sensorData)
    except KeyboardInterrupt:
        if sensor_ttyUSB.is_open:
            sensor_ttyUSB.close()
        if driver_ttyUSB.is_open:
            driver_ttyUSB.close()
        return 1


if __name__ == '__main__':
    sys.exit(main())

