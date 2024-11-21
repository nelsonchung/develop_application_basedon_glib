# 編譯器
CC = gcc

# 編譯選項
CFLAGS = `pkg-config --cflags glib-2.0`
LDFLAGS = `pkg-config --libs glib-2.0`

# 目標執行檔
TARGETS = glist_example glist_example_person glist_example_dynamic_insert

# 原始碼檔案
SRCS = glist_example.c glist_example_person.c glist_example_dynamic_insert.c

# 物件檔案
OBJS = $(SRCS:.c=.o)

# 編譯規則
all: $(TARGETS)

glist_example: glist_example.o
	$(CC) -o $@ $^ $(LDFLAGS)

glist_example_person: glist_example_person.o
	$(CC) -o $@ $^ $(LDFLAGS)

glist_example_dynamic_insert: glist_example_dynamic_insert.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理規則
clean:
	rm -f $(OBJS) $(TARGETS)
