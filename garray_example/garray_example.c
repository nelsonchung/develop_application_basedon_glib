/**
 * @file garray_example.c
 * @brief 使用 GLib 的 GArray 進行動態陣列操作的範例程式
 *
 * 此程式展示如何使用 GLib 提供的 GArray 來建立、追加、插入、移除和釋放動態陣列。
 * 程式首先建立一個 GArray，然後進行一系列元素操作，最後列印結果並釋放記憶體。
 *
 * 編譯方式：
 * gcc -o garray_example garray_example.c `pkg-config --cflags --libs glib-2.0`
 *
 * 執行方式：
 * ./garray_example
 *
 * 預期輸出：
 * 元素 0: 20
 * 元素 1: 10
 * 移除後的元素:
 * 元素 0: 10
 *
 * @author: [您的姓名]
 * @date: [日期]
 */

#include <glib.h>   // 引入 GLib 函式庫的標頭檔案
#include <stdio.h>   // 引入標準輸入輸出函式庫

int main() {
    // 建立 GArray，儲存 gint 型別的元素
    GArray *array = g_array_new(FALSE, FALSE, sizeof(gint));

    // 追加元素 10 到陣列末尾
    gint value1 = 10;
    g_array_append_val(array, value1);

    // 在索引 0 處插入元素 20
    gint value2 = 20;
    g_array_insert_val(array, 0, value2);

    // 訪問並列印所有元素
    for (guint i = 0; i < array->len; i++) {
        gint value = g_array_index(array, gint, i);
        printf("元素 %u: %d\n", i, value);
    }

    // 移除索引 0 處的元素
    g_array_remove_index(array, 0);

    // 列印移除後的所有元素
    printf("移除後的元素:\n");
    for (guint i = 0; i < array->len; i++) {
        gint value = g_array_index(array, gint, i);
        printf("元素 %u: %d\n", i, value);
    }

    // 釋放 GArray 所佔用的記憶體
    g_array_free(array, TRUE);

    return 0;
}

