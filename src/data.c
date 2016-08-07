#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "log.h"

line_t * line_new(const char *data, line_t *next)
{
    // create and memset to 0
    line_t *line = (line_t *) calloc(sizeof(line_t), 1);

    strncpy(line->data, data, LINE_LENGTH);
    line->next = next;
    return line;
}

void line_del(line_t **line_handle)
{
    if (!*line_handle) {
        logging(ERROR, "trying to delete empty line");
        return;
    }
    free(*line_handle);
    *line_handle = NULL;
}

pair_t * pair_new(const char *key, const int value, pair_t * next)
{
    // create and memset to 0
    pair_t *pair = (pair_t *) calloc(sizeof(pair_t), 1);

    strncpy(pair->key, key, KEY_LENGTH);
    pair->value = value;
    pair->next = next;
    return pair;
}

void pair_del(pair_t **pair_handle)
{
    if (!*pair_handle) {
        logging(ERROR, "trying to delete empty pair");
        return;
    }
    free(*pair_handle);
    *pair_handle = NULL;
}

int pair_cmp(const pair_t *a, const pair_t *b, const order_t order_by)
{
    if (order_by == ORDER_BY_KEY)
        return strncmp(a->key, b->key, KEY_LENGTH);
    if (order_by == ORDER_BY_VALUE)
        return a->value <= b->value;
    return -1;
}

void pair_sort(pair_t **input, const order_t order_by)
{
    pair_t *source = *input;
    pair_t *sorted = NULL;
    pair_t **sorted_handle = NULL;
    pair_t *tmp = NULL;

    if (!source || !source->next) return; // empty list or one item list

    // put the first item into the sorted list
    sorted = source;
    source = source->next;
    sorted->next = NULL;

    while (source) {
        sorted_handle = &sorted;

        // browse through the sorted list to find the next item position
        while (*sorted_handle && pair_cmp(source, *sorted_handle, order_by))
            sorted_handle = &(*sorted_handle)->next;

        // insert the item into the sorted list
        tmp = source;
        source = source->next;
        tmp->next = *sorted_handle;
        *sorted_handle = tmp;
    }

    *input = sorted; // replace with the sorted list
}

pair_t * pair_chain(pair_t **pair_array, int size)
{
    pair_t *sorted = NULL;
    pair_t *current = NULL;
    int i = 0;

    // find the head of the merged list
    while (i < size && !pair_array[i]) i++;

    // no head found
    if (i >= size) return NULL;

    // merge the lists into one list
    current = sorted = pair_array[i++];
    for (;i < size; i++) {
        // browse til' the end of the current list
        while (current->next) {
            current = current->next;
        }

        // append the next pairs list to the merged list
        if (pair_array[i]) current->next = pair_array[i];
    }

    return sorted;
}
