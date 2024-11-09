CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g
LIBS=
SRC=src/
BUILD=build/
BIN=bin/

DEPS=  $(BUILD)Unilang.o $(BUILD)lexer.o $(BUILD)string_view.o
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
	