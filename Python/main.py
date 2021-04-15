import socket

UDP_IP = "fd00:7d03:7d03:7d03::a2e:39"  # localhost
UDP_PORT = 12345
MESSAGE = "Hello, World!\n"

#UDP_IP2 = "fd00:7d03:7d03:7d03::a2e:7"  # localhost


sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM) # UDP
sock.sendto(MESSAGE.encode(), (UDP_IP, UDP_PORT))

#sock.sendto(MESSAGE.encode(), (UDP_IP2, UDP_PORT))