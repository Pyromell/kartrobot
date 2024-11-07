import socket
import sys
import time
import os
import select

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
	client_socket.bind(('0.0.0.0', 8027))
except socket.error as message:
	print(message)
	sys.exit()

client_socket.listen(9)

def main() -> int:
	try:
		conn, address = client_socket.accept()
		conn.setblocking(0)
		while True:
			ready = select.select([conn], [], [], 1.0)
			# TODO: Detect when conn has been broken
			if ready[0]:
				data = conn.recv(3)
				print(data)
				# TODO: Decide what to do with incoming data
				match int.from_bytes(data, 'big'):
					case 0:
						print("Got message start/stop")
					case 1:
						print("Got message fram")
					case 2:
						print("Got message sväng höger")
					case 3:
						print("Got message sväng vänster")
					case 4:
						print("Got message manuell / autonom")
			# TODO: Send sensor and map data
	except KeyboardInterrupt:
		return 1
	
if __name__ == '__main__':
	sys.exit(main())
