import socket
import sys
import time
import os
import select
import serial

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sensor_ttyUSB = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=9600,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_TWO,
    timeout=1,
)

try:
    client_socket.bind(('0.0.0.0', 8027))
except socket.error as message:
    print(message)
    sys.exit()

client_socket.listen(9)


def uart_send(data: bytes):
    try:
        sensor_ttyUSB.write(data)
    except serial.SerialException as e:
        print(f"serial error: {e}")

    if 'sensor_ttyUSB' in locals() and sensor_ttyUSB.is_open:
        serial_port.close()

def uart_recv() -> bytes:
   read_buf = sensor_ttyUSB.read(256)
   if read_buf:
       print("Read from sensor module: " + read_buf.decode('utf-8'))
   print("read " + str(len(read_buf)) + " amount of bytes")


def main() -> int:
    try:
        conn, address = client_socket.accept()
        print('Got new connection')
        conn.setblocking(0)
        while True:
            ready = select.select([conn], [], [], 1.0)
            if ready[0]:
                data = conn.recv(3)
                # print("Data: " data.decode(encoding='utf-8')
                # print("Got data")
                if len(data) == 0:
                    print('Disconnected')
                    break
                # TODO: Decide what to do with incoming data
                match int.from_bytes(data, 'big'):
                    case 0:
                        print("Sending Hello, world!")
                        uart_send(b'Hello, world!')
                    case 1:
                        uart_send(b'Got message fram')
                    case 2:
                        uart_send(b'Got message svang hoger')
                    case 3:
                        uart_send(b'Got message svang vanster')
                    case 4:
                        uart_send(b'Got message manuell / autonom')
            # TODO: Send sensor and map data
            uart_recv()
    except KeyboardInterrupt:
        return 1


if __name__ == '__main__':
    sys.exit(main())
