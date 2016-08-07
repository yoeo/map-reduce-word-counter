#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/counter.h"

void test_read_line(void)
{
    FILE *file = fopen("lorem_ipsum.txt", "r");
    line_t *line = read_line(file);
    fclose(file);

    assert(!strncmp(line->data, "Lorem ipsum", 10)); // first line read

    line_del(&line);
}

void test_split_words(void)
{
    pair_t *word = NULL;
    int position = 0;
    line_t *line = line_new("/!\\ :-) word1||word2~", NULL);

    word = split_words(line, &position);
    assert(!strncmp(word->key, "word1", KEY_LENGTH)); // first word extracted
    assert(word->value == 1); // first word weight
    assert(position == 14); // first word end
    pair_del(&word);

    word = split_words(line, &position);
    assert(!strncmp(word->key, "word2", KEY_LENGTH)); // second word extracted
    assert(word->value == 1); // second word weight
    assert(position == 21); // second word end
    pair_del(&word);

    line_del(&line);
}

void test_sum_counts(void)
{
    pair_t *word = NULL;
    pair_t *c = pair_new("xyz", 3, NULL);
    pair_t *b = pair_new("abc", 2, c);
    pair_t *a = pair_new("abc", 7, b);
    pair_t *position = a;

    word = sum_counts(&position);
    assert(!strncmp(word->key, "abc", KEY_LENGTH)); // word key matches
    assert(word->value == 9); // sum matches
    assert(position == c); // next item

    pair_del(&word);
    pair_del(&a);
    pair_del(&b);
    pair_del(&c);
}

void test_print_words(void)
{
    pair_t *word = pair_new("abc", 2, NULL);

    print_words(word);
    assert(!strncmp(word->key, "abc", KEY_LENGTH)); // word key not modified
    assert(word->value == 2); // word value not modified

    pair_del(&word);
}

int main(void)
{
    // run tests on counter
    test_read_line();
    test_split_words();
    test_sum_counts();
    test_print_words();

    // The program aborts before this line if at least one test / assert fails!
    printf("4 tests.....................OK\n");
    return 0;
}
