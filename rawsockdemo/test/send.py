import socket
import sys

UDP_IP = "169.254.69.101"
UDP_PORT = 5005
MESSAGE = sys.argv[1] if len(sys.argv) > 1 else "Hello, World!"

print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT
print "message:", MESSAGE

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))
