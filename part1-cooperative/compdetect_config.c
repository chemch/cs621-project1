#include "compdetect.h"

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
/**
 *
 * @param configuration_file
 * @return parsed configuration struct from the input file
 */
Configuration read_configuration(const char *configuration_file) {

    // create a configuration object with the default values (in case some attributes are not provided in the file)
    Configuration configuration = {
        .udp_src_port = DEF_UDP_SRC_PORT,
        .udp_dst_port = DEF_UDP_DST_PORT,
        .tcp_syn_x = DEF_TCP_SYN_X,
        .tcp_syn_y = DEF_TCP_SYN_Y,
        .tcp_pre_probe = DEF_TCP_PRE_PROBE,
        .tcp_post_probe = DEF_TCP_POST_PROBE,
        .udp_payload_size = DEF_UDP_PAYLOAD_SIZE,
        .inter_measure_time = DEF_INTER_MEASURE_TIME,
        .phase_transition_time = DEF_PHASE_TRANSITION_TIME,
        .udp_packet_count = DEF_UDP_PACKET_COUNT,
        .ttl = DEF_UDP_TTL
    };

    // create file stream to read from the input configuration
    FILE *raw_file = fopen(configuration_file, "r");

    // input parameter checking: make sure the file can be opened
    if (!raw_file) {
        perror("Error opening configuration file");
        exit(EXIT_FAILURE);
    }

    // determine file size
    fseek(raw_file, 0, SEEK_END);
    const long filesize = ftell(raw_file);
    rewind(raw_file);

    // read file contents into memory
    char *json_file = malloc(filesize + 1);
    fread(json_file, 1, filesize, raw_file);
    json_file[filesize] = '\0';

    // close the handle to the raw input file
    fclose(raw_file);

    // parse the json file into a cJSON object
    cJSON *json = cJSON_Parse(json_file);

    // validate that the json has been properly parsed to avoid errors later
    if (!json) {
        perror("Unable to parse the provided JSON. Check the Configuration file.");
        exit(EXIT_FAILURE);
    }

    // iterate through the json object and populate the configuration object with the values
    cJSON *json_item;

    if ((json_item = cJSON_GetObjectItem(json, "ClientIP")))
    strncpy(configuration.client_ip, json_item->valuestring, sizeof(configuration.client_ip));

    if ((json_item = cJSON_GetObjectItem(json, "ServerIP")))
        strncpy(configuration.server_ip, json_item->valuestring, sizeof(configuration.server_ip));

    if ((json_item = cJSON_GetObjectItem(json, "UDPSourcePort")))
        configuration.udp_src_port = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "UDPDestinationPort")))
        configuration.udp_dst_port = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "TCPSYNX")))
        configuration.tcp_syn_x = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "TCPSYNY")))
        configuration.tcp_syn_y = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "TCPPreProbePort")))
        configuration.tcp_pre_probe = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "TCPPostProbePort")))
        configuration.tcp_post_probe = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "UDPPayloadSize")))
        configuration.udp_payload_size = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "InterMeasureTime")))
        configuration.inter_measure_time = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "PhaseTransitionTime")))
    configuration.phase_transition_time = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "UDPPacketCount")))
        configuration.udp_packet_count = json_item->valueint;

    if ((json_item = cJSON_GetObjectItem(json, "TTL")))
        configuration.ttl = json_item->valueint;

    // free the memory allocated for the json file
    cJSON_Delete(json);
    free(json_file);

    return configuration;
}


// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
/**
 *
 * @param json to convert
 * @param config resulting from conversion
 * @return
 */
int json_to_configuration(cJSON *json, Configuration *config) {
    // input parameter checking, make sure valid config and json
    if (!json || !config) return 0;

    // iterate through the json object and populate the configuration object with the values
    cJSON *item;
    if (((item = cJSON_GetObjectItem(json, "ClientIP"))) && cJSON_IsString(item)) {
        strncpy(config->client_ip, item->valuestring, sizeof(config->client_ip));
    }

    if (((item = cJSON_GetObjectItem(json, "ServerIP"))) && cJSON_IsString(item)) {
        strncpy(config->server_ip, item->valuestring, sizeof(config->server_ip));
    }

    if (((item = cJSON_GetObjectItem(json, "UDPSourcePort"))) && cJSON_IsNumber(item)) {
        config->udp_src_port = item->valueint;
    }

    if (((item = cJSON_GetObjectItem(json, "UDPDestinationPort"))) && cJSON_IsNumber(item)) {
        config->udp_dst_port = item->valueint;
    }

    if (((item = cJSON_GetObjectItem(json, "TCPSYNX"))) && cJSON_IsNumber(item)) {
        config->tcp_syn_x = item->valueint;
    }

    if (((item = cJSON_GetObjectItem(json, "TCPSYNY"))) && cJSON_IsNumber(item)) {
        config->tcp_syn_y = item->valueint;
    }

    if (((item = cJSON_GetObjectItem(json, "TCPPreProbePort"))) && cJSON_IsNumber(item)) {
        config->tcp_pre_probe = item->valueint;
    }

    if (((item = cJSON_GetObjectItem(json, "TCPPostProbePort"))) && cJSON_IsNumber(item)) {
        config->tcp_post_probe = item->valueint;
    }

    if (((item = cJSON_GetObjectItem(json, "UDPPayloadSize"))) && cJSON_IsNumber(item)) {
        config->udp_payload_size = item->valueint;
    }

    if (((item = cJSON_GetObjectItem(json, "InterMeasureTime"))) && cJSON_IsNumber(item)) {
        config->inter_measure_time = item->valueint;
    }

    if (((item = cJSON_GetObjectItem(json, "PhaseTransitionTime"))) && cJSON_IsNumber(item)) {
        config->phase_transition_time = item->valueint;
    }

    if (((item = cJSON_GetObjectItem(json, "UDPPacketCount"))) && cJSON_IsNumber(item)) {
        config->udp_packet_count = item->valueint;
    }

    if (((item = cJSON_GetObjectItem(json, "TTL"))) && cJSON_IsNumber(item)) {
        config->ttl = item->valueint;
    }

    return 1;
}


// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
/**
 *
 * @param config
 */
void print_configuration(const Configuration *config) {
    printf("Configuration Settings:\n");
    printf("======================\n\n");
    printf("Client IP: %s\n", config->client_ip);
    printf("Server IP: %s\n", config->server_ip);
    printf("UDP Source Port: %d\n", config->udp_src_port);
    printf("UDP Destination Port: %d\n", config->udp_dst_port);
    printf("TCP SYN X Port: %d\n", config->tcp_syn_x);
    printf("TCP SYN Y Port: %d\n", config->tcp_syn_y);
    printf("TCP Pre-Probe Port: %d\n", config->tcp_pre_probe);
    printf("TCP Post-Probe Port: %d\n", config->tcp_post_probe);
    printf("UDP Payload Size: %dB\n", config->udp_payload_size);
    printf("Inter-Measurement Time: %d seconds\n", config->inter_measure_time);
    printf("Phase Transition Time: %d seconds\n", config->phase_transition_time);
    printf("Number of UDP Packets: %d\n", config->udp_packet_count);
    printf("UDP TTL: %d\n", config->ttl);
    printf("\n");
}

/**
 *
 * @param config to convert
 * @return converted json object
 */
// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
char *convert_configuration_to_json(Configuration *config) {

    // input parameter checking: confirm config is not empty
    if (!config) {
        return NULL;
    }

    // create empty json object for populating
    cJSON *json_obj = cJSON_CreateObject();

    // Validate that the json object was created successfully
    if (!json_obj) {
        return NULL;
    }

    // Add configuration values to object
    cJSON_AddStringToObject(json_obj, "ClientIP", config->client_ip);
    cJSON_AddStringToObject(json_obj, "ServerIP", config->server_ip);
    cJSON_AddNumberToObject(json_obj, "UDPSourcePort", config->udp_src_port);
    cJSON_AddNumberToObject(json_obj, "UDPDestinationPort", config->udp_dst_port);
    cJSON_AddNumberToObject(json_obj, "TCPSYNX", config->tcp_syn_x);
    cJSON_AddNumberToObject(json_obj, "TCPSYNY", config->tcp_syn_y);
    cJSON_AddNumberToObject(json_obj, "TCPPreProbePort", config->tcp_pre_probe);
    cJSON_AddNumberToObject(json_obj, "TCPPostProbePort", config->tcp_post_probe);
    cJSON_AddNumberToObject(json_obj, "UDPPayloadSize", config->udp_payload_size);
    cJSON_AddNumberToObject(json_obj, "InterMeasureTime", config->inter_measure_time);
    cJSON_AddNumberToObject(json_obj, "PhaseTransitionTime", config->phase_transition_time);
    cJSON_AddNumberToObject(json_obj, "UDPPacketCount", config->udp_packet_count);
    cJSON_AddNumberToObject(json_obj, "TTL", config->ttl);

    // Convert JSON object to string
    char *json_string = cJSON_PrintUnformatted(json_obj);

    // Free the JSON object
    cJSON_Delete(json_obj);

    return json_string;
}

/**
 *
 * @param configuration
 */
// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
void forward_configuration_to_server(Configuration *configuration) {
    int sock;
    struct sockaddr_in server_addr;

    // create tcp socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error: Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // set the server port
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(configuration->tcp_pre_probe);

    // set the server ip and validate
    if (inet_pton(AF_INET, configuration->server_ip, &server_addr.sin_addr) != 1) {
        perror("Error: Invalid Server IP Address");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // initiate connection to server using ip and port
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error: Failed to connect to server");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // convert configuration to json for transmission over the network
    char *json_data = convert_configuration_to_json(configuration);

    // validate that the json was created successfully
    if (!json_data) {
        perror("Error: Failed to convert configuration to JSON");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // forward JSON configuration using the socket
    if (send(sock, json_data, strlen(json_data), 0) == -1) {
        perror("Error: Failed to Forward the Configuration to Server. Check the socket settings");
        free(json_data);
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Successfully sent Config to %s:%d \n", configuration->server_ip, configuration->tcp_pre_probe);

    // release unneeded handles
    free(json_data);
    close(sock);
}
