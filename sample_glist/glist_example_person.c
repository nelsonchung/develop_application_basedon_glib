/**
 * @file glist_example_person.c
 * @brief 使用 GLib 的 GList 來儲存和操作自訂結構體資料的範例程式
 *
 * 此程式展示如何使用 GLib 提供的 GList 來建立、插入、遍歷和釋放包含自訂結構體資料的雙向鏈結串列。
 * 程式將三個 Person 結構體插入到串列中，然後遍歷並列印每個元素的資訊，最後釋放串列及其佔用的記憶體。
 *
 * 編譯方式：
 * gcc -o glist_example_person glist_example_person.c `pkg-config --cflags --libs glib-2.0`
 *
 * 執行方式：
 * ./glist_example_person
 *
 * 輸出結果：
 * ID: 1, Name: Alice
 * ID: 2, Name: Bob
 * ID: 3, Name: Charlie
 *
 * @author: Nelson Chung
 * @date: 2024/11/21
 */

#include <glib.h>   // 引入 GLib 函式庫的標頭檔案
#include <stdio.h>  // 引入標準輸入輸出函式庫

// 定義 Person 結構體，包含 id 和 name 欄位
typedef struct {
    int id;
    char name[50];
} Person;

int main() {
    // 建立一個空的 GList 指標，初始為 NULL
    GList *list = NULL;

    // 動態分配記憶體並初始化 Person 結構體
    Person *person1 = g_malloc(sizeof(Person));
    person1->id = 1;
    g_snprintf(person1->name, sizeof(person1->name), "Alice");

    Person *person2 = g_malloc(sizeof(Person));
    person2->id = 2;
    g_snprintf(person2->name, sizeof(person2->name), "Bob");

    Person *person3 = g_malloc(sizeof(Person));
    person3->id = 3;
    g_snprintf(person3->name, sizeof(person3->name), "Charlie");

    // 將 Person 結構體指標插入到串列的末尾
    list = g_list_append(list, person1);
    list = g_list_append(list, person2);
    list = g_list_append(list, person3);

    // 遍歷串列，並列印每個元素的資訊
    for (GList *l = list; l != NULL; l = l->next) {
        Person *p = (Person *)l->data;
        printf("ID: %d, Name: %s\n", p->id, p->name);
    }

    // 釋放串列中的每個元素的資料
    g_list_free_full(list, g_free);

    return 0;
}

