
# Using CLang for development (more expressive error messages)
LLVMCC = clang-3.5
LLVMFLAGS = -std=c11 -Wall -Wextra -pedantic -g -O3

# Using GCC for distribution
GNUCC = gcc
GNUFLAGS = -std=c11 -Wall -Wextra -pedantic -g -Og

CC = $(GNUCC)
FLAGS = $(GNUFLAGS)
EXTRAFLAGS = -DFLAG_RELEASE
INCLUDE = -Isrc/
LIBS = -lpthread

LD = ld
LDFLAGS = -r

EXECUTABLE = bin/wcount

# build the program
all: main
	@echo "\033[92mBuild SUCCEEDED \033[0m"
	@echo

debug: EXTRAFLAGS =
debug: main
	@echo "\033[92mDebug build SUCCEEDED \033[0m"
	@echo

main: src/main.o src/merged_libs.o
	@$(CC) $(FLAGS) $(INCLUDE) $^ -o $(EXECUTABLE) $(LIBS)
	@echo "Exec built $(EXECUTABLE)"

# cleanup
clean:
	@rm -f bin/* src/*.o src/*.gch tests/*.gch tests/*.o
	@echo "Clean all"

# build and run all the tests
test: memcheck test_mr test_data test_resource test_counter
	@echo
	@echo "\033[93mRunning tests... \033[0m"
	@echo
	cd ./bin && ./test_counter
	cd ./bin && ./test_data
	cd ./bin && ./test_mr
	@echo "\033[92mAll tests PASSED \033[0m"
	@echo

# check for memory leaks
memcheck: main test_resource
	@valgrind --leak-check=full --show-reachable=yes \
		bin/wcount bin/lorem_ipsum.txt 4 > /dev/null
	@echo "\033[92mNO MEMORY LEAK detected! \033[0m"
	@echo

test_resource:
	@cp tests/lorem_ipsum.txt bin/lorem_ipsum.txt

test_counter: tests/test_counter.o src/merged_libs.o
	@$(CC) $(FLAGS) $(INCLUDE) $^ -o bin/$@ $(LIBS)
	@echo "Test linked $<"

test_data: tests/test_data.o src/merged_libs.o
	@$(CC) $(FLAGS) $(INCLUDE) $^ -o bin/$@ $(LIBS)
	@echo "Test linked $<"

test_mr: tests/test_mr.o src/merged_libs.o
	@$(CC) $(FLAGS) $(INCLUDE) $^ -o bin/$@ $(LIBS)
	@echo "Test linked $<"

# compilation rules
src/merged_libs.o: src/log.o src/mapreduce.o src/counter.o src/data.o
	@$(LD) $(LDFLAGS) $^ -o src/merged_libs.o
	@echo "Libs linked $<"

%.o: %.c
	@$(CC) -c $(FLAGS) $(EXTRAFLAGS) $(INCLUDE) $^ -o $@
	@echo "Compiled $<"
