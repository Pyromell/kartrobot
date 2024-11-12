import socket
import sys
import time
import os
import select
import serial
from enum import Enum
from pprint import pp
import pickle

class Direction(Enum):
    EAST = 1
    NORTH = 2
    WEST = 3
    SOUTH = 4

currentDirection = Direction.NORTH

class SquareState(Enum):
    UNKNOWN = 0
    EMPTY = 1
    WALL = 2


mapData: list[list[bytes]] = [
    [SquareState.UNKNOWN for _ in range(75)]
    for _ in range(75)
]

mapPosition: tuple[int, int] = (37, 37)

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

def get_sensor_data() -> list[int] | None:
    read_buf = uart_recv(sensor_ttyUSB)
    if read_buf:
        print("read " + str(len(read_buf)) + " amount of bytes from SENSOR")
        # TODO: Convert to list[int] return read_buf
    return None

# NOTE: Blocks for 0.2 second to get sensor data
# Split this up later
def update_map(sensorData: list[int]) -> None:
    delta = []
    match currentDirection:
        case Direction.EAST:
            delta = [ (1, 0), (0, 1), (-1, 0), (0, -1) ]
        case Direction.NORTH:
            delta = [ (0, -1), (1, 0), (0, 1), (-1, 0) ]
        case Direction.WEST:
            delta = [ (-1, 0), (0, -1), (1, 0), (0, 1) ]
        case Direction.SOUTH:
            delta = [ (0, 1), (-1, 0), (0, -1), (1, 0) ]
    for i in range(4):
        nextPos = tuple(map(lambda x, y: x + y, mapPosition, delta[i]))
        if 10 <= sensorData[i] <= 50:
            mapData[nextPos[1]][nextPos[0]] = SquareState.WALL
        elif 50 < sensorData[i]:
            mapData[nextPos[1]][nextPos[0]] = SquareState.EMPTY

    return []


def send_sensor_data_to_interface(conn, sensorData) -> bool:
    global interface_ready_for_data
    pickled_data = pickle.dumps({ 'sensors': sensorData, 'mapd': mapData})
    pp(len(pickled_data))
    try:
        ready = select.select([], [conn], [], 0.01)
        print(interface_ready_for_data)
        if interface_ready_for_data and ready[1]:
            print("SENDING ALL")
            conn.sendall(pickled_data)
            interface_ready_for_data = False
    except ConnectionResetError:
        print("WARN: Connection reset")
        return False
    return True


def main() -> int:
    global interface_ready_for_data
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
                     case 1:
                         print('sending fram')
                         uart_send(driver_ttyUSB, (1).to_bytes(8, 'big'))
                     case 2:
                         print('sending back')
                         uart_send(driver_ttyUSB, (2).to_bytes(8, 'big'))
                     case 3:
                         print('sending svang höger')
                         uart_send(driver_ttyUSB, (3).to_bytes(8, 'big'))
                     case 4:
                         print('sending svang vänster')
                         uart_send(driver_ttyUSB, (4).to_bytes(8, 'big'))
                     case 5:
                         print('sending manuell / autonom')
                         uart_send(driver_ttyUSB, (5).to_bytes(8, 'big'))
                     case 6:
                         print('sending manuell / autonom')
                         uart_send(driver_ttyUSB, (6).to_bytes(8, 'big'))
                     case 255:
                         interface_ready_for_data = True
                     case -1:
                         print("Broken network connection")
                         break
                 driverReady = get_driver_data()
                 sensorData = get_sensor_data()
                 if driverReady:
                     sensorData = [
                         51,
                         10,
                         123092,
                         58,
                     ]
                     if sensorData:
                         update_map(sensorData)
                         # for i, v in enumerate(mapData):
                         #     pp(i)
                         #     pp([{num: value} for num, value in enumerate(v)], width=20)
                         print("Sending sensor data 1")
                         if not send_sensor_data_to_interface(conn, [1, 2, 3, 4]):
                             break
                 elif sensorData:
                     print("Sending sensor data 2")
                     send_sensor_data_to_interface(conn, sensorData)
    except KeyboardInterrupt:
        if sensor_ttyUSB.is_open:
            sensor_ttyUSB.close()
        if driver_ttyUSB.is_open:
            driver_ttyUSB.close()
        return 1


if __name__ == '__main__':
    sys.exit(main())

