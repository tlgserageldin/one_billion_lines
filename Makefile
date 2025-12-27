CC := clang
CFLAGS := -std=c23 -Wall -Wextra -Werror -Iinclude
LDFLAGS :=

SRC := src/hash_table.c src/q_strings.c
TESTS := test/test_ht.c test/test_runner.c
TESTBIN := build/tests
BUILD_DB := build/clang_db.json
COMP_DB := build/compile_commands.json

all: test

.PHONY: build_database
build_database: build
	rm -f $(BUILD_DB) $(COMP_DB)
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

$(TESTBIN): CFLAGS += -Itest -g
$(TESTBIN): build $(SRC) $(TESTS)
	$(CC) $(CFLAGS) $(SRC) $(TESTS) -o $(TESTBIN) $(LDFLAGS)

test: $(TESTBIN)

backup:
	tar -czvf ../one_billion_lines.tar.gz \
		--exclude 'data' --exclude '.cache' \
		-C .. one_billion_lines

asan: CFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer -g
asan: LDFLAGS += -fsanitize=address,undefined
asan: clean $(TESTBIN)
	./$(TESTBIN)
