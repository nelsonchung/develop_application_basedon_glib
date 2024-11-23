/**
 * @file glib_event_loop_example.c
 * @brief 使用 GLib 事件迴圈處理定時器事件的範例程式
 *
 * 此程式展示如何使用 GLib 提供的事件迴圈機制來處理定時器事件。
 * 程式建立一個主事件迴圈，並設定一個每秒觸發的定時器事件。
 * 當定時器觸發時，執行回呼函式，列印訊息。
 * 當定時器觸發五次後，結束事件迴圈。
 *
 * 編譯方式：
 * gcc -o glib_event_loop_example glib_event_loop_example.c `pkg-config --cflags --libs glib-2.0`
 *
 * 執行方式：
 * ./glib_event_loop_example
 *
 * 預期輸出：
 * Timer triggered: 1
 * Timer triggered: 2
 * Timer triggered: 3
 * Timer triggered: 4
 * Timer triggered: 5
 * Timer reached limit, exiting...
 *
 * @author: Nelson Chung
 * @date: 2024.11.23
 */

#include <glib.h>
#include <stdio.h>

// 定義計數器和主事件迴圈指標
static int counter = 0;
static GMainLoop *main_loop = NULL;

/**
 * @brief 定時器回呼函式
 *
 * 此函式在定時器觸發時被呼叫，列印計數器值。
 * 當計數器達到 5 時，結束主事件迴圈。
 *
 * @param data 傳遞給回呼函式的資料（此範例中未使用）
 * @return gboolean 若返回 TRUE，定時器繼續；若返回 FALSE，定時器停止
 */
gboolean timer_callback(gpointer data) {
    counter++;
    g_print("Timer triggered: %d\n", counter);
    if (counter >= 5) {
        g_print("Timer reached limit, exiting...\n");
        g_main_loop_quit(main_loop);
        return FALSE; // 停止定時器
    }
    return TRUE; // 繼續定時器
}

int main(int argc, char *argv[]) {
    // 建立主事件迴圈
    main_loop = g_main_loop_new(NULL, FALSE);

    // 新增定時器事件源，每秒觸發一次
    g_timeout_add_seconds(1, timer_callback, NULL);

    // 運行主事件迴圈
    g_main_loop_run(main_loop);

    // 釋放主事件迴圈資源
    g_main_loop_unref(main_loop);

    return 0;
}
