#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/data.h"

void test_line_new(void)
{
    const char * data = "a long line...";
    line_t *line = line_new(data, NULL);

    assert(line); // line created
    assert(!strncmp(line->data, data, LINE_LENGTH)); // data matches line data
    assert(!line->next); // next matches NULL

    line_del(&line);
}

void test_line_del(void)
{
    line_t *line = line_new("a long line...", NULL);

    line_del(&line);
    assert(!line); // line deleted
}

void test_pair_new(void)
{
    const char * key = "word";
    int value = rand();

    pair_t *pair = pair_new(key, value, NULL);

    assert(pair); // line created
    assert(!strncmp(pair->key, key, LINE_LENGTH)); // key matches line data
    assert(pair->value == value); // pair value matches
    assert(!pair->next); // next matches NULL

    pair_del(&pair);
}

void test_pair_del(void)
{
    pair_t *pair = pair_new("word", 0, NULL);

    pair_del(&pair);
    assert(!pair); // line deleted
}

void test_pair_cmp(void)
{
    pair_t *a = pair_new("", 0, NULL);
    pair_t *b = pair_new("", 0, NULL);

    strncpy(a->key, "aaa", KEY_LENGTH);
    strncpy(b->key, "aaa", KEY_LENGTH);
    assert(!pair_cmp(a, b, ORDER_BY_KEY)); // same key

    strncpy(a->key, "abcd", KEY_LENGTH);
    strncpy(b->key, "xyz", KEY_LENGTH);
    assert(pair_cmp(a, b, ORDER_BY_KEY)); // different keys

    a->value = 2;
    b->value = 1;
    assert(!pair_cmp(a, b, ORDER_BY_VALUE)); // desc ordered values
    assert(pair_cmp(b, a, ORDER_BY_VALUE)); // not desc ordererd values

    pair_del(&a);
    pair_del(&b);
}

void test_pair_sort(void)
{
    pair_t *c = pair_new("a", 3, NULL);
    pair_t *b = pair_new("b", 1, c);
    pair_t *a = pair_new("a", 7, b);
    pair_t **handle = &a;

    pair_sort(handle, ORDER_BY_KEY);
    assert(!strncmp((*handle)->key, "a", KEY_LENGTH)); // key_1 = a
    assert(!strncmp((*handle)->next->key, "a", KEY_LENGTH)); // key_2 = a
    assert(!strncmp((*handle)->next->next->key, "b", KEY_LENGTH)); // key_3 = b

    pair_sort(handle, ORDER_BY_VALUE);
    assert((*handle)->value == 7); // value_1 = 7
    assert((*handle)->next->value == 3); // value_2 = 3
    assert((*handle)->next->next->value == 1); // value_3 = 1

    pair_del(&a);
    pair_del(&b);
    pair_del(&c);
}

void test_pair_chain(void)
{
    pair_t *c = pair_new("c", 2, NULL);
    pair_t *b = pair_new("b", 1, c);
    pair_t *a = pair_new("a", 0, NULL);
    pair_t *pair_list[4] = {NULL, a, NULL, b};

    pair_t * result = pair_chain(pair_list, 4);
    assert(result == a); // pair_1 = a
    assert(result->next == b); // pair_2 = b
    assert(result->next->next == c); // pair_3 = c

    pair_del(&a);
    pair_del(&b);
    pair_del(&c);
}

int main(void)
{
    // run tests on line lists
    test_line_new();
    test_line_del();

    // run tests on pair lists
    test_pair_new();
    test_pair_del();

    // run tests on pair operations: sort, compare, chain
    test_pair_cmp();
    test_pair_sort();
    test_pair_chain();

    // The program aborts before this line if at least one test / assert fails!
    printf("7 tests.....................OK\n");
    return 0;
}
