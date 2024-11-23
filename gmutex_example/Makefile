# 編譯器
CC = gcc

# 編譯選項
CFLAGS = `pkg-config --cflags glib-2.0`
LDFLAGS = `pkg-config --libs glib-2.0`

# Parameters
FILE = gmutex_example

# 目標執行檔
TARGETS = ${FILE}

# 原始碼檔案
SRCS = ${FILE}.c

# 物件檔案
OBJS = $(SRCS:.c=.o)

# 編譯規則
all: $(TARGETS)

${FILE}: ${FILE}.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理規則
clean:
	rm -f $(OBJS) $(TARGETS)
