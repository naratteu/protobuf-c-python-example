# Protobuf Example : Python Server & C Client

This is a simple example demonstrating how to establish communication between a Python server and a C client using Protocol Buffers (protobuf) for data serialization and deserialization.

#### Communication flow will be as follows:

1. The Python server will listen for incoming connections.
2. The C client will establish a connection to the server.
3. The C client will send a sample message of type 1 (TYPE_ONE) to the server.
4. The Python server will receive the message, parse it, and print the value.
5. The Python server will send a response message of type 2 (TYPE_TWO) back to the client.
6. The C client will receive the response message from the server and print the received message.



## Getting Started

### Prerequisites

-   Protoc compiler & C library for protobuf generated code

    ```sh
    sudo apt install  protobuf-compiler  libprotobuf-c-dev
    ```

<!-- -   Python libraries
    ```sh
    pip3 install protobuf
    ``` -->

### Usage

1. Generate c and python code from protobuf & compile c code

    ```sh
    make
    ```

2. Run server and client

    ```sh
    #server
    python3 server-python.py

    #client
    ./client
    ```
