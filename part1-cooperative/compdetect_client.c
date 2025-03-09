#include "compdetect.h"
#include "compdetect_json.h"

/*
 * PART 1: Compression Detection Client/Server Application
*/
int main(const int argc, char *argv[]) {

    // perform input parameter check
    if (argc != 2) {
        fprintf(stderr, "Correct Usage: %s <configuration.json>\n", argv[0]);

        // exit with failure
        return EXIT_FAILURE;
    }

    // parse configuration
    char *config_file_name = argv[1];
    fprintf(stderr, "Parsing Configuration File: %s\n", config_file_name);
    const Config config = read_config(config_file_name);
    print_config(&config);

    return EXIT_SUCCESS;
}
