#include "compdetect.h"
#include <sys/time.h>  

#define MAX_UDP_BUFFER 2500000 
#define MAX_ARRIVAL_TIME_DELTA .1

/**
 *
 * @param port to connect to tcp client on
 * @return configuration struct resulting from parsing
 * @brief This function runs the pre-probing phase of the application. It sets up a TCP server
 *        that listens for incoming connections from the client. Once a connection is established,
 *        it receives a JSON configuration string from the client, parses it, and converts it into
 *        a Configuration struct.
 */
Configuration run_preprobing_phase(int port) {
    fprintf(stderr, "\t***PRE-PROBING PHASE STARTED***\n");

    // define socket parameters for the client and server
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // define buffer for receiving data from the client
    char buffer[DEF_BUFFER_SIZE];

    // create new tcp socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("FAILED TO CREATE SERVER SOCKET.");
        exit(EXIT_FAILURE);
    }

    // server socket settings
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // try to bind the server to the socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("FAILED TO BIND SERVER SOCKET.");
        exit(EXIT_FAILURE);
    }

    // wait for incoming connection
    if (listen(server_sock, 5) == -1) {
        perror("FAILED TO LISTEN ON SERVER SOCKET.");
        exit(EXIT_FAILURE);
    }

    printf("LISTENING ON SERVER (DESTINATION) PORT %d...\n", port);

    // accept incoming client connection
    if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) == -1) {
        perror("FAILED TO ACCEPT CLIENT CONNECTION.");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "CLIENT CONNECTED FROM SOURCE PORT %d\n", ntohs(client_addr.sin_port));

    // receive json data from client
    const int received_bytes = recv(client_sock, buffer, DEF_BUFFER_SIZE - 1, 0);
    if (received_bytes <= 0) {
        perror("FAILED TO RECEIVE DATA FROM CLIENT.");
        close(client_sock);
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // terminate the string
    buffer[received_bytes] = '\0';

    printf("SUCCESSFULLY RECEIVED CONFIGURATION FROM CLIENT...\n");

    // parse the JSON
    cJSON *json = cJSON_Parse(buffer);

    // validate that the json was parsed successfully
    if (!json) {
        fprintf(stderr, "UNABLE TO PARSE JSON!!\n");
        exit(EXIT_FAILURE);
    }

    // convert json object to Configuration struct
    Configuration config;
    if (!json_to_configuration(json, &config)) {
        fprintf(stderr, "FAILED TO CONVERT JSON TO VALID CONFIG OBJECT...\n");
        cJSON_Delete(json);
        close(client_sock);
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // free the memory allocated for the json object
    cJSON_Delete(json);

    // close the handles to the client and server sockets
    shutdown(client_sock, SHUT_RDWR);  // Disables further sends/receives
    close(client_sock);
    close(server_sock);

    fprintf(stderr, "\t***PRE-PROBE PHASE COMPLETED SUCCESSFULLY***\n");

    return config;
}

/**
 * begin the probing phase of the application
 * @param config the configuration struct
 * @return the time delta between the high and low entropy trains
 * @brief This function runs the probing phase of the application. It sets up a UDP server
 *       that listens for incoming packets from the client. It receives a specified number
 *      of packets with low entropy and high entropy, and calculates the time delta between
 *     the two trains.
 */
double run_probing_phase(const Configuration *config) {
    fprintf(stderr, "\n\t***PROBING PHASE STARTED***\n");

    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[config->udp_payload_size];

    // create udp socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("FAILED TO CREATE UDP SOCKET.");
        exit(EXIT_FAILURE);
    }

    // increase receive buffer size to reduce packet loss
    int rcvbuf_size = MAX_UDP_BUFFER;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvbuf_size, sizeof(rcvbuf_size)) == -1) {
        perror("FAILED TO SET BUFFER SIZE.");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // set socket receive timeout to prevent blocking
    struct timeval timeout;
    timeout.tv_sec = config->inter_measure_time + 1;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // set server address and port for socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(config->udp_dst_port);

    // bind socket to server address
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("FAILED TO BIND TO UDP SOCKET.");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // set up variables for tracking the number of packets received
    int low_entropy_received = 0, high_entropy_received = 0;
    struct timeval low_start, low_end, high_start, high_end, latest_received_timestamp, second_last_timestamp;
    memset(&low_start, 0, sizeof(low_start));
    memset(&low_end, 0, sizeof(low_end));
    memset(&high_start, 0, sizeof(high_start));
    memset(&high_end, 0, sizeof(high_end));

    // default to low entropy phase
    int in_high_phase = 0;

    // begin receiving packets and tracking the time of arrival
    while (low_entropy_received < config->udp_packet_count || high_entropy_received < config->udp_packet_count) {
        
        // second to last packet is used when the application has an error, it will use the last valid packet in that train
        second_last_timestamp = latest_received_timestamp;

        // receive packet from client
        int received_bytes = recvfrom(sock, buffer, sizeof(buffer), 0,
                                    (struct sockaddr *)&client_addr, &addr_len);

        // on packet receive, update the last received time                            
        if (received_bytes > 0) {
            gettimeofday(&latest_received_timestamp, NULL);

            // extract packet id in first two bytes
            int packet_id = (buffer[0] << 8) | buffer[1];

            // check entropy level of last received packet
            int is_high_entropy = 0;
            for (int i = 2; i < received_bytes; i++) {
                if (buffer[i] != 0) {
                    is_high_entropy = 1;
                    break;
                }
            }

            // check if the packet is in the low or high entropy phase
            if (!in_high_phase) {

                // start the low entropy train if this is the first packet in low phase
                if (low_entropy_received == 0) {
                    gettimeofday(&low_start, NULL);
                    printf("LOW ENTRY TRAIN STARTED AT TIMESTAMP: %ld.%06ld\n", low_start.tv_sec, low_start.tv_usec);
                }

                // if a high entropy packet is received during the low entropy phase, end the low entropy phase
                if (is_high_entropy) {
                    printf("ENDING LOW ENTROPY TRAIN BECAUSE A HIGH ENTROPY PACKET WAS DETECTED EARLY.\n");

                    // set the low end to the timestamp of the second to last packet which was the last valid low entropy packet
                    low_end = second_last_timestamp;
                    in_high_phase = 1;
                    continue; 
                } else {

                    // increment the number of low entropy packets received
                    low_entropy_received++;
                    low_end = latest_received_timestamp;
                }

                // if the low entropy train is complete, end the low entropy phase
                if (low_entropy_received == config->udp_packet_count) {
                    printf("LOW ENTROPY TRAIN COMPLETED AT %ld.%06ld\n", low_end.tv_sec, low_end.tv_usec);
                    in_high_phase = 1;
                }

            } else {

                // start the high entropy train if this is the first packet in high phase
                if (high_entropy_received == 0) {
                    gettimeofday(&high_start, NULL);
                    printf("HIGH ENTROPY TRAIN STARTED AT %ld.%06ld\n", high_start.tv_sec, high_start.tv_usec);
                }

                // increement the number of high entropy packets received and record the end time of last high packet
                high_entropy_received++;
                high_end = latest_received_timestamp;

                // once the high entropy train is complete, end the high entropy phase
                if (high_entropy_received == config->udp_packet_count) {
                    printf("HIGH ENTROPY TRAIN COMPLETED AT %ld.%06ld\n", high_end.tv_sec, high_end.tv_usec);
                    break;
                }
            
            }

            // debug output for received packets
            if(config->debug_mode) {
                printf("DEBUG MODE - PACKET ID %d RECEIVED FROM %s:%d (%d BYTES)\n",
                       packet_id, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), received_bytes);
            }

        } else {

            // if the packet receive fails, print an error message and break the loop
            perror("TIMEOUT REACHED OR RECVFROM FAILED.");

            // set the high end to the second to last packet which was the last valid high entropy packet
            if (in_high_phase) {
                high_end = second_last_timestamp;
                break;
            } else {

                // set the low end to the second to last packet which was the last valid low entropy packet
                printf("LOW ENTROPY TRAIN TIMEOUT. ENDING LOW ENTROPY TRAIN.\n");
                in_high_phase = 1;
                low_end = second_last_timestamp;
            }
        }
    }

    printf("UDP PROBING COMPLETE. CALCULATING DELTA BETWEEN TIMESTAMPS.\n");
    
    // perform time delta calculation
    double low_entropy_time = (low_end.tv_sec - low_start.tv_sec) + (low_end.tv_usec - low_start.tv_usec) / 1.0e6;
    double high_entropy_time = (high_end.tv_sec - high_start.tv_sec) + (high_end.tv_usec - high_start.tv_usec) / 1.0e6;
    double time_difference = high_entropy_time - low_entropy_time;

    printf("LOW ENTROPY TRAIN DURATION: %.6f SECONDS.\n", low_entropy_time);
    printf("HIGH ENTROPY TRAIN DURATION: %.6f SECONDS.\n", high_entropy_time);

    close(sock);
    
    fprintf(stderr, "\t***PROBING PHASE COMPLETED SUCCESSFULLY***\n");

    return time_difference;
}

/**
 * @param config the configuration struct
 * @param time_delta the time delta between the high and low entropy trains
 * @brief This function runs the post-probing phase of the application. It communicates with the client
 *       to send the result of the compression detection based on the time delta between the high and low
 *      entropy trains.
 * @note The result message is sent to the client via a TCP connection.
 */
void run_postprobing_phase(const Configuration *config, const double time_delta) {
    fprintf(stderr, "\n\t***POST-PROBING PHASE STARTED***\n");

    printf("DELTA BETWEEN LOW AND HIGH ENTROPY TRAIN DURATIONS: %.6f SECONDS\n", time_delta);

    // determine if compression was detected based on the time delta
    const char *result_msg;
    if (time_delta > MAX_ARRIVAL_TIME_DELTA) {
        result_msg = "Compression detected!";
    } else {
        result_msg = "No compression detected.";
    }

    printf("WAITING FOR CLIENT ON TCP PORT: %d...\n", config->tcp_post_probe);
    int server_sock = -1, client_sock = -1;

    // create tcp socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("POST-PROBE SOCKET CREATION FAILED.");
        exit(EXIT_FAILURE);
    }

    // to prevent failures on bind due to address already in use
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("SETSOCKOPT REUSE ALLOWANCE FAILED.");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // set tcp server address and port
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(config->tcp_post_probe);

    // bind the server to the socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("POST PROBING PHASE BIND FAILED.");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // listen for incoming connections from client
    if (listen(server_sock, 1) == -1) {
        perror("POST PROBING PHASE LISTEN FAILED.");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // accept client connection
    if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) == -1) {
        perror("POST PROBING PHASE ACCEPT FAILED.");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("CLIENT CONNECTED ON TCP PORT: %d.\n", config->tcp_post_probe);

    // send the final result message to client
    if (send(client_sock, result_msg, strlen(result_msg), 0) == -1) {
        perror("UNABLE TO SEND RESULT TO CLIENT.");
    } else {
        printf("FINAL RESULT SENT TO CLIENT SUCCESSFULLY: %s\n", result_msg);
    }

    // close the client and server sockets
    close(client_sock);
    close(server_sock);

    fprintf(stderr, "\t***POST-PROBE PHASE COMPLETED SUCCESSFULLY***\n");
}


/**
* part 1 - server side of the project to determine if compression is detected
* @param argc the number of command line arguments
* @param argv the command line arguments
* @return the exit status of the program
* @brief This function is the entry point for the server side of the application. It runs the pre-probing,
*       probing, and post-probing phases of the application to determine if compression is detected.
*      The server listens for incoming connections from the client and receives configuration data
*    and UDP packet trains from the client. It then calculates the time delta between the high and low
*  entropy trains and communicates the result back to the client.
* @note The server is run with the command line arguments: <TCP PORT>
*/
int main(const int argc, char *argv[]) {

    // perform input parameter check
    if (argc != 2) {
        fprintf(stderr, "CORRECT USAGE: %s <TCP PORT>\n", argv[0]);

        // exit with failure
        return EXIT_FAILURE;
    }

    // set the port from the provided command line input, perform validation it is a valid tcp port number
    const int port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "INVALID TCP PORT PROVIDED. VALUE MUST BE BETWEEN 0 AND 65535.\n");
        return EXIT_FAILURE;
    }

    // pre-probing phase will allow the server to receive the configuration from the client via a tcp connection
    Configuration configuration = run_preprobing_phase(port);

    // probing phase will allow the server to receive 2 udp packet trains, 1 w/ high entropy and 1 w/ low entropy
    double delta = run_probing_phase(&configuration);

    // post-probing phase will allow the server calculate compression detection and communicate that back to the client
    run_postprobing_phase(&configuration, delta);

    return EXIT_SUCCESS;
}
