#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "log.h"
#include "mapreduce.h"

typedef struct {
    int i;
    mapreduce_t *mr;
    map_fn map_function;
} map_thread_arg_t;

typedef struct {
    int i;
    mapreduce_t *mr;
    reduce_fn reduce_function;
} reduce_thread_arg_t;

void mr_read_lines(mapreduce_t *mr, read_fn read_function, FILE * file)
{
    line_t *line = NULL;
    int i = 0;
    while ((line = read_function(file))) {
        line->next = mr->map_input[i];
        mr->map_input[i] = line;

        logging(DEBUG, "line: \"%s\"", line->data);

        i = (i + 1) % mr->nb_threads;
    }
}

mapreduce_t * mr_new(read_fn read_function, const char * filename, int nb_threads)
{
    FILE *file = NULL;
    mapreduce_t * mr = NULL;

    // do not create a mapreduce if there is no thread to launch
    if (!nb_threads) return NULL;

    // do not create a mapreduce if there is no file to load
    file = fopen(filename, "r");
    if (!file) return NULL;

    mr = (mapreduce_t *) malloc(sizeof(mapreduce_t));
    mr->nb_threads = nb_threads;

    // create empty pointers to map / reduce inputs and output
    mr->map_input = (line_t **) calloc(sizeof(line_t *), nb_threads);
    mr->reduce_input = (pair_t **) calloc(sizeof(pair_t *), nb_threads);
    mr->combine_input = (pair_t **) calloc(sizeof(pair_t *), mr->nb_threads);
    mr->result = NULL;

    mr_read_lines(mr, read_function, file);
    fclose(file);

    return mr;
}

void mr_del(mapreduce_t **mr_handle)
{
    mapreduce_t *mr = *mr_handle;
    pair_t *current = NULL;
    pair_t *next = NULL;

    line_t *line = NULL;
    line_t *next_line = NULL;

    // clean the result lists
    for (current = mr->result; current; current = next) {
        next = current->next;
        pair_del(&current);
    }

    // clean intermediate data when mr is deleted before map & reduce operations
    for (int i = 0; i < mr->nb_threads; i++) {
        if (mr->map_input[i]) {
            logging(DEBUG, "not empty map input[%d]", i);
            for (line = mr->map_input[i]; line; line = next_line) {
                next_line = line->next;
                line_del(&line);
            }
        }
        if (mr->reduce_input[i]) {
            logging(DEBUG, "not empty reduce input[%d]", i);
            for (current = mr->reduce_input[i]; current; current = next) {
                next = current->next;
                pair_del(&current);
            }
        }
        if (mr->combine_input[i]) {
            logging(DEBUG, "not empty combine input[%d]", i);
            for (current = mr->combine_input[i]; current; current = next) {
                next = current->next;
                pair_del(&current);
            }
        }
    }

    // clean mr
    free(mr->map_input);
    free(mr->reduce_input);
    free(mr->combine_input);
    free(mr);

    *mr_handle = NULL;
}

// Map operation

void parallel_map(int id, line_t **input, pair_t **output, map_fn map_function)
{
    line_t *line = *input;
    line_t *next_line = NULL;
    pair_t *result = NULL;
    pair_t *current = NULL;
    int position = 0;

    // apply map function to each line
    while (line) {
        position = 0;

        while ((current = map_function(line, &position))) {
            logging(
                DEBUG, "mapper[%d] produces <%s, %d>",
                id, current->key, current->value);

            current->next = result;
            result = current;
        }

        // delete mapped line
        next_line = line->next;
        line_del(&line);
        line = next_line;
    }

    *input = NULL;
    *output = result;
}

void * map_thread_wrapper(void *thread_arg)
{
    map_thread_arg_t *arg = (map_thread_arg_t *) thread_arg;
    int i = arg->i;
    mapreduce_t *mr = arg->mr;
    map_fn map_function = arg->map_function;

    parallel_map(i, &mr->map_input[i], &mr->reduce_input[i], map_function);
    return NULL;
}

void mr_map(mapreduce_t *mr, map_fn map_function)
{
    pthread_t *thread_list = (pthread_t *) calloc(
        sizeof(pthread_t), mr->nb_threads);
    map_thread_arg_t *arg_list = (map_thread_arg_t *) calloc(
        sizeof(map_thread_arg_t), mr->nb_threads);

    // launch all threads
    for (int i = 0; i < mr->nb_threads; i++) {
        arg_list[i].i = i;
        arg_list[i].mr = mr;
        arg_list[i].map_function = map_function;

        pthread_create(
            &thread_list[i], NULL, map_thread_wrapper, (void *) &arg_list[i]);
    }

    // join all threads
    for (int i = 0; i < mr->nb_threads; i++)
        pthread_join(thread_list[i], NULL);

    free(arg_list);
    free(thread_list);
}

// Reduce operation

void parallel_reduce(
    int id, pair_t **input, pair_t **output, reduce_fn reduce_function)
{
    pair_t *position = *input;
    pair_t *result = NULL;
    pair_t *current = NULL;
    pair_t *tmp = NULL;

    // sort pairs list
    pair_sort(input, ORDER_BY_KEY);
    logging(DEBUG, "reducer[%d] input sorted by key", id);

    position = *input;

    // apply reduce function to pairs that have the same key
    while ((current = reduce_function(&position))) {
        logging(
            DEBUG, "reducer[%d] produces <%s, %d>",
            id, current->key, current->value);

        current->next = result;
        result = current;
    }

    // free the reducer input list
    current = *input;
    while (current) {
        tmp = current->next;
        pair_del(&current);
        current = tmp;
    }
    *input = NULL;

    *output = result;
}

void * reduce_thread_wrapper(void *thread_arg)
{
    reduce_thread_arg_t *arg = (reduce_thread_arg_t *) thread_arg;
    int i = arg->i;
    mapreduce_t *mr = arg->mr;
    reduce_fn reduce_function = arg->reduce_function;

    parallel_reduce(
        i, &mr->reduce_input[i], &mr->combine_input[i], reduce_function);
    return NULL;
}

void mr_combine(mapreduce_t *mr, reduce_fn reduce_function)
{
    pair_t *result = NULL;

    // merge the partial results
    result = pair_chain(mr->combine_input, mr->nb_threads);

    // clean combine input, the inputs are now in result list
    for (int i = 0; i < mr->nb_threads; i++) mr->combine_input[i] = NULL;
    logging(DEBUG, "combined reducers outputs");

    // reduce one last time
    parallel_reduce(0, &result, &mr->result, reduce_function);
}

void mr_reduce(mapreduce_t *mr, reduce_fn reduce_function)
{
    pthread_t *thread_list = (pthread_t *) calloc(
        sizeof(pthread_t), mr->nb_threads);
    reduce_thread_arg_t *arg_list = (reduce_thread_arg_t *) calloc(
        sizeof(reduce_thread_arg_t), mr->nb_threads);

    // reduce each input list
    for (int i = 0; i < mr->nb_threads; i++) {
        arg_list[i].i = i;
        arg_list[i].mr = mr;
        arg_list[i].reduce_function = reduce_function;

        pthread_create(
            &thread_list[i], NULL, reduce_thread_wrapper,
            (void *) &arg_list[i]);
    }

    for (int i = 0; i < mr->nb_threads; i++)
        pthread_join(thread_list[i], NULL);

    free(thread_list);
    free(arg_list);

    mr_combine(mr, reduce_function);
}

// Access resuts

void mr_sorted_results(mapreduce_t *mr, print_fn print_function)
{
    pair_t * current;
    pair_sort(&mr->result, ORDER_BY_VALUE);
    for (current = mr->result; current; current = current->next)
        print_function(current);
}
