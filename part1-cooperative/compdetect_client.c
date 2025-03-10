#include "compdetect.h"
#include "compdetect_config.h"

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
