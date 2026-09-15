#!/usr/bin/env python3
import socket, struct, json, sys

host = sys.argv[1] if len(sys.argv) > 1 else "127.0.0.1"
port = int(sys.argv[2]) if len(sys.argv) > 2 else 9000
s = socket.create_connection((host, port))

def send(payload):
    s.sendall(struct.pack("!I", len(payload)) + payload)
    n = struct.unpack("!I", s.recv(4))[0]
    data = b""
    while len(data) < n:
        data += s.recv(n-len(data))
    print(json.loads(data))

send(bytes([1]) + struct.pack("<QBBqQ", 100, 1, 1, 10000, 25))
send(bytes([1]) + struct.pack("<QBBqQ", 101, 2, 1, 10000, 10))
s.close()
