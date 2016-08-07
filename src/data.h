#ifndef DATA_H
#define DATA_H

#define LINE_LENGTH 1024
#define KEY_LENGTH 128

typedef struct _line_t {
    char data[LINE_LENGTH];
    struct _line_t *next;
} line_t;

typedef struct _pair_t {
    char key[KEY_LENGTH];
    int value;
    struct _pair_t *next;
} pair_t;

typedef enum {
    ORDER_BY_KEY,
    ORDER_BY_VALUE
} order_t;

/**
    Create a new line.

    @param data: data of the new line.
    @param next: next line of the list.
    @return new line.
*/
line_t * line_new(const char *data, line_t *next);

/**
    Delete a line.

    @param line_handle: handle to the line to delete.
    @return none.
*/
void line_del(line_t **line_handle);

/**
    Create a new pair.

    @param key: key of the new pair.
    @param value: value of the new pair.
    @param next: next pair of the list.
    @return new pair.
*/
pair_t * pair_new(const char *key, const int value, pair_t * next);

/**
    Delete a pair.

    @param pair_handle: handle to the pair to delete.
    @return none.
*/
void pair_del(pair_t **pair_handle);

/**
    Compare two pairs.

    @param a: first pair.
    @param b: second pair.
    @param order_by: order by key or by value.
    @return 0 if values are in the right order, != 0 if not.
*/
int pair_cmp(const pair_t *a, const pair_t *b, const order_t order_by);

/**
    Sort a list of pairs.

    @param pair_handle: handle to the list to sort.
    @param order_by: order by key or by value.
    @return none.
*/
void pair_sort(pair_t **pair_handle, const order_t order_by);

/**
    Chain multiple pair lists.

    @param pair_array: array that contains the heads of the lists to chain.
    @param size: number of items in the array.
    @return chained list.
*/
pair_t * pair_chain(pair_t **pair_array, int size);

#endif // DATA_H
