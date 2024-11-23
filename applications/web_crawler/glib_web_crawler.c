 /**
 * @file glib_web_crawler.c
 * @brief A simple multithreaded web crawler supporting multiple websites using GLib.
 *
 * This program demonstrates how to use GLib to implement a multithreaded web crawler
 * capable of starting from multiple initial URLs and managing depth-limited crawling.
 *
 * Features:
 * - Concurrent crawling with GThreadPool.
 * - URL deduplication using GHashTable.
 * - Depth control to limit recursive crawling.
 * - Resolving relative URLs to absolute URLs.
 *
 * Compilation:
 * gcc -o glib_web_crawler glib_web_crawler.c `pkg-config --cflags --libs glib-2.0 libsoup-2.4`
 *
 * Execution:
 * ./glib_web_crawler <start_url1> [<start_url2> ...] [max_threads]
 *
 * Example:
 * ./glib_web_crawler https://example.com https://another.com 10
 *
 * @author: Nelson Chung
 * @date: 2024.11.23*/

#include <glib.h>
#include <libsoup/soup.h>
#include <stdio.h>
#include <stdlib.h> // For rand()

// Structure to represent a URL with its crawling depth
typedef struct {
    gchar *url;
    int depth;
} UrlItem;

// URL queue, mutex, and visited URLs hash table
GQueue *url_queue;
GMutex queue_mutex;
GHashTable *visited_urls;

// Thread pool and depth limit
GThreadPool *thread_pool;
int max_depth = 3;

/**
 * @brief Generate a unique filename for each URL.
 *
 * @param url The URL being saved.
 * @return A dynamically allocated string containing the filename.
 */
gchar* generate_filename(const gchar *url) {
    // Extract basename from URL and append a random number for uniqueness
    gchar *basename = g_path_get_basename(url);
    gchar *filename = g_strdup_printf("fetched_content_%s_%d.html", basename, rand());
    g_free(basename);
    return filename;
}

/**
 * @brief Save content to a file with a unique name.
 *
 * @param url The URL of the content (used for generating the filename).
 * @param content The content to save.
 */
void save_to_file(const gchar *url, const gchar *content) {
    gchar *filename = generate_filename(url);
    FILE *file = fopen(filename, "w");
    if (file) {
        fprintf(file, "%s", content);
        fclose(file);
        g_print("Content saved to %s\n", filename);
    } else {
        g_printerr("Failed to save content to %s\n", filename);
    }
    g_free(filename);
}

/**
 * @brief Resolve relative URLs to absolute URLs using libsoup.
 *
 * @param base_url The base URL.
 * @param relative_url The relative URL to resolve.
 * @return A dynamically allocated string containing the resolved URL.
 */
gchar* resolve_url(const gchar *base_url, const gchar *relative_url) {
    SoupURI *base = soup_uri_new(base_url);
    SoupURI *resolved = soup_uri_new_with_base(base, relative_url);
    gchar *resolved_url = soup_uri_to_string(resolved, FALSE);
    soup_uri_free(base);
    soup_uri_free(resolved);
    return resolved_url;
}

/**
 * @brief Extract URLs from HTML content and add them to the queue.
 *
 * @param content The HTML content to scan.
 * @param base_url The base URL for resolving relative URLs.
 * @param depth The current depth of crawling.
 */
void extract_urls(const gchar *content, const gchar *base_url, int depth) {
    if (depth >= max_depth) return;

    GRegex *regex = g_regex_new("href=[\"']?([^\"'>]+)", 0, 0, NULL);
    GMatchInfo *match_info;

    g_regex_match(regex, content, 0, &match_info);

    while (g_match_info_matches(match_info)) {
        gchar *url = g_match_info_fetch(match_info, 1);
        gchar *absolute_url = resolve_url(base_url, url);

        g_mutex_lock(&queue_mutex);
        if (!g_hash_table_contains(visited_urls, absolute_url)) {
            g_hash_table_add(visited_urls, g_strdup(absolute_url));

            UrlItem *item = g_new(UrlItem, 1);
            item->url = g_strdup(absolute_url);
            item->depth = depth + 1;

            g_queue_push_tail(url_queue, item);
            g_print("Discovered URL: %s (Depth: %d)\n", absolute_url, depth + 1);
        }
        g_mutex_unlock(&queue_mutex);

        g_free(url);
        g_free(absolute_url);
        g_match_info_next(match_info, NULL);
    }

    g_match_info_free(match_info);
    g_regex_unref(regex);
}

/**
 * @brief Fetch webpage content and process it.
 *
 * @param data Pointer to UrlItem containing the URL and depth.
 */
void fetch_url(gpointer data, gpointer user_data) {
    UrlItem *item = (UrlItem *)data;
    gchar *url = item->url;
    int depth = item->depth;

    g_print("Fetching URL: %s (Depth: %d)\n", url, depth);

    SoupSession *session = soup_session_new();
    SoupMessage *msg = soup_message_new("GET", url);

    soup_session_send_message(session, msg);

    if (msg->status_code == SOUP_STATUS_OK) {
        g_print("Successfully fetched: %s (Status: %d)\n", url, msg->status_code);

        gchar *response = g_strdup(msg->response_body->data);

        // Save content to a unique file
        save_to_file(url, response);

        // Extract URLs from the content
        extract_urls(response, url, depth);

        g_free(response);
    } else {
        g_printerr("Failed to fetch %s: %s (Status: %d)\n", url, msg->reason_phrase, msg->status_code);
    }

    g_object_unref(msg);
    g_object_unref(session);
    g_free(item->url);
    g_free(item);
}


/**
 * @brief Start the web crawler with the given URLs and thread count.
 *
 * @param start_urls A NULL-terminated array of initial URLs.
 * @param thread_count The maximum number of threads.
 */
void start_crawler(const gchar **start_urls, int thread_count) {
    url_queue = g_queue_new();
    g_mutex_init(&queue_mutex);
    visited_urls = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    // Add initial URLs to the queue
    for (int i = 0; start_urls[i] != NULL; i++) {
        g_mutex_lock(&queue_mutex);
        g_hash_table_add(visited_urls, g_strdup(start_urls[i]));

        UrlItem *item = g_new(UrlItem, 1);
        item->url = g_strdup(start_urls[i]);
        item->depth = 0;

        g_queue_push_tail(url_queue, item);
        g_mutex_unlock(&queue_mutex);
    }

    // Initialize the thread pool
    thread_pool = g_thread_pool_new(fetch_url, NULL, thread_count, FALSE, NULL);

    // Process the queue
    while (!g_queue_is_empty(url_queue)) {
        g_mutex_lock(&queue_mutex);
        UrlItem *item = g_queue_pop_head(url_queue);
        g_mutex_unlock(&queue_mutex);

        g_thread_pool_push(thread_pool, item, NULL);
    }

    // Wait for all threads to finish
    g_thread_pool_free(thread_pool, FALSE, TRUE);

    g_queue_free(url_queue);
    g_hash_table_destroy(visited_urls);
    g_mutex_clear(&queue_mutex);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        g_print("Usage: %s <start_url1> [<start_url2> ...] [max_threads]\n", argv[0]);
        return 1;
    }

    int thread_count = (argc > 2) ? atoi(argv[argc - 1]) : 5;

    // Create a NULL-terminated array of start URLs
    const gchar **start_urls = (const gchar **)g_malloc((argc - 1) * sizeof(gchar *));
    for (int i = 1; i < argc - 1; i++) {
        start_urls[i - 1] = argv[i];
    }
    start_urls[argc - 2] = NULL;

    g_print("Starting crawler with %d threads...\n", thread_count);
    start_crawler(start_urls, thread_count);
    g_print("Crawling finished.\n");

    g_free(start_urls);
    return 0;
}
