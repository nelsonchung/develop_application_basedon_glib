/**
 * @file glist_example.c
 * @brief 使用 GLib 的 GList 進行雙向鏈結串列操作的範例程式
 *
 * 此程式展示如何使用 GLib 提供的 GList 來建立、插入、遍歷和釋放雙向鏈結串列。
 * 程式將三個字串插入到串列中，然後遍歷並列印每個元素，最後釋放串列佔用的記憶體。
 *
 * 編譯方式：
 * gcc -o glist_example glist_example.c `pkg-config --cflags --libs glib-2.0`
 *
 * 執行方式：
 * ./glist_example
 *
 * 輸出結果：
 * 第一個元素
 * 第二個元素
 * 第三個元素
 *
 * @author: Nelson Chung
 * @date: 2024/11/21
 */

#include <glib.h>   // 引入 GLib 函式庫的標頭檔案
#include <stdio.h>  // 引入標準輸入輸出函式庫

int main() {
    // 建立一個空的 GList 指標，初始為 NULL
    GList *list = NULL;

    // 使用 g_list_prepend 函式將元素插入到串列的開頭
    // 由於是從開頭插入，最先插入的元素會在串列的最後
    list = g_list_prepend(list, "第三個元素");
    list = g_list_prepend(list, "第二個元素");
    list = g_list_prepend(list, "第一個元素");

    // 遍歷串列，並列印每個元素的資料
    // GList 結構包含一個指向下一個節點的指標（next）
    for (GList *l = list; l != NULL; l = l->next) {
        // l->data 是 gpointer 型別，需要轉型為 char* 才能正確列印
        printf("%s\n", (char *)l->data);
    }

    // 釋放整個串列佔用的記憶體
    // 注意：這僅釋放 GList 結構本身，若 data 指向動態配置的記憶體，需額外釋放
    g_list_free(list);

    return 0;
}

