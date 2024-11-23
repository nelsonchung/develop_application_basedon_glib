/**
 * @file glib_custom_event_source_example.c
 * @brief Using GLib's event loop to handle custom event sources
 *
 * This program demonstrates how to use GLib's event loop mechanism to handle custom event sources.
 * It sets up a main event loop and a custom event source that triggers once per second.
 * When the event source triggers, a callback function is executed to print a message.
 * The event loop stops after the event source triggers five times.
 *
 * Compilation:
 * gcc -o glib_custom_event_source_example glib_custom_event_source_example.c `pkg-config --cflags --libs glib-2.0`
 *
 * Execution:
 * ./glib_custom_event_source_example
 *
 * Expected Output:
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

// Counter and main event loop pointer
static int counter = 0;
static GMainLoop *main_loop = NULL;

// Custom event source structure
typedef struct {
    GSource source;
    gint64 next_execution_time; // Next scheduled execution time in microseconds
    int interval;               // Interval in milliseconds
} CustomSource;

// Callback for custom event source
gboolean custom_source_dispatch(GSource *source, GSourceFunc callback, gpointer user_data) {
    CustomSource *custom_source = (CustomSource *)source;

    counter++;
    g_print("Custom event triggered: %d\n", counter);

    // Schedule the next execution time
    custom_source->next_execution_time = g_get_monotonic_time() + (custom_source->interval * 1000);

    if (counter >= 5) {
        g_print("Custom event reached limit, exiting...\n");
        g_main_loop_quit(main_loop);
        return FALSE; // Stop the event source
    }
    return TRUE; // Continue the event source
}

// Prepare function for the custom source
gboolean custom_source_prepare(GSource *source, gint *timeout) {
    CustomSource *custom_source = (CustomSource *)source;
    gint64 current_time = g_get_monotonic_time();

    if (current_time >= custom_source->next_execution_time) {
        *timeout = 0; // Ready to dispatch immediately
        return TRUE;
    }

    // Calculate remaining time until the next execution
    *timeout = (custom_source->next_execution_time - current_time) / 1000;
    return FALSE;
}

// Check function for the custom source
gboolean custom_source_check(GSource *source) {
    CustomSource *custom_source = (CustomSource *)source;
    gint64 current_time = g_get_monotonic_time();

    // Check if the current time has reached the next execution time
    return current_time >= custom_source->next_execution_time;
}

// Finalize function for the custom source
void custom_source_finalize(GSource *source) {
    g_print("Custom source finalized.\n");
}

int main(int argc, char *argv[]) {
    // Create the main event loop
    main_loop = g_main_loop_new(NULL, FALSE);
    g_print("Main event loop created.\n");

    // Define custom event source function table
    GSourceFuncs custom_source_funcs = {
        .prepare = custom_source_prepare,
        .check = custom_source_check,
        .dispatch = custom_source_dispatch,
        .finalize = custom_source_finalize,
    };

    // Create the custom event source
    CustomSource *custom_source = (CustomSource *)g_source_new(&custom_source_funcs, sizeof(CustomSource));
    custom_source->interval = 1000; // Set interval to 1000 ms
    custom_source->next_execution_time = g_get_monotonic_time() + (custom_source->interval * 1000);
    g_print("Custom event source created.\n");

    // Attach the custom event source to the main event loop
    guint source_id = g_source_attach(&custom_source->source, NULL);
    g_print("Custom event source attached successfully, Source ID: %u\n", source_id);

    // Run the main event loop
    g_print("Main event loop is running...\n");
    g_main_loop_run(main_loop);

    // Release resources
    g_source_unref(&custom_source->source);
    g_print("Custom event source unreferenced.\n");
    g_main_loop_unref(main_loop);
    g_print("Main event loop terminated.\n");

    return 0;
}