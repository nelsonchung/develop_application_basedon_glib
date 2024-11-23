/**
 * @file gstring_example.c
 * @brief 使用 GLib 的 GString 進行動態字串操作的範例程式
 *
 * 此程式展示如何使用 GLib 提供的 GString 來建立、追加、插入、刪除和釋放動態字串。
 * 程式首先建立一個 GString，然後進行一系列字串操作，最後列印結果並釋放記憶體。
 *
 * 編譯方式：
 * gcc -o gstring_example gstring_example.c `pkg-config --cflags --libs glib-2.0`
 *
 * 執行方式：
 * ./gstring_example
 *
 * 預期輸出：
 * Hello, World!
 *
 * @author: Nelson Chung
 * @date: 2024/11/22
 */

#include <glib.h>   // 引入 GLib 函式庫的標頭檔案
#include <stdio.h>   // 引入標準輸入輸出函式庫

int main() {
    // 建立 GString，初始內容為 "Hello"
    GString *str = g_string_new("Hello");

    // 追加字串 ", World!" 到 GString 的末尾
    g_string_append(str, ", World!");

    // 在索引 5 處插入字串 " GLib"
    g_string_insert(str, 5, " GLib");

    // 列印中間結果
    printf("%s\n", str->str);  // 預期輸出：Hello GLib, World!
    
    // 從索引 5 開始刪除長度為 5 的子字串
    g_string_erase(str, 5, 5);

    // 列印最終結果
    printf("%s\n", str->str);  // 預期輸出：Hello, World!

    // 釋放 GString 所佔用的記憶體
    g_string_free(str, TRUE);

    return 0;
}

