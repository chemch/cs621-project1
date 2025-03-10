#include "compdetect.h"
#include "compdetect_json.h"

char *configuration_to_json(Configuration *config) {
    if (!config) {
        return NULL;
    }

    // Create JSON object
    cJSON *json = cJSON_CreateObject();
    if (!json) {
        return NULL;
    }

    // Add configuration values
    cJSON_AddStringToObject(json, "ServerIP", config->server_ip);
    cJSON_AddNumberToObject(json, "UDPSourcePort", config->udp_src_port);
    cJSON_AddNumberToObject(json, "UDPDestinationPort", config->udp_dst_port);
    cJSON_AddNumberToObject(json, "TCPSYNX", config->tcp_syn_x);
    cJSON_AddNumberToObject(json, "TCPSYNY", config->tcp_syn_y);
    cJSON_AddNumberToObject(json, "TCPPreProbePort", config->tcp_pre_probe);
    cJSON_AddNumberToObject(json, "TCPPostProbePort", config->tcp_post_probe);
    cJSON_AddNumberToObject(json, "UDPPayloadSize", config->udp_payload_size);
    cJSON_AddNumberToObject(json, "InterMeasureTime", config->inter_measure_time);
    cJSON_AddNumberToObject(json, "UDPPacketCount", config->udp_packet_count);
    cJSON_AddNumberToObject(json, "TTL", config->ttl);

    // Convert JSON object to string
    char *json_string = cJSON_PrintUnformatted(json);

    // Free the JSON object (string must be freed separately after use)
    cJSON_Delete(json);

    return json_string;  // Caller must free this memory
}

/**
 *
 * @param configuration
 */
void forward_configuration_to_server(Configuration *configuration) {
    int sock;
    struct sockaddr_in server_addr;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(configuration->tcp_pre_probe);
    if (inet_pton(AF_INET, configuration->server_ip, &server_addr.sin_addr) != 1) {
        perror("Invalid server IP address format");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Convert configuration to JSON
    char *json_data = configuration_to_json(configuration);
    if (!json_data) {
        perror("Failed to convert configuration to JSON");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Send JSON configuration
    if (send(sock, json_data, strlen(json_data), 0) == -1) {
        perror("Failed to send configuration data");
        free(json_data);
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Configuration file sent successfully to %s:%d\n", configuration->server_ip, configuration->tcp_pre_probe);

    // Clean up
    free(json_data);
    close(sock);
}

/*
 * PART 1 - Client Side: Compression Detection Client/Server Application
*/
int main(const int argc, char *argv[]) {

    // Perform input parameter check
    if (argc != 2) {
        fprintf(stderr, "Correct Usage: %s <configuration.json>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Read configuration file
    char *configuration_file = argv[1];
    fprintf(stderr, "Configuration File: %s\n\n", configuration_file);
    Configuration configuration = read_configuration(configuration_file);

    // Validate that server ip and port are valid
    if (configuration.server_ip[0] == '\0' || configuration.tcp_pre_probe <= 0) {
        fprintf(stderr, "Error: Invalid Server Configuration Read from Config\n");
        return EXIT_FAILURE;
    }

    // Print configuration for validation
    print_configuration(&configuration);

    // Send the configuration to the server
    forward_configuration_to_server(&configuration);

    return EXIT_SUCCESS;
}
