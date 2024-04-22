# Server that receive data from the MCU and send it back

import socket
import sys
import time

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the address
server_address = ('0.0.0.0', 60003)
print('starting up on {} port {}'.format(*server_address))

sock.bind(server_address)

# Listen for incoming connections
sock.listen(1)


'''
1. Client connect to the server
2. Server send "0000000018192.168.1.1;60003;" to the client
3. Client send JSON data to the server
4. Server print the JSON data

3 and 4 will be in a loop
'''

while True:
    # Wait for a connection
    print('waiting for a connection')
    connection, client_address = sock.accept()
    try:
        print('connection from', client_address)

        # Send the server address to the client
        message = "0000000018192.168.1.1;60003;"
        connection.sendall(message.encode())

        # Receive the data in small chunks and retransmit it
        while True:
            data = connection.recv(16)
            print('received {!r}'.format(data))

    finally:
        # Clean up the connection
        connection.close()