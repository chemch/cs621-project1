#include "compdetect.h"
#include <sys/time.h>  
#include <poll.h> 

/**
 *
 * @param port to connect to tcp client on
 */
Configuration run_preprobing_phase(int port) {

    // define socket parameters for the client and server
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // define buffer for receiving data from the client`
    char buffer[DEF_BUFFER_SIZE];

    // create new tcp socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create socket!");
        exit(EXIT_FAILURE);
    }

    // server socket settings
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // try to bind the server to the socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed!");
        exit(EXIT_FAILURE);
    }

    // wait for incoming connection
    if (listen(server_sock, 5) == -1) {
        perror("Listening failed on server!");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    // accept incoming client connection
    if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) == -1) {
        perror("Connection acceptance failed!!");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Client connected using tcp source port %d\n", ntohs(client_addr.sin_port));

    // receive JSON data from client
    const int received_bytes = recv(client_sock, buffer, DEF_BUFFER_SIZE - 1, 0);
    if (received_bytes <= 0) {
        perror("Failed to receive data");
        exit(EXIT_FAILURE);
    }

    // terminate the string
    buffer[received_bytes] = '\0';

    printf("Received Configuration from Client...\n");

    // parse the JSON
    cJSON *json = cJSON_Parse(buffer);

    // validate that the json was parsed successfully
    if (!json) {
        fprintf(stderr, "Error parsing received JSON!!!\n");
        exit(EXIT_FAILURE);
    }

    // Convert JSON object to Configuration struct
    Configuration config;
    if (!json_to_configuration(json, &config)) {
        fprintf(stderr, "Unable to convert received JSON to a Configuration struct...\n");
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

    return config;
}

/**
 *
 */
void run_probing_phase(const Configuration *config) {
    printf("Running Probing Phase...\n");
    printf("Using UDP Destination Port: %d\n", config->udp_dst_port);
    printf("Inter-Measurement Time: %d seconds\n", config->inter_measure_time);

    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[PACKET_SIZE];

    // Create UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("[ERROR] UDP socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("[DEBUG] UDP socket created successfully.\n");

    // Bind socket to the UDP destination port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(config->udp_dst_port);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("[ERROR] Binding failed");
        exit(EXIT_FAILURE);
    }
    
    printf("[DEBUG] Server bound to UDP port %d.\n", config->udp_dst_port);
    printf("Listening for UDP packets on port %d...\n", config->udp_dst_port);

    // Track received packets
    int low_entropy_received = 0, high_entropy_received = 0;
    struct timeval low_start, low_end, high_start, high_end;
    struct timeval last_received_time;

    int low_entropy_started = 0;
    int high_entropy_started = 0;
    int switching_to_high = 0;  // Flag to indicate train transition

    struct pollfd fds;
    fds.fd = sock;
    fds.events = POLLIN;

    int timeout_ms = config->inter_measure_time * 1000;  // Convert seconds to milliseconds

    while ((low_entropy_received < config->udp_packet_count && !high_entropy_started) || 
            high_entropy_received < config->udp_packet_count) {

        printf("[DEBUG] Waiting for packets... (polling for %d ms)\n", timeout_ms);
        fflush(stdout);  // Ensure immediate output

        int poll_result = poll(&fds, 1, timeout_ms);
        
        if (poll_result == 0) {
            // Timeout: No packets received within Inter-Measurement Time
            if (!switching_to_high) {
                printf("[DEBUG] Timeout reached! No packets received for %d seconds. Marking low entropy train as complete.\n",
                       config->inter_measure_time);
                low_end = last_received_time;  // Use last known packet time
                switching_to_high = 1;         // Move to high-entropy train
                gettimeofday(&high_start, NULL);  // Start timing for high-entropy train
            } else {
                printf("[DEBUG] Timeout reached! No packets received for %d seconds. Marking high entropy train as complete.\n",
                       config->inter_measure_time);
                high_end = last_received_time;  // Use last known packet time
                break;  // Probing phase complete
            }
            continue;  // Skip recvfrom() and transition
        } else if (poll_result < 0) {
            perror("[ERROR] poll() failed");
            exit(EXIT_FAILURE);
        }

        printf("[DEBUG] poll() detected incoming data.\n");
        fflush(stdout);

        int received_bytes = recvfrom(sock, buffer, PACKET_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        
        if (received_bytes > 0) {
            gettimeofday(&last_received_time, NULL);  // Update last received packet time

            printf("[DEBUG] Received %d bytes from %s:%d\n",
                   received_bytes, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            fflush(stdout);

            // Extract Packet ID (first 2 bytes of payload)
            int packet_id = (buffer[0] << 8) | buffer[1];

            // Check if packet is high entropy
            int is_high_entropy = 0;
            for (int i = 2; i < received_bytes; i++) {
                if (buffer[i] != 0) {
                    is_high_entropy = 1;
                    break;
                }
            }

            if (!switching_to_high) {
                if (!low_entropy_started) {
                    gettimeofday(&low_start, NULL);  // Capture start time
                    low_entropy_started = 1;
                }

                if (is_high_entropy) {
                    // High entropy detected, transition immediately
                    printf("[DEBUG] WARNING: High entropy packet detected! Ending low-entropy train.\n");
                    low_end = last_received_time;  // Ensure low_end is set
                    switching_to_high = 1;
                    gettimeofday(&high_start, NULL);  // Start high-entropy train timing
                } else {
                    low_entropy_received++;
                    low_end = last_received_time;  // Continuously update last known packet time
                }

                if (low_entropy_received == config->udp_packet_count) {
                    printf("[DEBUG] Low entropy packet train complete.\n");
                    switching_to_high = 1;
                    gettimeofday(&high_start, NULL);  // Start high-entropy train timing
                }
            } 
            
            if (switching_to_high) {
                if (!high_entropy_started) {
                    gettimeofday(&high_start, NULL);  // Ensure start time is captured
                    high_entropy_started = 1;
                }
                high_entropy_received++;
                high_end = last_received_time;  // Continuously update last known packet time

                if (high_entropy_received == config->udp_packet_count) {
                    printf("[DEBUG] High entropy packet train complete.\n");
                    break;  // End loop once all high-entropy packets are received
                }
            }

            printf("Received packet ID %d from %s:%d (%d bytes) - Train %s (%d/%d low, %d/%d high)\n",
                   packet_id, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),
                   received_bytes, 
                   switching_to_high ? "HIGH" : "LOW",
                   low_entropy_received, config->udp_packet_count,
                   high_entropy_received, config->udp_packet_count);
            fflush(stdout);
        } else {
            perror("[ERROR] recvfrom() failed");
        }
    }

    printf("[DEBUG] Probing phase complete! Received all packets or timed out.\n");

    // Ensure last received time is recorded
    if (low_entropy_received > 0 && switching_to_high) {
        low_end = last_received_time;
    }
    if (high_entropy_received > 0) {
        high_end = last_received_time;
    }

    // Calculate time differences
    double low_entropy_time = (low_end.tv_sec - low_start.tv_sec) + (low_end.tv_usec - low_start.tv_usec) / 1.0e6;
    double high_entropy_time = (high_end.tv_sec - high_start.tv_sec) + (high_end.tv_usec - high_start.tv_usec) / 1.0e6;

    printf("Low entropy packet train duration: %.6f seconds\n", low_entropy_time);
    printf("High entropy packet train duration: %.6f seconds\n", high_entropy_time);

    close(sock);
}

/**
 *
 */
void run_postprobing_phase() {


}


/*
 * PART 1 - Server Side: Compression Detection Client/Server Application
*/
int main(const int argc, char *argv[]) {

    // perform input parameter check
    if (argc != 2) {
        fprintf(stderr, "Correct Usage: %s <tcp port>\n", argv[0]);

        // exit with failure
        return EXIT_FAILURE;
    }

    // set the port from the provided command line input, perform validation it is a valid tcp port number
    const int port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Error due to the provided tcp port. Tcp ports must be between 1 and 65535.\n");
        return EXIT_FAILURE;
    }

    // pre-probing phase will allow the server to receive the configuration from the client via a tcp connection
    Configuration configuration = run_preprobing_phase(port);

    // probing phase will allow the server to receive 2 udp packet trains, 1 w/ high entropy and 1 w/ low entropy
    run_probing_phase(&configuration);

    // post-probing phase will allow the server calculate compression detection and communicate that back to the client
    run_postprobing_phase();

    // exit will never hit
    return EXIT_SUCCESS;
}
