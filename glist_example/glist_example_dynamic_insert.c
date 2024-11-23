/**
 * @file glist_example_dynamic_insert.c
 * @brief 使用 GLib 的 GList 動態插入元素的範例程式
 *
 * 此程式展示如何使用 GLib 提供的 GList 來建立雙向鏈結串列，並動態地在串列中插入元素。
 * 程式將元素插入到串列的開頭、末尾，以及指定的位置，然後遍歷並列印每個元素，最後釋放串列佔用的記憶體。
 *
 * 編譯方式：
 * gcc -o glist_example_dynamic_insert glist_example_dynamic_insert.c `pkg-config --cflags --libs glib-2.0`
 *
 * 執行方式：
 * ./glist_example_dynamic_insert
 *
 * 輸出結果：
 * 元素0
 * 元素1.5
 * 元素1
 * 元素2
 *
 * @author: [您的姓名]
 * @date: [日期]
 */

#include <glib.h>   // 引入 GLib 函式庫的標頭檔案
#include <stdio.h>   // 引入標準輸入輸出函式庫

int main() {
    // 建立一個空的 GList 指標，初始為 NULL
    GList *list = NULL;

    // 動態插入元素到串列的末尾
    list = g_list_append(list, "元素1");
    list = g_list_append(list, "元素2");

    // 動態插入元素到串列的開頭
    list = g_list_prepend(list, "元素0");

    // 在指定位置插入元素（例如：在索引1的位置插入）
    list = g_list_insert(list, "元素1.5", 1);

    // 遍歷並列印串列中的元素
    for (GList *l = list; l != NULL; l = l->next) {
        printf("%s\n", (char *)l->data);
    }

    // 釋放串列佔用的記憶體
    g_list_free(list);

    return 0;
}

