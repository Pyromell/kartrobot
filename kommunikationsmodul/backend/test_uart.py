import serial
driver_ttyUSB = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=9600,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_TWO,
    timeout=0.2,
)
sensor_ttyUSB = serial.Serial(
    port='/dev/ttyUSB1',
    baudrate=9600,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_TWO,
    timeout=0.2,
)

def uart_send(ttyUSB: serial.Serial, data: bytes):
    try:
        ttyUSB.write(data)
    except serial.SerialException as e:
        print(f"serial error: {e}")

def uart_recv(ttyUSB) -> bytes:
   read_buf = ttyUSB.read(256)
   print("UART RECV", read_buf)
   return read_buf

uart_send(driver_ttyUSB, (255).to_bytes(1, 'big'))
uart_send(sensor_ttyUSB, (255).to_bytes(1, 'big'))

if int.from_bytes(uart_recv(driver_ttyUSB), 'big') == 1:
    driver_ttyUSB, sensor_ttyUSB = sensor_ttyUSB, driver_ttyUSB
    print("DEBUG: switched UART connections")
elif int.from_bytes(uart_recv(sensor_ttyUSB), 'big') == 1:
    print("UART connections Should be fine?")
else:
    print("Neither USB port sent identifier??")
