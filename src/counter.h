#ifndef COUNTER_H_
#define COUNTER_H_

#include <stdio.h>

#include "data.h"

/**
    Read a line from a file.

    @param file: opened file.
    @return line that was read from the file.
*/
line_t *read_line(FILE *file);

/**
    Get a word (contiguous alphanum characters) from a line.

    @param line: line to split.
    @param position: start position.
    @return a word.
*/
pair_t * split_words(line_t *line, int *position);

/**
    Sum the values of pairs that have the same key.

    @param position: sorted pair list.
    @return new pair with the sum as value.
*/
pair_t * sum_counts(pair_t **position);

/**
    Print a pair.

    @param pair: pair to print.
    @return none.
*/
void print_words(pair_t *pair);

#endif // COUNTER_H_
