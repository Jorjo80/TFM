import socket

UDP_IP = "fd00:7d03:7d03:7d03::a2e:9f"  # localhost
UDP_PORT = 12345
MESSAGE = "Hello, World!\n"

sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM) # UDP
sock.sendto(MESSAGE.encode(), (UDP_IP, UDP_PORT))

