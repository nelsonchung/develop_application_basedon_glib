/**
 * @file glib_web_crawler.c
 * @brief A simple web crawler using GLib with enhanced logging.
 *
 * This program demonstrates how to use GLib to implement a basic web crawler.
 * It fetches webpage content, extracts URLs, and continues crawling iteratively.
 * Logs are added to track the progress and diagnose issues.
 *
 * Key Features:
 * - Fetch HTML content via HTTP GET requests using libsoup.
 * - Extract hyperlinks (href="...") from HTML using GRegex.
 * - Manage a queue of URLs with GQueue and ensure thread-safety using GMutex.
 *
 * Compilation:
 * gcc -o glib_web_crawler glib_web_crawler.c `pkg-config --cflags --libs glib-2.0 libsoup-2.4`
 *
 * Execution:
 * ./glib_web_crawler <start_url>
 *
 * Example:
 * ./glib_web_crawler https://example.com
 *
 * Notes:
 * - This implementation adds logging to track the progress and results of the crawler.
 * - Enhancements like concurrency, URL deduplication, or depth limitation can be added.
 *
 * @author: Nelson Chung
 * @date: 2024.11.23
 */

#include <glib.h>
#include <libsoup/soup.h>

// Queue to store URLs for crawling
GQueue *url_queue;
// Mutex for synchronizing access to the URL queue
GMutex queue_mutex;

/**
 * @brief Fetch webpage content via HTTP GET request with logging.
 *
 * @param url The URL of the webpage to fetch.
 * @return A dynamically allocated string containing the webpage content.
 *         The caller is responsible for freeing the string.
 */
gchar* fetch_url(const gchar *url) {
    g_print("Fetching URL: %s\n", url);

    SoupSession *session = soup_session_new();
    SoupMessage *msg = soup_message_new("GET", url);

    soup_session_send_message(session, msg);

    if (msg->status_code == SOUP_STATUS_OK) {
        g_print("Successfully fetched: %s (Status: %d)\n", url, msg->status_code);
        gchar *response = g_strdup(msg->response_body->data);
        g_object_unref(msg);
        g_object_unref(session);
        return response;
    } else {
        g_printerr("Failed to fetch %s: %s (Status: %d)\n", url, msg->reason_phrase, msg->status_code);
        g_object_unref(msg);
        g_object_unref(session);
        return NULL;
    }
}

/**
 * @brief Extract URLs from HTML content using regex and log the results.
 *
 * @param content The HTML content to scan.
 */
void extract_urls(const gchar *content) {
    g_print("Extracting URLs from content...\n");

    GRegex *regex = g_regex_new("href=\"(http[^\"]+)\"", 0, 0, NULL);
    GMatchInfo *match_info;

    g_regex_match(regex, content, 0, &match_info);

    int count = 0;
    while (g_match_info_matches(match_info)) {
        gchar *url = g_match_info_fetch(match_info, 1);

        g_mutex_lock(&queue_mutex);
        g_queue_push_tail(url_queue, g_strdup(url));
        g_mutex_unlock(&queue_mutex);

        g_print("Discovered URL: %s\n", url);
        count++;
        g_free(url);
        g_match_info_next(match_info, NULL);
    }

    g_print("Total URLs extracted: %d\n", count);

    g_match_info_free(match_info);
    g_regex_unref(regex);
}

/**
 * @brief Process a single URL and log progress.
 *
 * @param data A dynamically allocated string representing the URL to process.
 */
void crawl_url(gpointer data) {
    gchar *url = (gchar *)data;
    g_print("Crawling URL: %s\n", url);

    gchar *content = fetch_url(url);
    if (content) {
        extract_urls(content);
        g_free(content);
    } else {
        g_printerr("Failed to process URL: %s\n", url);
    }

    g_free(url);
}

/**
 * @brief Main crawler loop with logging.
 *
 * @param start_url The initial URL to begin crawling.
 */
void start_crawler(const gchar *start_url) {
    url_queue = g_queue_new();
    g_mutex_init(&queue_mutex);

    g_queue_push_tail(url_queue, g_strdup(start_url));

    while (!g_queue_is_empty(url_queue)) {
        g_mutex_lock(&queue_mutex);
        gchar *url = g_queue_pop_head(url_queue);
        g_mutex_unlock(&queue_mutex);

        crawl_url(url);
    }

    g_queue_free(url_queue);
    g_mutex_clear(&queue_mutex);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        g_print("Usage: %s <URL>\n", argv[0]);
        return 1;
    }

    g_print("Starting web crawler with initial URL: %s\n", argv[1]);
    start_crawler(argv[1]);
    g_print("Web crawler finished.\n");

    return 0;
}
