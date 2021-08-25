#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <curses.h>
#include <curl/curl.h>
#include <zip.h>
#include <string.h>

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

void make_directories(char* dirName)
{
    int check;

    check = mkdir(dirName, 0777);

    // if (!check)
    // {
    //     printf("Directories created!");
    // }
    // else
    // {
    //     printf("Unable to create directories.");
    // }

    if (check)
    {
        return;
    }

    getch();
    system("dir");
    getch();
}

void unzip_file(char* filePath, char* fileInArchive, char* path)
{
    int err = 0;
    FILE *outFile;
    zip_t *z = zip_open(filePath, 0, &err);

    const char *name = fileInArchive;
    struct zip_stat st;
    zip_stat_init(&st);
    zip_stat(z, name, 0, &st);

    char *contents[st.size];

    zip_file_t *f = zip_fopen(z, name, 0);
    zip_fread(f, contents, st.size);
    zip_fclose(f);

    zip_close(z);

    //Do something with the contents
    char* str = malloc(strlen(path) + strlen(fileInArchive) + 1); 
    strcpy(str, path);
    strcat(str, fileInArchive);

    outFile = fopen(str, "wb");
    fwrite(contents, st.size, sizeof(st.size), outFile);
    fclose(outFile);

    printf("File unzipped: %s\n", fileInArchive);

    memset(contents, 0, sizeof(contents));
}

void betacraft_directories()
{
    make_directories("/home/kazu/betacraft-c");
    make_directories("/home/kazu/betacraft-c/launcher");
    make_directories("/home/kazu/betacraft-c/versions/");
    make_directories("/home/kazu/betacraft-c/versions/jsons");
    make_directories("/home/kazu/betacraft-c/bin");
    make_directories("/home/kazu/betacraft-c/bin/natives");
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

void betacraft_download()
{
    download_file("https://files.betacraft.pl/improvedjsons/bcwrapper-1.0.1-pre3.jar", "/home/kazu/betacraft-c/launcher/bcwrapper-1.0.1-pre3.jar");
    download_file("https://launcher.mojang.com/v1/objects/43db9b498cb67058d2e12d394e6507722e71bb45/client.jar", "/home/kazu/betacraft-c/versions/b1.7.3.jar");
    download_file("https://files.betacraft.pl/launcher/assets/libs-linux.zip", "/home/kazu/betacraft-c/bin/libs-linux.zip");
    download_file("https://files.betacraft.pl/launcher/assets/natives-linux.zip", "/home/kazu/betacraft-c/bin/natives/natives-linux.zip");
}

void betacraft_unzip()
{
    char* libsPath = "/home/kazu/betacraft-c/bin/";
    char* nativesPath = "/home/kazu/betacraft-c/bin/natives/";

    unzip_file("/home/kazu/betacraft-c/bin/libs-linux.zip", "jinput.jar", libsPath);
    unzip_file("/home/kazu/betacraft-c/bin/libs-linux.zip", "lwjgl.jar", libsPath);
    unzip_file("/home/kazu/betacraft-c/bin/libs-linux.zip", "lwjgl_util.jar", libsPath);

    unzip_file("/home/kazu/betacraft-c/bin/natives/natives-linux.zip", "libjinput-linux.so", nativesPath);
    unzip_file("/home/kazu/betacraft-c/bin/natives/natives-linux.zip", "libjinput-linux64.so", nativesPath);
    unzip_file("/home/kazu/betacraft-c/bin/natives/natives-linux.zip", "liblwjgl.so", nativesPath);
    unzip_file("/home/kazu/betacraft-c/bin/natives/natives-linux.zip", "liblwjgl64.so", nativesPath);
    unzip_file("/home/kazu/betacraft-c/bin/natives/natives-linux.zip", "libopenal.so", nativesPath);
    unzip_file("/home/kazu/betacraft-c/bin/natives/natives-linux.zip", "libopenal64.so", nativesPath);
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

    builder = gtk_builder_new_from_file("../glade/window_main.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    // Get pointers to widgets here
    //widgets->w_x  = GTK_WIDGET(gtk_builder_get_object(builder, "x"));
    widgets->w_webkit_webview = GTK_WIDGET(gtk_builder_get_object(builder, "webkit_webview"));
    
     // Widgets pointer are passed to all widget handler functions as the user_data parameter
    gtk_builder_connect_signals(builder, widgets);

    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(widgets->w_webkit_webview), "https://betacraft.pl/versions");

    betacraft_directories();

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
        betacraft_download();
        betacraft_unzip();
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