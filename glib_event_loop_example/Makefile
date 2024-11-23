# 編譯器
CC = gcc

# 編譯選項
CFLAGS = $(shell pkg-config --cflags glib-2.0)
LDFLAGS = $(shell pkg-config --libs glib-2.0)

# 原始碼檔案
SRCS = $(wildcard *.c)

# 目標執行檔
TARGETS = $(SRCS:.c=)

# 編譯規則
all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# 清理規則
clean:
	rm -f $(TARGETS)
