import socket
import struct
import sample_pb2

def process_read(buffer, m_type, client_socket):
    if m_type == 1:
        message_one = sample_pb2.SampleMessageOne()
        message_one.ParseFromString(buffer)
        print("Received TYPE_ONE message:", message_one.value)

        # Send a response message to the client
        response_message = sample_pb2.SampleMessageTwo()
        response_message.message = "hello"
        serialized_response = response_message.SerializeToString()

        response_header = struct.pack('!ii', len(serialized_response), 2)
        client_socket.send(response_header + serialized_response)

    elif m_type == 2:
        message_two = sample_pb2.SampleMessageTwo()
        message_two.ParseFromString(buffer)
        print("Received TYPE_TWO message:", message_two.message)

        # Send a response message to the client
        response_message = sample_pb2.SampleMessageOne()
        response_message.value = 123456
        serialized_response = response_message.SerializeToString()

        response_header = struct.pack('!ii', len(serialized_response), 1)
        client_socket.send(response_header + serialized_response)
    else:
        print("Unknown message type")

def handle_client_connection(client_socket):
    # Receive header from client
    serialized_header = client_socket.recv(8)  # 8 bytes for int32 + 1 byte for '@'
    msg_length, m_type = struct.unpack('!ii', serialized_header)

    # Convert message length to host byte order
    msg_length = socket.ntohl(msg_length)

    # Receive message payload from client
    serialized_message = client_socket.recv(msg_length)

    process_read(serialized_message, m_type, client_socket)

    # Close the connection
    client_socket.close()

def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('127.0.0.1', 12345))
    server_socket.listen(5)
    print("Server listening on port 12345")

    while True:
        client_socket, addr = server_socket.accept()
        print("Client connected from:", addr)
        handle_client_connection(client_socket)

if __name__ == "__main__":
    start_server()



# import socket
# import struct
# import sample_pb2

# def process_read(buffer, msg_size, type):
#     if type == 0:
#         message_one = sample_pb2.SampleMessageOne()
#         message_one.ParseFromString(buffer)
#         print("Received TYPE_ONE message:", message_one.value)
#     elif type == 1:
#         message_two = sample_pb2.SampleMessageTwo()
#         message_two.ParseFromString(buffer)
#         print("Received TYPE_TWO message:", message_two.message)
#     else:
#         print("Unknown message type")

# def handle_client_connection(client_socket):
#     print("-----------START-------------")
#     # Receive header from client
#     serialized_header = client_socket.recv(9)  # 8 bytes for int32 + 1 byte for '@'
#     msg_length, _, type = struct.unpack('!iBi', serialized_header)

#     # Convert message length to host byte order
#     msg_length = socket.ntohl(msg_length)

#     print("Received message length:", msg_length)
#     print("Received message type:", type)

#     # Receive message payload from client
#     serialized_message = client_socket.recv(msg_length)

#     process_read(serialized_message, msg_length - 4, type)

#     # Close the connection
#     client_socket.close()
#     print("-----------END-------------")

# def start_server():
#     server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     server_socket.bind(('127.0.0.1', 12345))
#     server_socket.listen(5)
#     print("Server listening on port 12345")

#     while True:
#         client_socket, addr = server_socket.accept()
#         print("Client connected from:", addr)
#         handle_client_connection(client_socket)

# if __name__ == "__main__":
#     start_server()
