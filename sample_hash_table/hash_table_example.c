/**
 * @file hash_table_example.c
 * @brief 使用 GLib 的 GHashTable 進行基本操作的範例程式
 *
 * 此程式展示如何使用 GLib 提供的 GHashTable 來建立雜湊表，並進行插入、查詢、移除等基本操作。
 * 程式將三個鍵值對插入到雜湊表中，然後查詢特定鍵對應的值，移除一個鍵值對，最後釋放雜湊表佔用的記憶體。
 *
 * 編譯方式：
 * gcc -o hash_table_example hash_table_example.c `pkg-config --cflags --libs glib-2.0`
 *
 * 執行方式：
 * ./hash_table_example
 *
 * 預期輸出：
 * 鍵 'banana' 對應的值為：香蕉
 *
 * @author: [您的姓名]
 * @date: [日期]
 */

#include <glib.h>   // 引入 GLib 函式庫的標頭檔案
#include <stdio.h>   // 引入標準輸入輸出函式庫

int main() {
    // 建立雜湊表，使用字串作為鍵
    GHashTable *hash_table = g_hash_table_new(g_str_hash, g_str_equal);

    // 插入鍵值對
    g_hash_table_insert(hash_table, "apple", "蘋果");
    g_hash_table_insert(hash_table, "banana", "香蕉");
    g_hash_table_insert(hash_table, "orange", "橙子");

    // 查詢特定鍵對應的值
    char *key = "banana";
    char *value = g_hash_table_lookup(hash_table, key);
    if (value) {
        printf("鍵 '%s' 對應的值為：%s\n", key, value);
    } else {
        printf("未找到鍵 '%s' 的對應值。\n", key);
    }

    // 移除特定鍵值對
    g_hash_table_remove(hash_table, "orange");

    // 釋放雜湊表佔用的記憶體
    g_hash_table_destroy(hash_table);

    return 0;
}

