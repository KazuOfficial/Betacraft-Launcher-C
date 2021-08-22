#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <curl/curl.h>

// Custom structure that holds pointers to widgets and user variables
typedef struct {
    GtkWidget *w_webkit_webview;
} app_widgets;

const gchar *nickname;
static const char *pagefilename;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

void init_directories()
{
    int check;
    //check = mkdir
}

void download_file(char* fileURL, char* fileName)
{
  CURL *curl_handle;
  pagefilename = fileName;
  FILE *pagefile;
 
  curl_global_init(CURL_GLOBAL_ALL);
 
  curl_handle = curl_easy_init();
 
  curl_easy_setopt(curl_handle, CURLOPT_URL, fileURL);
  curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
 
  pagefile = fopen(pagefilename, "wb");
  if(pagefile) {
 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
 
    curl_easy_perform(curl_handle);

    fclose(pagefile);
  }
 
  curl_easy_cleanup(curl_handle);
 
  curl_global_cleanup();
}

int main(int argc, char *argv[])
{
    GtkBuilder      *builder; 
    GtkWidget       *window;
    // Instantiate structure, allocating memory for it
    app_widgets     *widgets = g_slice_new(app_widgets);
    gtk_init(&argc, &argv);

    webkit_web_view_get_type();
    webkit_settings_get_type();

    builder = gtk_builder_new_from_file("glade/window_main.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    // Get pointers to widgets here
    //widgets->w_x  = GTK_WIDGET(gtk_builder_get_object(builder, "x"));
    widgets->w_webkit_webview = GTK_WIDGET(gtk_builder_get_object(builder, "webkit_webview"));
    
     // Widgets pointer are passed to all widget handler functions as the user_data parameter
    gtk_builder_connect_signals(builder, widgets);

    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(widgets->w_webkit_webview), "https://betacraft.pl/versions");

    g_object_unref(builder);

    gtk_widget_show_all(window);                
    gtk_main();
    // Free up widget structure memory
    g_slice_free(app_widgets, widgets);

    return 0;
}

void on_play_button_clicked(GtkButton *button)
{
    if (nickname != NULL)
    {
        printf("%s\n", nickname);
        download_file("https://files.betacraft.pl/launcher/assets/jsons/b1.7.3.info", "b1.7.3.info");
        //system("java -cp ");
    }
}

void on_nick_textbox_changed(GtkEntry *entry)
{
    nickname = gtk_entry_get_text(entry);
}

// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}