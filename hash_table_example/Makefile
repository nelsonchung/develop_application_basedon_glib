# 編譯器
CC = gcc

# 編譯選項
CFLAGS = `pkg-config --cflags glib-2.0`
LDFLAGS = `pkg-config --libs glib-2.0`

# 目標執行檔
TARGETS = hash_table_example

# 原始碼檔案
SRCS = hash_table_example.c

# 物件檔案
OBJS = $(SRCS:.c=.o)

# 編譯規則
all: $(TARGETS)

hash_table_example: hash_table_example.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理規則
clean:
	rm -f $(OBJS) $(TARGETS)
