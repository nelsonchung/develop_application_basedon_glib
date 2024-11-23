/**
 * @file glib_custom_event_source_example.c
 * @brief 使用 GLib 的事件迴圈處理自訂事件來源的範例程式
 *
 * 此程式展示如何使用 GLib 的事件迴圈機制來處理自訂事件來源。
 * 程式建立了一個主事件迴圈，以及一個自訂的事件來源，每秒觸發一次。
 * 當事件來源觸發時，執行回呼函式並印出訊息。
 * 當事件來源觸發五次後，結束事件迴圈。
 *
 * 編譯方式：
 * gcc -o glib_custom_event_source_example glib_custom_event_source_example.c `pkg-config --cflags --libs glib-2.0`
 *
 * 執行方式：
 * ./glib_custom_event_source_example
 *
 * 預期輸出：
 * Custom event triggered: 1
 * Custom event triggered: 2
 * Custom event triggered: 3
 * Custom event triggered: 4
 * Custom event triggered: 5
 * Custom event reached limit, exiting...
 *
 * @author: Nelson Chung
 * @date: 2024.11.23
 */

#include <glib.h>
#include <stdio.h>
#include <time.h>

// 計數器和主事件迴圈指標
static int counter = 0;
static GMainLoop *main_loop = NULL;

// 自訂事件來源的結構體
typedef struct {
    GSource source;             // 基礎 GSource 結構
    gint64 next_execution_time; // 下一次觸發的時間（以微秒為單位）
    int interval;               // 事件間隔（以毫秒為單位）
} CustomSource;

// 自訂事件來源的回呼函式
gboolean custom_source_dispatch(GSource *source, GSourceFunc callback, gpointer user_data) {
    CustomSource *custom_source = (CustomSource *)source;

    counter++;
    g_print("Custom event triggered: %d\n", counter);

    // 設定下一次觸發時間
    custom_source->next_execution_time = g_get_monotonic_time() + (custom_source->interval * 1000);

    if (counter >= 5) {
        g_print("Custom event reached limit, exiting...\n");
        g_main_loop_quit(main_loop);
        return FALSE; // 停止事件來源
    }
    return TRUE; // 繼續事件來源
}

// 自訂事件來源的準備函式
gboolean custom_source_prepare(GSource *source, gint *timeout) {
    CustomSource *custom_source = (CustomSource *)source;
    gint64 current_time = g_get_monotonic_time();

    if (current_time >= custom_source->next_execution_time) {
        *timeout = 0; // 立即觸發事件
        return TRUE;
    }

    // 計算剩餘時間並設定為 timeout
    *timeout = (custom_source->next_execution_time - current_time) / 1000;
    return FALSE;
}

// 自訂事件來源的檢查函式
gboolean custom_source_check(GSource *source) {
    CustomSource *custom_source = (CustomSource *)source;
    gint64 current_time = g_get_monotonic_time();

    // 確認是否已到下一次觸發時間
    return current_time >= custom_source->next_execution_time;
}

// 自訂事件來源的結束函式
void custom_source_finalize(GSource *source) {
    g_print("Custom source finalized.\n");
}

int main(int argc, char *argv[]) {
    // 建立主事件迴圈
    main_loop = g_main_loop_new(NULL, FALSE);
    g_print("主事件迴圈已建立。\n");

    // 定義自訂事件來源的函式表
    GSourceFuncs custom_source_funcs = {
        .prepare = custom_source_prepare,
        .check = custom_source_check,
        .dispatch = custom_source_dispatch,
        .finalize = custom_source_finalize,
    };

    // 建立自訂事件來源
    CustomSource *custom_source = (CustomSource *)g_source_new(&custom_source_funcs, sizeof(CustomSource));
    custom_source->interval = 1000; // 設定事件間隔為 1000 毫秒
    custom_source->next_execution_time = g_get_monotonic_time() + (custom_source->interval * 1000);
    g_print("自訂事件來源已建立。\n");

    // 將自訂事件來源加入主事件迴圈
    guint source_id = g_source_attach(&custom_source->source, NULL);
    g_print("自訂事件來源已成功附加，Source ID: %u\n", source_id);

    // 啟動主事件迴圈
    g_print("主事件迴圈開始運行...\n");
    g_main_loop_run(main_loop);

    // 釋放資源
    g_source_unref(&custom_source->source);
    g_print("自訂事件來源的參考已解除。\n");
    g_main_loop_unref(main_loop);
    g_print("主事件迴圈已終止。\n");

    return 0;
}