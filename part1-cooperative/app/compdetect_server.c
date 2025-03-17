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

    // Convert JSON object to Configuration struct
    Configuration config;
    if (!json_to_configuration(json, &config)) {
        fprintf(stderr, "FAILED TO CONVERT JSON TO VALID CONFIG OBJECT...\n");
        cJSON_Delete(json);
        close(client_sock);
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // print the configuration parsed from client
    print_configuration(&config);

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
 *
 */
double run_probing_phase(const Configuration *config) {
    fprintf(stderr, "\t***PROBING PHASE STARTED***\n");

    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[config->udp_payload_size];

    // Create UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("FAILED TO CREATE UDP SOCKET.");
        exit(EXIT_FAILURE);
    }

    printf("UDP SOCKET SET UP SUCCESSFULLY.\n");

    // Increase socket receive buffer size
    int rcvbuf_size = MAX_UDP_BUFFER;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvbuf_size, sizeof(rcvbuf_size)) == -1) {
        perror("FAILED TO SET BUFFER SIZE.");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Set socket receive timeout
    struct timeval timeout;
    timeout.tv_sec = config->inter_measure_time + 1;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // Bind socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(config->udp_dst_port);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("FAILED TO BIND TO UDP SOCKET.");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("SERVER BOUND TO UDP PORT %d.\n", config->udp_dst_port);

    // Packet tracking
    int low_entropy_received = 0, high_entropy_received = 0;
    struct timeval low_start, low_end, high_start, high_end, last_received_time, second_to_last_received_time;
    memset(&low_start, 0, sizeof(low_start));
    memset(&low_end, 0, sizeof(low_end));
    memset(&high_start, 0, sizeof(high_start));
    memset(&high_end, 0, sizeof(high_end));

    int in_high_phase = 0;

    while (low_entropy_received < config->udp_packet_count || high_entropy_received < config->udp_packet_count) {
        second_to_last_received_time = last_received_time;
        int received_bytes = recvfrom(sock, buffer, sizeof(buffer), 0,
                                    (struct sockaddr *)&client_addr, &addr_len);

        if (received_bytes > 0) {
            gettimeofday(&last_received_time, NULL);

            // Extract Packet ID
            int packet_id = (buffer[0] << 8) | buffer[1];

            // Check entropy level
            int is_high_entropy = 0;
            for (int i = 2; i < received_bytes; i++) {
                if (buffer[i] != 0) {
                    is_high_entropy = 1;
                    break;
                }
            }

            if (!in_high_phase) {
                if (low_entropy_received == 0) {
                    gettimeofday(&low_start, NULL);
                    printf("LOW ENTRY TRAIN STARTED AT TIMESTAMP: %ld.%06ld\n", low_start.tv_sec, low_start.tv_usec);
                }

                if (is_high_entropy) {
                    printf("ENDING LOW ENTROPY TRAIN BECAUSE A HIGH ENTROPY PACKET WAS DETECTED EARLY.\n");
                    low_end = last_received_time;
                    in_high_phase = 1;
                    continue; 
                } else {
                    low_entropy_received++;
                    low_end = last_received_time;
                }

                if (low_entropy_received == config->udp_packet_count) {
                    printf("LOW ENTROPY TRAIN COMPLETED AT %ld.%06ld\n", low_end.tv_sec, low_end.tv_usec);
                    in_high_phase = 1;
                }

            } else {
                if (is_high_entropy) {
                    if (high_entropy_received == 0) {
                        gettimeofday(&high_start, NULL);
                        printf("HIGH ENTROPY TRAIN STARTED AT %ld.%06ld\n", high_start.tv_sec, high_start.tv_usec);
                    }

                    high_entropy_received++;
                    high_end = last_received_time;

                    if (high_entropy_received == config->udp_packet_count) {
                        printf("HIGH ENTROPY TRAIN COMPLETED AT %ld.%06ld\n", high_end.tv_sec, high_end.tv_usec);
                        break;
                    }

                } else {
                    printf("IGNORING LOW ENTROPY PACKETS DETECTED DURING HIGH ENTROPY TRAIN PHASE (PACKET ID %d)\n", packet_id);
                }
            }

            // Debug output for received packets
            if(config->debug_mode) {
                printf("DEBUG MODE - PACKET ID %d RECEIVED FROM %s:%d (%d BYTES)\n",
                       packet_id, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), received_bytes);
            }

        } else {
            perror("TIMEOUT REACHED OR RECVFROM FAILED.");
            if (in_high_phase) {
                high_end = second_to_last_received_time;
                break;
            } else {
                printf("LOW ENTROPY TRAIN TIMEOUT. ENDING LOW ENTROPY TRAIN.\n");
                in_high_phase = 1;
                low_end = second_to_last_received_time;
            }
        }
    }

    printf("PROBING PHASE COMPLETE. CALCULATING DELTA BETWEEN TIMESTAMPS.\n");

    // Debug timestamps if debug mode is enabled
    if(config->debug_mode) {
        printf("DEBUG MODE -\n");
        printf("  START OF LOW TRAIN: %ld.%06ld\n", low_start.tv_sec, low_start.tv_usec);
        printf("  END OF LOW TRAIN: %ld.%06ld\n", low_end.tv_sec, low_end.tv_usec);
        printf("  START OF HIGH TRAIN: %ld.%06ld\n", high_start.tv_sec, high_start.tv_usec);
        printf("  END OF HIGH TRAIN: %ld.%06ld\n", high_end.tv_sec, high_end.tv_usec);
    }
    
    // Time delta calculation
    double low_entropy_time = (low_end.tv_sec - low_start.tv_sec) + (low_end.tv_usec - low_start.tv_usec) / 1.0e6;
    double high_entropy_time = (high_end.tv_sec - high_start.tv_sec) + (high_end.tv_usec - high_start.tv_usec) / 1.0e6;

    printf("LOW ENTROPY TRAIN DURATION: %.6f SECONDS.\n", low_entropy_time);
    printf("HIGH ENTROPY TRAIN DURATION: %.6f SECONDS.\n", high_entropy_time);

    double time_difference = high_entropy_time - low_entropy_time;

    close(sock);
    
    printf("PROBING PHASE COMPLETED SUCCESSFULLY.\n");

    return time_difference;
}

/**
 *
 */
void run_postprobing_phase(const Configuration *config, const double time_delta) {
    fprintf(stderr, "\t***POST-PROBE PHASE STARTED***\n");

    printf("DELTA BETWEEN LOW AND HIGH ENTROPY TRAIN DURATIONS: %.6f SECONDS\n", time_delta);

    const char *result_msg;
    if (time_delta > MAX_ARRIVAL_TIME_DELTA) {
        result_msg = "Compression detected!";
    } else {
        result_msg = "No compression detected.";
    }

    printf("RESULT FOR CLIENT: %s\n", result_msg);
    printf("WAITING FOR CLIENT ON TCP PORT: %d...\n", config->tcp_post_probe);

    int server_sock = -1, client_sock = -1;

    // Create TCP socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("POST-PROBE SOCKET CREATION FAILED.");
        exit(EXIT_FAILURE);
    }

    // Allow port reuse
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("SETSOCKOPT REUSE ALLOWANCE FAILED.");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Setup address
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(config->tcp_post_probe);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("POST PROBING PHASE BIND FAILED.");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 1) == -1) {
        perror("POST PROBING PHASE LISTEN FAILED.");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) == -1) {
        perror("POST PROBING PHASE ACCEPT FAILED.");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("CLIENT CONNECTED ON TCP PORT: %d.\n", config->tcp_post_probe);

    // Send result message to client
    if (send(client_sock, result_msg, strlen(result_msg), 0) == -1) {
        perror("UNABLE TO SEND RESULT TO CLIENT.");
    } else {
        printf("FINAL RESULT SENT TO CLIENT SUCCESSFULLY: %s\n", result_msg);
    }

    // Cleanup
    close(client_sock);
    close(server_sock);

    fprintf(stderr, "\t***POST-PROBE PHASE COMPLETED SUCCESSFULLY***\n");
}


/*
 * part 1 - server side of the project to determine if compression is detected
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
