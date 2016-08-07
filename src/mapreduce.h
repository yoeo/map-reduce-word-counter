#ifndef MAPREDUCE_H_
#define MAPREDUCE_H_

#include <stdio.h>

#include "data.h"

typedef struct {
    int nb_threads;
    line_t **map_input;
    pair_t **reduce_input;
    pair_t **combine_input;
    pair_t *result;
} mapreduce_t;

typedef line_t * (*read_fn)(FILE *file);
typedef pair_t * (*map_fn)(line_t *line, int *position);
typedef pair_t * (*reduce_fn)(pair_t **position);
typedef void (*print_fn)(pair_t *pair);

/**
    Create a new MapReduce.

    @param read_function: function that reads a file line by line.
    @param filename: name of the input file.
    @param nb_threads: number of parallel workers.
    @return new MapReduce.
*/
mapreduce_t * mr_new(
    read_fn read_function, const char * filename, int nb_threads);

/**
    Delete a MapReduce.

    @param mr_handle: handle to the MapReduce to delete.
    @return none.
*/
void mr_del(mapreduce_t **mr_handle);

/**
    Launch a Map operation.

    @param mr: a MapReduce.
    @param map_function: mapper function.
    @return none.
*/
void mr_map(mapreduce_t *mr, map_fn map_function);

/**
    Launch a Reduce operation.

    @param mr: a MapReduce.
    @param reduce_function: reducer function.
    @return none.
*/
void mr_reduce(mapreduce_t *mr, reduce_fn reduce_function);

/**
    Sort and print the MapReduce result.

    @param mr: a MapReduce.
    @param print_function: pair printer function.
    @return none.
*/
void mr_sorted_results(mapreduce_t *mr, print_fn print_function);

#endif // MAPREDUCE_H_
