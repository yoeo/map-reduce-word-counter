#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "../src/counter.h"
#include "../src/mapreduce.h"

int rand_range(int start, int end)
{
    return start + rand() % end;
}

int count(pair_t *current)
{
    int nb_items = 0;
    while (current) {
        nb_items++;
        current = current->next;
    }
    return nb_items;
}

void test_mr_new(void)
{
    mapreduce_t *mr = NULL;
    int nb_threads = rand_range(1, 20);

    mr = mr_new(read_line, "__missing_file__", nb_threads);
    assert(!mr); // mapreduce not created, bad filename

    mr = mr_new(read_line, "lorem_ipsum.txt", 0);
    assert(!mr); // mapreduce not created, bad thread number

    mr = mr_new(read_line, "lorem_ipsum.txt", nb_threads);
    assert(mr); // mapreduce created
    assert(mr->nb_threads == nb_threads); // nb_threads matches

    mr_del(&mr);
}

void test_mr_map(void)
{
    int nb_threads = rand_range(1, 20);
    mapreduce_t *mr = mr_new(read_line, "lorem_ipsum.txt", nb_threads);
    int nb_items = 0;

    mr_map(mr, split_words);
    for (int i = 0; i < mr->nb_threads; i++) {
        nb_items += count(mr->reduce_input[i]);
    }

    assert(nb_items == 490); // number of words in lorem_ipsum.txt

    mr_del(&mr);
}

void test_mr_reduce(void)
{
    int nb_threads = rand_range(1, 20);
    mapreduce_t *mr = mr_new(read_line, "lorem_ipsum.txt", nb_threads);
    int nb_items = 0;

    mr_map(mr, split_words);

    mr_reduce(mr, sum_counts);
    nb_items += count(mr->result);
    assert(nb_items == 145); // number of distinct words in lorem_ipsum.txt

    mr_del(&mr);
}

void assert_is_sorted(pair_t *word)
{
    static int old_value = 0;
    if (!old_value) {
        old_value = word->value;
        return;
    }

    assert(word->value <= old_value); // the list is sorted by value
    old_value = word->value;
}

void test_mr_result(void)
{
    int nb_threads = rand_range(1, 20);
    mapreduce_t *mr = mr_new(read_line, "lorem_ipsum.txt", nb_threads);

    mr_map(mr, split_words);
    mr_reduce(mr, sum_counts);
    mr_sorted_results(mr, assert_is_sorted); // sorted list

    mr_del(&mr);
}

int main(void)
{
    srand(time(0));

    // run tests on map reduce
    test_mr_new();
    test_mr_map();
    test_mr_reduce();
    test_mr_result();

    // The program aborts before this line if at least one test / assert fails!
    printf("4 tests.....................OK\n");
    return 0;
}
