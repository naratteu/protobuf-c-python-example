#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "sample.pb-c.h"
#include <netinet/in.h>
#include <stdint.h> // For int32_t and uint8_t

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345

void receive_message(int sockfd)
{
    int32_t response_header[2]; // For storing response header
    // u_int8_t response_header[9]; // For storing response header
    recv(sockfd, response_header, sizeof(response_header), 0);

    // Convert message length and type from network byte order to host byte order
    int32_t response_msg_length = ntohl(response_header[0]);
    int32_t response_type = ntohl(response_header[1]);

    printf("Received response header from server: %d, %d\n", response_msg_length, response_type);

    // Allocate memory for the response message payload
    uint8_t *response_payload = (uint8_t *)malloc(response_msg_length);

    // Receive the response message payload
    recv(sockfd, response_payload, response_msg_length, 0);

    // Process the response based on the type
    if (response_type == 1)
    {
        // SampleMessageOne response_message = SAMPLE_MESSAGE_ONE__INIT;
        SampleMessageOne *response_message = sample_message_one__unpack(NULL, response_msg_length, response_payload);

        printf("Received response from server: %d\n", response_message->value);

        // Clean up
        sample_message_one__free_unpacked(response_message, NULL);
    }
    else // Process the response based on the type
        if (response_type == 2)
        {
            // SampleMessageOne response_message = SAMPLE_MESSAGE_ONE__INIT;
            SampleMessageTwo *response_message = sample_message_two__unpack(NULL, response_msg_length, response_payload);

            printf("Received response from server: %s\n", response_message->message);

            // Clean up
            sample_message_two__free_unpacked(response_message, NULL);
        }
        else
        {
            printf("Unknown message type received from server.\n");
        }

    // Clean up
    free(response_payload);
}

void send_sample_message(int32_t type, uint8_t *serialized_message, size_t serialized_size)
{
    // Calculate the total message length including the header
    // int32_t header_size = 2 * sizeof(int32_t) + 1; // Two int32_t and one byte ('@') for the header
    int32_t header_size = 2 * sizeof(int32_t);
    int32_t msg_length = header_size + serialized_size;

    uint8_t *buffer_msg = (uint8_t *)malloc(msg_length);

    // Convert message length and type to network byte order (big-endian)
    // int32_t msg_length_n = htonl(msg_length);
    int32_t serialzed_len = htonl(serialized_size);
    int32_t type_n = htonl(type);

    // Create binary form of header
    memcpy(buffer_msg, &serialzed_len, sizeof(int32_t));
    // buffer_msg[sizeof(int32_t)] = '@';
    // memcpy(buffer_msg + sizeof(int32_t) + 1, &type_n, sizeof(int32_t));
    memcpy(buffer_msg + sizeof(int32_t), &type_n, sizeof(int32_t));

    // Concatenate the serialized message with the header
    memcpy(buffer_msg + header_size, serialized_message, serialized_size);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        free(buffer_msg);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr));

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error connecting to the server");
        free(buffer_msg);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Send the message
    send(sockfd, buffer_msg, msg_length, 0);

    // Receive the response message from the server
    receive_message(sockfd);

    free(buffer_msg);
    close(sockfd);
}

int main()
{
    // Sample Message Type One
    SampleMessageOne message_one = SAMPLE_MESSAGE_ONE__INIT;
    message_one.value = 123;

    // Get the serialized size and create the serialized message buffer
    size_t serialized_size_one = sample_message_one__get_packed_size(&message_one);
    uint8_t *serialized_message_one = (uint8_t *)malloc(serialized_size_one);
    sample_message_one__pack(&message_one, serialized_message_one);

    // Send the Type One message
    send_sample_message(1, serialized_message_one, serialized_size_one);

    free(serialized_message_one);

    // Sample Message Type Two
    SampleMessageTwo message_two = SAMPLE_MESSAGE_TWO__INIT;
    message_two.message = "Hello, Server!";

    // Get the serialized size and create the serialized message buffer
    size_t serialized_size_two = sample_message_two__get_packed_size(&message_two);
    uint8_t *serialized_message_two = (uint8_t *)malloc(serialized_size_two);
    sample_message_two__pack(&message_two, serialized_message_two);

    // Send the Type Two message
    send_sample_message(2, serialized_message_two, serialized_size_two);

    free(serialized_message_two);

    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include "sample.pb-c.h"
// #include <netinet/in.h>
// #include <stdint.h> // For int32_t and uint8_t

// #define SERVER_IP "127.0.0.1"
// #define SERVER_PORT 12345

// void send_sample_message(int32_t type, uint8_t *serialized_message, size_t serialized_size)
// {
//     // Calculate the total message length including the header
//     int32_t header_size = 2 * sizeof(int32_t) + 1; // Two int32_t and one byte ('@') for the header
//     int32_t msg_length = header_size + serialized_size;

//     uint8_t *buffer_msg = (uint8_t *)malloc(msg_length);

//     // Convert message length and type to network byte order (big-endian)
//     int32_t msg_length_n = htonl(msg_length);
//     int32_t type_n = htonl(type);

//     // Create binary form of header
//     memcpy(buffer_msg, &msg_length_n, sizeof(int32_t));
//     buffer_msg[sizeof(int32_t)] = '@';
//     memcpy(buffer_msg + sizeof(int32_t) + 1, &type_n, sizeof(int32_t));

//     // Concatenate the serialized message with the header
//     memcpy(buffer_msg + header_size, serialized_message, serialized_size);

//     int sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd < 0)
//     {
//         perror("Error creating socket");
//         free(buffer_msg);
//         exit(EXIT_FAILURE);
//     }

//     struct sockaddr_in server_addr;
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(SERVER_PORT);
//     inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr));

//     if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
//     {
//         perror("Error connecting to the server");
//         free(buffer_msg);
//         close(sockfd);
//         exit(EXIT_FAILURE);
//     }

//     // Send the message
//     send(sockfd, buffer_msg, msg_length, 0);

//     free(buffer_msg);
//     close(sockfd);
// }

// int main()
// {
//     // Sample Message Type One
//     SampleMessageOne message_one = SAMPLE_MESSAGE_ONE__INIT;
//     message_one.value = 42;

//     // Get the serialized size and create the serialized message buffer
//     size_t serialized_size_one = sample_message_one__get_packed_size(&message_one);
//     uint8_t *serialized_message_one = (uint8_t *)malloc(serialized_size_one);
//     sample_message_one__pack(&message_one, serialized_message_one);

//     // Send the Type One message
//     send_sample_message(0, serialized_message_one, serialized_size_one);

//     free(serialized_message_one);

//     // Sample Message Type Two
//     SampleMessageTwo message_two = SAMPLE_MESSAGE_TWO__INIT;
//     message_two.message = "Hello, Server!";

//     // Get the serialized size and create the serialized message buffer
//     size_t serialized_size_two = sample_message_two__get_packed_size(&message_two);
//     uint8_t *serialized_message_two = (uint8_t *)malloc(serialized_size_two);
//     sample_message_two__pack(&message_two, serialized_message_two);

//     // Send the Type Two message
//     send_sample_message(1, serialized_message_two, serialized_size_two);

//     free(serialized_message_two);

//     return 0;
// }
