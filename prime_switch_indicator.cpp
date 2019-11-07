#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>
#include <fstream>

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
  GtkWidget *dialog, *label, *content_area;
  GtkDialogFlags flags;

  // Create the widgets
  flags = GTK_DIALOG_DESTROY_WITH_PARENT;
  dialog = gtk_dialog_new_with_buttons ("You should restart.", nullptr, flags, "_OK", GTK_RESPONSE_ACCEPT, "_Cancel", GTK_RESPONSE_REJECT, NULL);

  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  label = gtk_label_new ("You need to restart to make the switch effective. Do you want me to do it for you?");

  g_signal_connect_swapped (dialog,
                            "response",
                            G_CALLBACK (&onRestartAlert),
                            dialog);

  gtk_container_add (GTK_CONTAINER (content_area), label);
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
    GtkWidget* menuItem1 = gtk_image_menu_item_new_with_label(voiceName);
    gtk_widget_add_events(menuItem1, GDK_BUTTON_PRESS_MASK);
    g_signal_connect (menuItem1, "activate", G_CALLBACK (callback), NULL);
    gtk_menu_shell_insert(GTK_MENU_SHELL(menu), menuItem1, 0);
    gtk_widget_show(menuItem1);
    return menuItem1;
}

int main (int argc, char **argv)
{
    GtkWidget* indicator_menu = nullptr;
    GtkWidget* menuItem1 = nullptr;

    AppIndicator* indicator = nullptr;
    GError* error = nullptr;

    gtk_init(&argc, &argv);

    indicator = app_indicator_new ("prime-switch-indicator", "/usr/share/icons/prime-switch-indicator/indicator.png", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_icon(indicator, "/usr/share/icons/prime-switch-indicator/indicator.png");

    indicator_menu = gtk_menu_new();
    quit = setupVoice("Quit", &onClick, indicator_menu);
    switchToIntel = setupVoice("Switch to Intel", &onClick, indicator_menu);
    switchToNvidia = setupVoice("Switch to Nvidia", &onClick, indicator_menu);
    app_indicator_set_menu(indicator, GTK_MENU(indicator_menu));
    updateVoices();

    gtk_main();

    return 0;
}