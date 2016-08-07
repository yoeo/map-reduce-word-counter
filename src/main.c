#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "counter.h"
#include "log.h"
#include "mapreduce.h"

void print_help(void)
{
    printf(
        "Usage: counter <FILENAME> <NB_THREADS> [--help]\n"
        "Count the number of occurences of each word in a given file.\n"
        "\n"
        "Arguments\n"
        "    FILENAME           the file to process.\n"
        "    NB_THREADS         number of parallel threads.\n"
        "    --help             print this help message\n"
    );
}

int get_arguments(int argc, char **argv, const char **filename, int *nb_threads)
{
    char *garbage = NULL;

    // check arguments number
    if (argc != 3) {
        print_help();
        return -1;
    }

    // handle --help option
    for (int i = 0; i < argc; i++) {
        if (strncmp(argv[i], "--help", 7) == 0) { // length("--help\0") == 7
            print_help();
            return -1;
        }
    }

    // retrieve filename argument
    *filename = argv[1];

    // retrieve nb_threads argument
    errno = 0;
    *nb_threads = (int) strtol(argv[2], &garbage, 10);
    if (errno || *garbage) {
        print_help();
        return -1;
    }
    if (*nb_threads <= 0) {
        *nb_threads = 1;
    }

    return 0; // OK
}

int main (int argc, char **argv)
{
    const char *filename = NULL;
    int nb_threads = 1;
    mapreduce_t *mr;

    if (get_arguments(argc, argv, &filename, &nb_threads))
        return EXIT_FAILURE;

    logging(DEBUG, "run with arguments: %s, %d", filename, nb_threads);

    mr = mr_new(read_line, filename, nb_threads);
    if (!mr) {
        logging(ERROR, "unable to open file \"%s\"", filename);
        return EXIT_FAILURE;
    }

    mr_map(mr, split_words);
    mr_reduce(mr, sum_counts);
    mr_sorted_results(mr, print_words);
    mr_del(&mr);

    return EXIT_SUCCESS;
}
