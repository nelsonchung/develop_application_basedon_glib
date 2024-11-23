/**
 * @file gtree_example.c
 * @brief 使用 GLib 的 GTree 進行平衡二元樹操作的範例程式
 *
 * 此程式展示如何使用 GLib 提供的 GTree 來建立、插入、搜尋和遍歷平衡二元樹。
 * 程式將一些整數鍵值對插入到樹中，然後搜尋特定鍵，並遍歷列印所有鍵值對。
 *
 * 編譯方式：
 * gcc -o gtree_example gtree_example.c `pkg-config --cflags --libs glib-2.0`
 *
 * 執行方式：
 * ./gtree_example
 *
 * 預期輸出：
 * 搜尋鍵 2 的結果：二
 * 鍵：1，值：一
 * 鍵：2，值：二
 * 鍵：3，值：三
 *
 * @author: Nelson Chung 
 * @date: 2024.11.23
 */

#include <glib.h>
#include <stdio.h>

// 比較函式，用於比較兩個整數鍵的大小
gint int_compare(gconstpointer a, gconstpointer b) {
    return GPOINTER_TO_INT(a) - GPOINTER_TO_INT(b);
}

// 列印鍵值對的函式
gboolean print_key_value(gpointer key, gpointer value, gpointer data) {
    printf("鍵：%d，值：%s\n", GPOINTER_TO_INT(key), (char *)value);
    return FALSE; // 返回 FALSE 以繼續遍歷
}

int main() {
    // 建立一個新的 GTree，使用 int_compare 函式作為比較函式
    GTree *tree = g_tree_new(int_compare);

    // 插入鍵值對
    g_tree_insert(tree, GINT_TO_POINTER(3), "三");
    g_tree_insert(tree, GINT_TO_POINTER(1), "一");
    g_tree_insert(tree, GINT_TO_POINTER(2), "二");

    // 搜尋鍵為 2 的值
    gpointer value = g_tree_lookup(tree, GINT_TO_POINTER(2));
    if (value) {
        printf("搜尋鍵 2 的結果：%s\n", (char *)value);
    } else {
        printf("未找到鍵 2 的值。\n");
    }

    // 遍歷並列印所有鍵值對
    g_tree_foreach(tree, print_key_value, NULL);

    // 釋放 GTree 所佔用的記憶體
    g_tree_destroy(tree);

    return 0;
}

