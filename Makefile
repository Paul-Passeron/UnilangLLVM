CC=gcc
CFLAGS=-Wall -Wextra -g
LIBS=-I/usr/include -fno-exceptions -funwind-tables -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -L/usr/lib64 -lLLVM

SRC=src/
BUILD=build/
BIN=bin/

DEPS=  $(BUILD)Unilang.o $(BUILD)lexer.o $(BUILD)string_view.o $(BUILD)regexp.o $(BUILD)unilang_lexer.o $(BUILD)parser.o $(BUILD)ast.o $(BUILD)parser_helper.o    $(BUILD)generator.o $(BUILD)unilang_parser.o
all: init lines Unilang
lines:
	@echo "C:"
	@wc -l $$( find -wholename './*.[hc]') | tail -n 1
$(BUILD)%.o: $(SRC)%.c
	 $(CC) $(CFLAGS) -o $@ $^ -c
$(BIN)Unilang: $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
Unilang: $(BIN)Unilang
clean:
	rm -rf $(BIN)*
	rm -rf $(BUILD)*
install:
	cp $(BIN)Unilang /bin/
init:
	