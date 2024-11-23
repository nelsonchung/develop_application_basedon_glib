/**
 * @file glib_io_event_example.c
 * @brief 使用 GLib 事件迴圈監聽標準輸入的範例程式
 *
 * 此程式展示如何使用 GLib 提供的事件迴圈機制來監聽標準輸入（stdin）的資料輸入。
 * 程式建立一個主事件迴圈，並設定一個 I/O 事件源，監聽標準輸入的可讀事件。
 * 當標準輸入有資料輸入時，執行回呼函式，讀取並列印輸入的資料。
 * 若輸入為 "exit\n"，則結束事件迴圈。
 *
 * 編譯方式：
 * gcc -o glib_io_event_example glib_io_event_example.c `pkg-config --cflags --libs glib-2.0`
 *
 * 執行方式：
 * ./glib_io_event_example
 *
 * 預期輸出：
 * 請輸入文字（輸入 "exit" 結束程式）：
 * [使用者輸入的文字]
 * ...
 * 請輸入文字（輸入 "exit" 結束程式）：
 * exit
 * 收到結束指令，程式即將結束。
 *
 * @author: [您的姓名]
 * @date: [日期]
 */

#include <glib.h>
#include <stdio.h>

// 定義主事件迴圈指標
static GMainLoop *main_loop = NULL;

/**
 * @brief 標準輸入 I/O 事件回呼函式
 *
 * 此函式在標準輸入有資料可讀時被呼叫，讀取並列印輸入的資料。
 * 若輸入為 "exit\n"，則結束主事件迴圈。
 *
 * @param source GIOChannel 指標，代表標準輸入
 * @param condition GIOCondition，事件條件（此範例中未使用）
 * @param data 傳遞給回呼函式的資料（此範例中未使用）
 * @return gboolean 若返回 TRUE，繼續監聽；若返回 FALSE，停止監聽
 */
gboolean stdin_callback(GIOChannel *source, GIOCondition condition, gpointer data) {
    gchar *line = NULL;
    gsize length = 0;
    GError *error = NULL;

    // 讀取一行輸入
    GIOStatus status = g_io_channel_read_line(source, &line, &length, NULL, &error);
    if (status == G_IO_STATUS_NORMAL) {
        // 列印輸入的資料
        g_print("We got your input: %s", line);
        // 若輸入為 "exit\n"，則結束事件迴圈
        if (g_strcmp0(line, "exit\n") == 0) {
            g_print("Get the exit command and ready to close the application。\n");
            g_main_loop_quit(main_loop);
            g_free(line);
            return FALSE; // 停止監聽
        }
    } else if (status == G_IO_STATUS_ERROR) {
        g_printerr("Something wrong when read the input: %s\n", error->message);
        g_error_free(error);
    }

    g_free(line);
    return TRUE; // 繼續監聽
}

int main(int argc, char *argv[]) {
    // 建立主事件迴圈
    main_loop = g_main_loop_new(NULL, FALSE);

    // 取得標準輸入的 GIOChannel
    GIOChannel *stdin_channel = g_io_channel_unix_new(fileno(stdin));

    // 設定標準輸入為非阻塞模式
    g_io_channel_set_flags(stdin_channel, g_io_channel_get_flags(stdin_channel) | G_IO_FLAG_NONBLOCK, NULL);

    // 新增 I/O 事件源，監聽標準輸入的可讀事件
    g_io_add_watch(stdin_channel, G_IO_IN, (GIOFunc)stdin_callback, NULL);

    // 提示使用者輸入
    g_print("Please input (input \"exit\" to exit the application): \n");

    // 運行主事件迴圈
    g_main_loop_run(main_loop);

    // 釋放資源
    g_io_channel_unref(stdin_channel);
    g_main_loop_unref(main_loop);

    return 0;
}
