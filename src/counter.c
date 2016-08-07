#include <ctype.h>
#include <stdio.h>

#include "counter.h"

line_t * read_line(FILE *file)
{
    char buffer[LINE_LENGTH] = "";

    if (fgets(buffer, LINE_LENGTH, file)) return line_new(buffer, NULL);
    return NULL;
}

pair_t * split_words(line_t *line, int *position)
{
    pair_t *result = NULL;
    char c;
    int i = *position;

    // drop the non alphanumerical characters before the word
    do {
        c = line->data[i++];
    } while (c && !isalnum(c) && i < LINE_LENGTH);

    // copy the word
    if (c) {
        result = pair_new("", 1, NULL);
        int start = i;

        do {
            // convert the word to lowercase to avoid duplicates
            result->key[i - start] = tolower(c);
            c = line->data[i++];
        } while (c && isalnum(c) && i < LINE_LENGTH);

        result->key[i - start] = '\0'; // set the end of string
    }

    *position = i;
    return result;
}

pair_t * sum_counts(pair_t **position)
{
    pair_t *current = *position;
    pair_t *result = NULL;

    if (!current) return NULL; // trying to reduce empty list

    result = pair_new(current->key, 0, NULL);

    // browse through the ordered list and sum pairs with same key
    while (current && !pair_cmp(current, result, ORDER_BY_KEY)) {
        result->value += current->value;
        current = current->next;
    }
    *position = current; // set the position of the next element to process

    return result;
}

void print_words(pair_t *pair)
{
    printf("%s=%d\n", pair->key, pair->value);
}
