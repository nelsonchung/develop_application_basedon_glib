/**
 * @file gmutex_example.c
 * @brief 使用 GLib 的 GMutex 進行執行緒同步的範例程式
 *
 * 此程式展示如何使用 GLib 提供的 GMutex 來保護共享變數的存取，避免多執行緒環境下的競爭條件。
 * 程式建立兩個執行緒，分別對共享計數器進行遞增操作，並使用互斥鎖確保操作的同步性。
 * 在每次遞增操作時，列印是哪個執行緒進行了操作。
 * 使用一個布林變數控制是否啟用互斥鎖。
 *
 * 編譯方式：
 * gcc -o gmutex_example gmutex_example.c `pkg-config --cflags --libs glib-2.0`
 *
 * 執行方式：
 * ./gmutex_example
 *
 * 預期輸出：
 * [thread1] shared_counter incremented to: 1
 * [thread2] shared_counter incremented to: 2
 * ...
 * 最終計數值：200000
 *
 * @author: Nelson Chung 
 * @date: 2024.11.23
 */

#include <glib.h>
#include <stdio.h>

// 定義互斥鎖變數
GMutex mutex;

// 定義共享計數器變數
gint shared_counter = 0;

// 定義布林變數以控制是否啟用互斥鎖
gboolean use_mutex = TRUE;

/**
 * @brief 執行緒函式，對共享計數器進行遞增操作
 *
 * 每個執行緒執行此函式，對共享計數器進行 100,000 次遞增操作。
 * 根據 use_mutex 變數的值，決定是否使用互斥鎖進行同步。
 * 並在每次遞增操作時，列印是哪個執行緒進行了操作。
 *
 * @param data 傳遞給執行緒的資料，包含執行緒名稱
 * @return NULL
 */
void* increment_counter(gpointer data) {
    const gchar* thread_name = (const gchar*)data;
    for (int i = 0; i < 100000; i++) {
        if (use_mutex) {
            // 鎖定互斥鎖，進入臨界區域
            g_mutex_lock(&mutex);
        }
        // 對共享計數器進行遞增操作
        shared_counter++;
        // 列印執行緒名稱和當前計數值
        printf("[%s] shared_counter incremented to: %d\n", thread_name, shared_counter);
        if (use_mutex) {
            // 解鎖互斥鎖，離開臨界區域
            g_mutex_unlock(&mutex);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    // 檢查命令列參數，決定是否啟用互斥鎖
    if (argc > 1 && g_strcmp0(argv[1], "--no-mutex") == 0) {
        use_mutex = FALSE;
    }

    // 初始化互斥鎖
    g_mutex_init(&mutex);

    // 建立並啟動兩個執行緒，執行 increment_counter 函式，並傳遞執行緒名稱作為參數
    GThread* thread1 = g_thread_new("thread1", increment_counter, "thread1");
    GThread* thread2 = g_thread_new("thread2", increment_counter, "thread2");

    // 等待兩個執行緒執行完畢
    g_thread_join(thread1);
    g_thread_join(thread2);

    // 列印最終的共享計數器值
    printf("最終計數值：%d\n", shared_counter);

    // 清理互斥鎖
    g_mutex_clear(&mutex);
    return 0;
}

