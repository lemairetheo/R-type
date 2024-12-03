import socket

def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_address = ('127.0.0.1', 4242)

    try:
        message = b'Hello Server'
        print(f'Sending {message}')
        sent = sock.sendto(message, server_address)
        print(f'Sent {sent} bytes')
        sock.settimeout(2.0)

        try:
            print('Waiting for response...')
            data, server = sock.recvfrom(4096)
            print(f'Received {len(data)} bytes from {server}')
            print(f'Data: {data}')
        except socket.timeout:
            print('No response received - timeout')

    finally:
        print('Closing socket')
        sock.close()

if __name__ == '__main__':
    main()