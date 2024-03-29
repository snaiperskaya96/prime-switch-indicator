#include <fstream>
#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>

typedef void (*VoiceActivated_t)(GtkWidget *widget, gpointer data);

enum Gpu
{
  None,
  Intel,
  Nvidia
};

GtkWidget* switchToIntel = nullptr;
GtkWidget* switchToNvidia = nullptr;
GtkWidget* quit = nullptr;

static void log(const char* what)
{
  std::ofstream stream("/dev/kmsg");
  stream << what << std::endl;
}

static void onRestartAlert(GtkDialog *dialog, gint response_id, gpointer user_data)
{
  if (response_id == GTK_RESPONSE_ACCEPT)
  {
    system("reboot");
  }
  else
  {
    gtk_widget_destroy(GTK_WIDGET(dialog));
  }
}

static void restartAlert()
{
  GtkWidget* dialog = nullptr;
  GtkWidget* label = nullptr;
  GtkWidget* contentArea = nullptr;

  dialog = gtk_dialog_new_with_buttons ("You should restart.", nullptr, GTK_DIALOG_DESTROY_WITH_PARENT, "_OK", GTK_RESPONSE_ACCEPT, "_Cancel", GTK_RESPONSE_REJECT, NULL);
  contentArea = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  label = gtk_label_new ("You need to restart to make the switch effective. Do you want me to do it for you?");

  g_signal_connect_swapped (dialog, "response", G_CALLBACK(&onRestartAlert), dialog);
  gtk_container_add (GTK_CONTAINER(contentArea), label);
  gtk_widget_show_all (dialog);
}

static Gpu getCurrentGpu()
{
  char gpu[256] = { 0x0 };
  FILE* fp = popen("prime-select query", "r");
  fgets(gpu, sizeof(gpu), fp);
  pclose(fp);

  if (strncmp(gpu, "nvidia", 6) == 0)
  {
    return Gpu::Nvidia;
  }
  else if (strncmp(gpu, "intel", 5) == 0)
  {
    return Gpu::Intel;
  }
  else
  {
    return Gpu::None;
  }
}

static void updateVoices()
{
  if (switchToIntel && switchToNvidia)
  {
    switch (getCurrentGpu())
    {
      case Gpu::Nvidia:
        gtk_widget_show(switchToIntel);
        gtk_widget_hide(switchToNvidia);
        break;
      case Gpu::Intel:
        gtk_widget_show(switchToNvidia);
        gtk_widget_hide(switchToIntel);
        break;
    }
  }
}

static void onClick(GtkWidget *widget, gpointer data)
{
  if (widget == quit)
  {
    gtk_main_quit();
  }
  else if (widget == switchToNvidia)
  {
    if (system("prime-switch-indicator-select nvidia") != 0)
    {
      log("[prime-switch-indicator] There was an error while trying to switch to Nvidia");
    }
    else
    {
      restartAlert();
    }
    
  }
  else if (widget == switchToIntel)
  {
    if (system("prime-switch-indicator-select intel") != 0)
    {
      log("[prime-switch-indicator] There was an error while trying to switch to Intel");
    }
    else
    {
      restartAlert();      
    }
    
  }

  updateVoices();
}

static GtkWidget* setupVoice(const char* voiceName, VoiceActivated_t callback, GtkWidget* menu)
{
    GtkWidget* item = gtk_menu_item_new_with_label(voiceName);
    gtk_widget_add_events(item, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (item, "activate", G_CALLBACK (callback), NULL);
    gtk_menu_shell_insert(GTK_MENU_SHELL(menu), item, 0);
    gtk_widget_show(item);
    return item;
}

int main (int argc, char** argv)
{
    GtkWidget* indicatorMenu = nullptr;
    AppIndicator* indicator = nullptr;
    GError* error = nullptr;

    gtk_init(&argc, &argv);

    indicator = app_indicator_new ("prime-switch-indicator", "/usr/share/icons/prime-switch-indicator/indicator.png", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_icon(indicator, "/usr/share/icons/prime-switch-indicator/indicator.png");

    indicatorMenu = gtk_menu_new();
    quit = setupVoice("Quit", &onClick, indicatorMenu);
    switchToIntel = setupVoice("Switch to Intel", &onClick, indicatorMenu);
    switchToNvidia = setupVoice("Switch to Nvidia", &onClick, indicatorMenu);
    app_indicator_set_menu(indicator, GTK_MENU(indicatorMenu));
    updateVoices();

    gtk_main();

    return 0;
}