CC := clang
CFLAGS := -std=c23 -Wall -Wextra -Werror -Iinclude

SRC := src/hash_table.c src/q_strings.c src/multi_threaded.c
TESTS := 
HEADERS := include/hash_table.h include/q_strings.h
BUILD_DB := build/clang_db.json
COMP_DB := build/compile_commands.json

all: multithreaded

.PHONY: build_database
build_database: | build
	rm -f $(BUILD_DB)
	@for f in $(SRC) $(TESTS); do \
		$(CC) $(CFLAGS) -MJ $(BUILD_DB) -c $$f -o /dev/null; \
	done
	printf '[\n' > $(COMP_DB)
	awk '{ sub(/,$$/,""); print }' $(BUILD_DB) >> $(COMP_DB)
	printf '\n]\n' >> $(COMP_DB)

clean:
	rm -rf build

build:
	mkdir -p build

multithreaded: $(SRC) $(HEADERS) | build
	$(CC) $(CFLAGS) $(SRC) -o build/multithreaded


CFLAGS += -g -O0
debug_multithreaded: $(SRC) $(HEADERS) | build
	$(CC) $(CFLAGS) $(SRC) -o build/debug_multithreaded
