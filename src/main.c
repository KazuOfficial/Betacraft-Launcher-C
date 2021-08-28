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

char *betacraft_directories_array[] = 
{
    "/home/kazu/betacraft-c",
    "/home/kazu/betacraft-c/launcher",
    "/home/kazu/betacraft-c/versions/",
    "/home/kazu/betacraft-c/versions/jsons",
    "/home/kazu/betacraft-c/bin",
    "/home/kazu/betacraft-c/bin/natives"
};

char *betacraft_unzip_array_libs[] = 
{
    "jinput.jar",
    "lwjgl.jar",
    "lwjgl_util.jar"
};

char *betacraft_unzip_array_natives[] = 
{
    "libjinput-linux.so",
    "libjinput-linux64.so",
    "liblwjgl.so",
    "liblwjgl64.so",
    "libopenal.so",
    "libopenal64.so"
};

char *betacraft_download_array[] = 
{
    "https://files.betacraft.pl/improvedjsons/bcwrapper-1.0.1-pre3.jar",
    "https://launcher.mojang.com/v1/objects/43db9b498cb67058d2e12d394e6507722e71bb45/client.jar",
    "https://files.betacraft.pl/launcher/assets/libs-linux.zip",
    "https://files.betacraft.pl/launcher/assets/natives-linux.zip"
};

char *betacraft_download_array_destination[] = 
{
    "/home/kazu/betacraft-c/launcher/betacraft_wrapper.jar",
    "/home/kazu/betacraft-c/versions/b1.7.3.jar",
    "/home/kazu/betacraft-c/bin/libs-linux.zip",
    "/home/kazu/betacraft-c/bin/natives/natives-linux.zip"
};

const gchar *nickname;
static const char *pagefilename;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

void create_directories(char* dirName)
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

    char* str = malloc(strlen(path) + strlen(fileInArchive) + 1); 
    strcpy(str, path);
    strcat(str, fileInArchive);

    outFile = fopen(str, "wb");
    fwrite(contents, st.size, sizeof(st.size), outFile);
    fclose(outFile);

    printf("File unzipped: %s\n", fileInArchive);

    memset(contents, 0, sizeof(contents));
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

    builder = gtk_builder_new_from_file("../glade/window_main.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    // Get pointers to widgets here
    //widgets->w_x  = GTK_WIDGET(gtk_builder_get_object(builder, "x"));
    widgets->w_webkit_webview = GTK_WIDGET(gtk_builder_get_object(builder, "webkit_webview"));
    
     // Widgets pointer are passed to all widget handler functions as the user_data parameter
    gtk_builder_connect_signals(builder, widgets);

    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(widgets->w_webkit_webview), "https://betacraft.pl/versions");

    // Create directories for the launcher
    for (int i = 0; i<sizeof(betacraft_directories_array) / sizeof(betacraft_directories_array[0]); i++)
    {
        create_directories(betacraft_directories_array[i]);
    }

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

        // Download required files
        for (int i = 0; i<sizeof(betacraft_download_array) / sizeof(betacraft_download_array[0]); i++)
        {
            download_file(betacraft_download_array[i], betacraft_download_array_destination[i]);
        }

        // Unzip libs
        for (int i = 0; i<sizeof(betacraft_unzip_array_libs) / sizeof(betacraft_unzip_array_libs[0]); i++)
        {
            unzip_file("/home/kazu/betacraft-c/bin/libs-linux.zip", betacraft_unzip_array_libs[i], "/home/kazu/betacraft-c/bin/");
        }

        // Unzip natives
        for (int i = 0; i<sizeof(betacraft_unzip_array_natives) / sizeof(betacraft_unzip_array_natives[0]); i++)
        {
            unzip_file("/home/kazu/betacraft-c/bin/natives/natives-linux.zip", betacraft_unzip_array_natives[i], "/home/kazu/betacraft-c/bin/natives/");
        }

        // Launch the game
        system("java -cp /home/kazu/betacraft-c/launcher/betacraft_wrapper.jar:/home/kazu/betacraft-c/versions/b1.7.3.jar:/home/kazu/betacraft-c/bin/lwjgl.jar:/home/kazu/betacraft-c/bin/lwjgl_util.jar:/home/kazu/betacraft-c/bin/jinput.jar pl.moresteck.BCWrapper username=Kazu___ sessionid= gameDir=/home/kazu/betacraft-c/ versionName=b1.7.3 frameName=Minecraft width=854 height=480 assetsDir= nativesDir=/home/kazu/betacraft-c/bin/natives/");
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