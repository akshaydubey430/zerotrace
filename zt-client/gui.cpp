#include "include/gui.hpp"
#include <gtk/gtk.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

// --- Utilities ---

std::string formatSize(uint64_t bytes) {
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB"};
    int i = 0;
    double dblBytes = bytes;
    if (bytes > 1024) {
        for (i = 0; (bytes / 1024) > 0 && i < 5; i++, bytes /= 1024)
            dblBytes = bytes / 1024.0;
    }
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << dblBytes << " " << suffixes[i];
    return ss.str();
}

// --- Callbacks ---

static void on_wipe_clicked(GtkButton* btn, gpointer user_data) {
    char* devicePath = (char*)user_data;
    std::cout << "Wipe requested for: " << devicePath << std::endl;
    // TODO: Connect to actual wipe logic
}

static void free_device_path(gpointer data, GClosure* closure) {
    (void)closure;
    g_free(data);
}

static void refresh_device_list(GtkWidget* container_box) {
    // Remove all children
    GtkWidget *child = gtk_widget_get_first_child(container_box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_box_remove(GTK_BOX(container_box), child);
        child = next;
    }

    std::vector<Device> devices = getDevices();
    
    if (devices.empty()) {
        GtkWidget *label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label), "<span color='red' size='large'>No devices found.</span>\n(Try running as root/sudo if drives are missing)");
        gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_top(label, 20);
        gtk_box_append(GTK_BOX(container_box), label);
        return;
    }

    for (const auto& dev : devices) {
        // Card Container (Frame)
        GtkWidget *frame = gtk_frame_new(NULL);
        gtk_widget_set_margin_bottom(frame, 10);
        
        // Main Box inside Frame
        GtkWidget *card_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        gtk_widget_set_margin_top(card_box, 12);
        gtk_widget_set_margin_bottom(card_box, 12);
        gtk_widget_set_margin_start(card_box, 12);
        gtk_widget_set_margin_end(card_box, 12);
        gtk_frame_set_child(GTK_FRAME(frame), card_box);

        // -- Top Row: Type & Name --
        GtkWidget *top_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        
        // Type Badge
        GtkWidget *type_label = gtk_label_new(dev.type.c_str());
        gtk_widget_add_css_class(type_label, "badge"); // We can add CSS later if we want, or use markup
        char* type_markup = g_strdup_printf("<span background='#40a4ff' color='white' weight='bold'>  %s  </span>", dev.type.c_str());
        gtk_label_set_markup(GTK_LABEL(type_label), type_markup);
        g_free(type_markup);
        gtk_box_append(GTK_BOX(top_row), type_label);

        // Name
        GtkWidget *name_label = gtk_label_new(dev.name.c_str());
        char* name_markup = g_strdup_printf("<span size='large' weight='bold'>%s</span>", dev.name.c_str());
        gtk_label_set_markup(GTK_LABEL(name_label), name_markup);
        g_free(name_markup);
        gtk_box_append(GTK_BOX(top_row), name_label);

        gtk_box_append(GTK_BOX(card_box), top_row);

        // -- Details Row --
        GtkWidget *details_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
        
        // Model
        std::string modelStr = "Model: " + (dev.model.empty() ? "Unknown" : dev.model);
        GtkWidget *model_label = gtk_label_new(modelStr.c_str());
        gtk_widget_add_css_class(model_label, "dim-label");
        gtk_box_append(GTK_BOX(details_box), model_label);

        // Size
        std::string sizeStr = "Size: " + formatSize(dev.sizeBytes);
        GtkWidget *size_label = gtk_label_new(sizeStr.c_str());
        gtk_widget_add_css_class(size_label, "dim-label");
        gtk_box_append(GTK_BOX(details_box), size_label);

        // Flags
        if (dev.isReadOnly) {
            GtkWidget *flag = gtk_label_new(NULL);
            gtk_label_set_markup(GTK_LABEL(flag), "<span color='#ff6464'>[READ-ONLY]</span>");
            gtk_box_append(GTK_BOX(details_box), flag);
        }
        if (dev.isRemovable) {
            GtkWidget *flag = gtk_label_new(NULL);
            gtk_label_set_markup(GTK_LABEL(flag), "<span color='#64ff64'>[REMOVABLE]</span>");
            gtk_box_append(GTK_BOX(details_box), flag);
        }

        gtk_box_append(GTK_BOX(card_box), details_box);

        // -- Actions Row --
        GtkWidget *actions_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_widget_set_margin_top(actions_box, 8);

        GtkWidget *wipe_btn = gtk_button_new_with_label("Wipe Drive");
        gtk_widget_add_css_class(wipe_btn, "destructive-action"); // Standard GTK class? maybe not, but "destructive-action" is common in Adwaita
        
        // Copy path for callback
        char* path_copy = g_strdup(dev.path.c_str());
        g_signal_connect_data(wipe_btn, "clicked", G_CALLBACK(on_wipe_clicked), path_copy, free_device_path, (GConnectFlags)0);

        gtk_box_append(GTK_BOX(actions_box), wipe_btn);
        gtk_box_append(GTK_BOX(card_box), actions_box);

        gtk_box_append(GTK_BOX(container_box), frame);
    }
}

static void on_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "ZeroTrace");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 700);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    // --- Header ---
    GtkWidget *header_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_top(header_bar, 10);
    gtk_widget_set_margin_bottom(header_bar, 10);
    gtk_widget_set_margin_start(header_bar, 20);
    gtk_widget_set_margin_end(header_bar, 20);
    
    GtkWidget *title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label), "<span size='xx-large' weight='bold' color='#40a4ff'>ZeroTrace</span> <span size='large' color='grey'>| Secure Data Cleanup</span>");
    gtk_box_append(GTK_BOX(header_bar), title_label);

    // Spacer to push button to right
    GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(spacer, TRUE);
    gtk_box_append(GTK_BOX(header_bar), spacer);

    GtkWidget *refresh_btn = gtk_button_new_with_label("Refresh Devices");
    gtk_box_append(GTK_BOX(header_bar), refresh_btn);

    gtk_box_append(GTK_BOX(main_box), header_bar);
    gtk_box_append(GTK_BOX(main_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

    // --- Content ---
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_box_append(GTK_BOX(main_box), scrolled_window);

    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(content_box, 20);
    gtk_widget_set_margin_bottom(content_box, 20);
    gtk_widget_set_margin_start(content_box, 30);
    gtk_widget_set_margin_end(content_box, 30);
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), content_box);

    // Wire up refresh
    g_signal_connect_swapped(refresh_btn, "clicked", G_CALLBACK(refresh_device_list), content_box);

    // Initial load
    refresh_device_list(content_box);

    gtk_window_present(GTK_WINDOW(window));
}

void runGui() {
    // We use G_APPLICATION_DEFAULT_FLAGS. 
    // ID must be valid domain style.
    GtkApplication *app = gtk_application_new("com.zerotrace.client", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    
    // We pass 0 argc here for simplicity, but main could pass args if we changed signature
    g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
}
