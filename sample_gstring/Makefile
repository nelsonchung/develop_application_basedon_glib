# 編譯器
CC = gcc

# 編譯選項
CFLAGS = `pkg-config --cflags glib-2.0`
LDFLAGS = `pkg-config --libs glib-2.0`

# 目標執行檔
TARGETS = gstring_example

# 原始碼檔案
SRCS = gstring_example.c

# 物件檔案
OBJS = $(SRCS:.c=.o)

# 編譯規則
all: $(TARGETS)

gstring_example: gstring_example.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理規則
clean:
	rm -f $(OBJS) $(TARGETS)
