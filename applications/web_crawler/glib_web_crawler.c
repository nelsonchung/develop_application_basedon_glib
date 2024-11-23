/**
 * @file glib_web_crawler.c
 * @brief A simple multithreaded web crawler using GLib.
 *
 * This program demonstrates how to use GLib to implement a basic multithreaded web crawler.
 * It fetches webpage content using libsoup, extracts URLs using GRegex, and processes them in multiple threads.
 *
 * Key Features:
 * - Fetch HTML content via HTTP GET requests using libsoup.
 * - Extract hyperlinks (href="...") from HTML using GRegex.
 * - Manage a queue of URLs with GQueue and ensure thread safety using GMutex.
 * - Use GThreadPool for concurrent HTTP requests.
 *
 * Compilation:
 * gcc -o glib_web_crawler glib_web_crawler.c `pkg-config --cflags --libs glib-2.0 libsoup-2.4`
 *
 * Execution:
 * ./glib_web_crawler <start_url> [max_threads]
 *
 * Example:
 * ./glib_web_crawler https://example.com 10
 *
 * Notes:
 * - Enhancements like URL deduplication, depth limitation, or domain restriction can be added.
 *
 * @author: Nelson Chung
 * @date: 2024.11.23
 */

#include <stdio.h>
#include <glib.h>
#include <libsoup/soup.h>

// Queue for storing URLs to be crawled
GQueue *url_queue;
// Mutex for synchronizing access to the URL queue
GMutex queue_mutex;
// Hash table to track visited URLs
GHashTable *visited_urls;
// Thread pool for concurrent crawling
GThreadPool *thread_pool;

/**
 * @brief Save content to a file for debugging or analysis.
 *
 * @param filename The name of the file.
 * @param content The content to save.
 */
void save_to_file(const gchar *filename, const gchar *content) {
    FILE *file = fopen(filename, "w");
    if (file) {
        fprintf(file, "%s", content);
        fclose(file);
        g_print("Content saved to %s\n", filename);
    } else {
        g_printerr("Failed to save content to %s\n", filename);
    }
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
 * @brief Extract URLs from HTML content using a regular expression.
 *
 * @param content The HTML content to scan.
 * @param base_url The base URL for resolving relative URLs.
 */
void extract_urls(const gchar *content, const gchar *base_url) {
    GRegex *regex = g_regex_new("href=[\"']?([^\"'>]+)", 0, 0, NULL);
    GMatchInfo *match_info;

    g_regex_match(regex, content, 0, &match_info);

    while (g_match_info_matches(match_info)) {
        gchar *url = g_match_info_fetch(match_info, 1);

        // Resolve relative URLs
        gchar *absolute_url = resolve_url(base_url, url);

        // Add URL to the queue if not visited
        g_mutex_lock(&queue_mutex);
        if (!g_hash_table_contains(visited_urls, absolute_url)) {
            g_hash_table_add(visited_urls, g_strdup(absolute_url));
            g_queue_push_tail(url_queue, g_strdup(absolute_url));
            g_print("Discovered URL: %s\n", absolute_url);
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
 * @param data The URL to fetch.
 */
void fetch_url(gpointer data, gpointer user_data) {
    gchar *url = (gchar *)data;
    g_print("Fetching URL: %s\n", url);

    SoupSession *session = soup_session_new();
    SoupMessage *msg = soup_message_new("GET", url);

    soup_session_send_message(session, msg);

    if (msg->status_code == SOUP_STATUS_OK) {
        g_print("Successfully fetched: %s (Status: %d)\n", url, msg->status_code);

        gchar *response = g_strdup(msg->response_body->data);

        // Save content to a file (for debugging)
        save_to_file("fetched_content.html", response);

        // Extract URLs from the content
        extract_urls(response, url);

        g_free(response);
    } else {
        g_printerr("Failed to fetch %s: %s (Status: %d)\n", url, msg->reason_phrase, msg->status_code);
    }

    g_object_unref(msg);
    g_object_unref(session);
    g_free(url);
}

/**
 * @brief Start the web crawler with the given URL and thread count.
 *
 * @param start_url The initial URL to crawl.
 * @param max_threads The maximum number of threads.
 */
void start_crawler(const gchar *start_url, int max_threads) {
    url_queue = g_queue_new();
    g_mutex_init(&queue_mutex);
    visited_urls = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    // Add the initial URL to the queue
    g_mutex_lock(&queue_mutex);
    g_hash_table_add(visited_urls, g_strdup(start_url));
    g_queue_push_tail(url_queue, g_strdup(start_url));
    g_mutex_unlock(&queue_mutex);

    // Initialize the thread pool
    thread_pool = g_thread_pool_new(fetch_url, NULL, max_threads, FALSE, NULL);

    // Process the URL queue
    while (!g_queue_is_empty(url_queue)) {
        g_mutex_lock(&queue_mutex);
        gchar *url = g_queue_pop_head(url_queue);
        g_mutex_unlock(&queue_mutex);

        g_thread_pool_push(thread_pool, url, NULL);
    }

    // Wait for all threads to finish
    g_thread_pool_free(thread_pool, FALSE, TRUE);

    g_queue_free(url_queue);
    g_hash_table_destroy(visited_urls);
    g_mutex_clear(&queue_mutex);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        g_print("Usage: %s <start_url> [max_threads]\n", argv[0]);
        return 1;
    }

    const gchar *start_url = argv[1];
    int max_threads = (argc > 2) ? atoi(argv[2]) : 5;

    g_print("Starting crawler with %d threads...\n", max_threads);
    start_crawler(start_url, max_threads);
    g_print("Crawling finished.\n");

    return 0;
}
