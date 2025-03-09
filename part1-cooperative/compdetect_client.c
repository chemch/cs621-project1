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

    // read configuration file and print to terminal for validation
    char *configuration_file = argv[1];
    fprintf(stderr, "Configuration File: %s\n\n", configuration_file);
    const Configuration config = read_configuration(configuration_file);
    print_configuration(&config);

    return EXIT_SUCCESS;
}
