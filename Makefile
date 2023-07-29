# Makefile to compile server.c and generate protobuf code

CLIENT_TARGET = client

CLIENT_FILE = client-c.c

PROTO_FILE = sample.proto
PROTO_DIR = .

# Generated c code from proto file
PROTO_C_FILES = sample.pb-c.c
PROTO_C_HEADERS = sample.pb-c.h
PROTO_PYTHON_FILES = sample_pb2.py

# Command for protobuf code generation
PROTOC_C_CMD = protoc-c --c_out=.  --proto_path=$(PROTO_DIR) $(PROTO_FILE)
PROTOC_PYTHON_CMD = protoc --python_out=. --proto_path=$(PROTO_DIR) $(PROTO_FILE)



CFLAGS = -Wall -Wextra -g

# Libraries for protobuf C code
LIBS = `pkg-config --cflags --libs 'libprotobuf-c'`


all: proto server

proto:
	$(PROTOC_C_CMD)
	$(PROTOC_PYTHON_CMD)

server: $(CLIENT_FILE) $(PROTO_C_FILES)
	$(CC) $(CFLAGS) -o $(CLIENT_TARGET) $(CLIENT_FILE) $(PROTO_C_FILES) $(LIBS)


clean:
	rm -f $(CLIENT_TARGET) $(PROTO_C_FILES) $(PROTO_C_HEADERS) $(PROTO_PYTHON_FILES) 

clean-c:
	rm -f $(CLIENT_TARGET)
