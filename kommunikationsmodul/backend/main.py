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

currentDirection = Direction.NORTH
autoMode = False





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
    timeout=0.03,
)
sensor_ttyUSB = serial.Serial(
    port='/dev/ttyUSB1',
    baudrate=9600,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_TWO,
    timeout=0.03,
)

try:
    client_socket.bind(('0.0.0.0', 8027))
except socket.error as message:
    # print(message)
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

def get_simulated_data() -> list[int]:
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
            try:
                if fakeWalls[checkPos]:
                    fakeSensorData.append(10 + (distance-1)*50 + (random() * 10.0 - 5.0))
                    break
            except KeyError:
                pass
            if distance == 2:
                fakeSensorData.append(255)
    return fakeSensorData


# Graph = list[list[tuple]] = [tuple[tuple[int, int], bool] for _ in range(75) for _ in range(75)]
# #Tar startruta, målruta och mapdata som argument
# def dijkstra(s: tuple[int,int], t: tuple[int, int]) -> LifoQueue[tuple[tuple[int, int], bool]]:
    
#     # 75x75 of tuple: (mapData[XX][YY], VISITED = true/false)
    
    
#     shortestPath: LifoQueue[tuple[tuple[int, int], bool]] = LifoQueue()

#     for row in len(75):
#         for col in len(75):
#             Graph[s] = (s,True)
#             # if neighborlist == empty -> return, else:
#             # for each neighbor[1] == false: sätt true, lägg till i path, getNeighbours till neighbor
            
# def getNeighbours(sq = tuple[int,int]):
    
    # returna lista med alla neighbors till sq i Graph
             
            
            
        
    


def flood(goal: tuple[int, int]) -> Direction:
    floodQueue: LifoQueue[tuple[tuple[int, int], list[Direction]]] = LifoQueue()
    floodVisited: set[tuple[int, int]] = set()
    floodQueue.put((robotPosition, []))

    while not flood.empty():
        nextSquare, path_there = floodQueue.pop()
        floodVisited.put(nextSquare)
        for sq, direction in adjacentSquares(nextSquare):
            path_there.append(direction)
            if sq not in visitedSquares and mapData[sq] == SquareState.EMPTY:
                floodQueue.put(sq, path_there)
                if sq == goal:
                    return path_there[0]
            path_there.pop()
            


def adjacentSquares() -> tuple[tuple[int, int], tuple[int, int], tuple[int, int], tuple[int, int]]:
    return (
        (robotPosition[0], robotPosition[1] - 1), Direction.NORTH,
        (robotPosition[0] + 1, robotPosition[1]), Direction.EAST,
        (robotPosition[0], robotPosition[1] + 1), Direction.SOUTH,
        (robotPosition[0] - 1, robotPosition[1]), Direction.WEST,
    )

# Init queue
queue: LifoQueue[tuple[int, int]] = LifoQueue()
queue.put((robotPosition[0], robotPosition[1] - 1)) # start
visitedSquares: set[tuple[int, int]] = set()

# Hitta nästa ruta att åka till
def pathfind_empty():

    nextSquare = queue.get()
    
    print("next square", nextSquare)
    while robotPosition != nextSquare:
        command = flood(nextSquare)
        print("command", command)
        visitedSquares.add(robotPosition)
        # TODO: Convert direction to command
        uart_send(command)
    for adjacentSquare in adjacentSquares(nextSquare):
        if adjacentSquare not in visitedSquares and mapData[adjacentSquare] == SquareState.EMPTY:
            queue.put(adjacentSquare)


def get_sensor_data() -> list[int] | None:
    sensor_ttyUSB.write((254).to_bytes(1, 'big'))
    read_buf = uart_recv(sensor_ttyUSB)
    if read_buf:
        print("read " + str(len(read_buf)) + " amount of bytes from SENSOR")
        pp(int.from_bytes(read_buf[0:2], 'big', signed = True))
        # b1 = int.from_bytes(read_buf[0:1], 'big')
        # b2 = int.from_bytes(read_buf[1:2], 'big')
        # pp(b2 << 8 | b1)
        # TODO: Convert to list[int] return read_buf
    return None

# NOTE: Blocks for 0.2 second to get sensor data
# Split this up later

def update_map(sensorData: list[int]) -> None:
    global interface_ready_for_data, robotPosition, currentDirection, autoMode, lastPosition
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
    global interface_ready_for_data, robotPosition, currentDirection, autoMode, lastPosition
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

def send_forward(short: bool) -> None:
    global interface_ready_for_data, robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB
    print('sending fram', short)
    if short:
        uart_send(driver_ttyUSB, (1).to_bytes(1, 'big'))
    else:
        uart_send(driver_ttyUSB, (5).to_bytes(1, 'big'))
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
    global interface_ready_for_data, robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB
    print('sending back', short)
    if short:
        uart_send(driver_ttyUSB, (2).to_bytes(1, 'big'))
    else:
        uart_send(driver_ttyUSB, (6).to_bytes(1, 'big'))
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
    global interface_ready_for_data, robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB
    print('sending svang höger')
    if short:
        uart_send(driver_ttyUSB, (3).to_bytes(1, 'big'))
        # TODO: How do we know what direction???
    else:
        uart_send(driver_ttyUSB, (7).to_bytes(1, 'big'))
        currentDirection = (int(currentDirection) + 1) % 4

def turn_left(short: bool) -> None:
    global interface_ready_for_data, robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB
    print('sending svang vänster')
    if short:
        uart_send(driver_ttyUSB, (4).to_bytes(1, 'big'))
        # TODO: How do we know what direction???
    else:
        uart_send(driver_ttyUSB, (8).to_bytes(1, 'big'))
        currentDirection = (int(currentDirection) + 3) % 4

def main() -> int:
    global interface_ready_for_data, robotPosition, currentDirection, autoMode, lastPosition, driver_ttyUSB, sensor_ttyUSB
    try:
        uart_send(driver_ttyUSB, (255).to_bytes(1, 'big'))
        uart_send(sensor_ttyUSB, (255).to_bytes(1, 'big'))
        # NOTE: This depends on the format they send data in
        if int.from_bytes(uart_recv(driver_ttyUSB), 'big') == 1:
            driver_ttyUSB, sensor_ttyUSB = sensor_ttyUSB, driver_ttyUSB
            print("DEBUG: switched UART connections")
        elif int.from_bytes(uart_recv(sensor_ttyUSB), 'big') == 1:
            print("UART connections Should be fine?")
        else:
            print("??")
        while True:
            conn, address = client_socket.accept()
            interface_ready_for_data = True
            #interfaceCommand = get_interface_data(conn)
            print('Got new connection')
            while True:
                if not autoMode:
                    interfaceCommand = get_interface_data(conn)
                    match interfaceCommand:
                        case 0:
                            print('sending Start/Stop!')
                            uart_send(driver_ttyUSB, (0).to_bytes(1, 'big'))
                            interface_ready_for_data = True
                        case 1:
                            send_forward(False)
                            interface_ready_for_data = True
                        case 2:
                            send_backward(False)
                            interface_ready_for_data = True
                        case 3:
                            turn_right(False)
                            interface_ready_for_data = True
                        case 4:
                            turn_left(False)
                            interface_ready_for_data = True
                        case 5:
                            send_forward(True)
                            interface_ready_for_data = True
                        case 6:
                            send_backward(True)
                            interface_ready_for_data = True
                        case 7:
                            turn_right(True)
                            interface_ready_for_data = True
                        case 8:
                            turn_left(True)
                            interface_ready_for_data = True
                        case 9:
                            print('got autonom')
                            uart_send(driver_ttyUSB, (5).to_bytes(1, 'big'))
                            interface_ready_for_data = True
                            autoMode = True
                        case 10:
                            sys.exit(0)
                        case 255:
                            interface_ready_for_data = True
                        case -1:
                            print("Broken network connection")
                            break
                    driverReady = get_driver_data()
                    # sensorData = get_simulated_data()
                    sensorData = get_sensor_data()
                    if driverReady:
                        if sensorData:
                            update_map(sensorData)
                    # TODO: Wait updating interface until new square?
                    send_sensor_data_to_interface(conn, sensorData)
                elif autoMode:
                    interfaceCommand = get_interface_data(conn)
                    match interfaceCommand:
                        case 1:
                            interface_ready_for_data = True
                        case 2:
                            interface_ready_for_data = True
                        case 3:
                            interface_ready_for_data = True
                        case 4:
                            interface_ready_for_data = True
                        case 5:
                            print('got manual')
                            interface_ready_for_data = True
                            autoMode = False
                        case 255:
                            interface_ready_for_data = True
                        case -1:
                            print("Broken network connection")
                            break
                    driverReady = get_driver_data()
                    sensorData = get_simulated_data()
                    #sensorData = get_sensor_data()
                    if driverReady:
                        if sensorData:
                            update_map(sensorData)
                    # TODO: Wait updating interface until new square?
                    send_sensor_data_to_interface(conn, sensorData)
                    pathfind_empty()

    except KeyboardInterrupt:
        if sensor_ttyUSB.is_open:
            sensor_ttyUSB.close()
        if driver_ttyUSB.is_open:
            driver_ttyUSB.close()
        return 1


if __name__ == '__main__':
    sys.exit(main())

