CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g
LDFLAGS = -lglfw -lGLESv2 -lGLEW -lGL -lm

BUILD_DIR = build
INC_DIR = include

TEST_SRCS = $(wildcard *.c) $(wildcard arda/tests/*/*.c) $(wildcard ozan/tests/*/*.c) $(wildcard gizem/tests/*/*.c) $(wildcard hasan/tests/*.c)
TEST_OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(TEST_SRCS))

TARGET = $(BUILD_DIR)/test_suite

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)/*
