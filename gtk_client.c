#include <gtk/gtk.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static void destroy_event(GtkWidget *widget, gpointer data) {
  gtk_main_quit();
}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
  gtk_main_quit();
  return FALSE;
}

//To compile append `pkg-config --cflags --libs gtk+-2.0`
int main(int argc, char *argv[]) {
  GtkWidget *window;
  GtkWidget *table;
  GtkWidget *view;
  GtkWidget *view2;
  GtkWidget *file;
  GtkWidget *quit;
  GtkWidget *button;
  GtkWidget *scrolledwindow;
  GtkWidget *scrolledwindow2;
  GtkWidget *menubar;
  GtkWidget *filemenu;

  GtkTextMark *mark;
  GtkTextBuffer *main_buffer, *buffer;
  GtkTextIter main_start, main_end, start, end, iter;

  //GdkColor color;

  gchar *utf8;
  gchar *text;
  //gchar *array;
  gsize length;
  GError *err = NULL;
  gsize *g_bytes_read = NULL; 
  gsize *g_bytes_written = NULL;

  int sockfd, portno, yes;
  ssize_t bytes_read, bytes_written; 
  struct hostent *server; 
  struct sockaddr_in serv_addr/*, cli_addr*/;
  //char buffer[4096];

	if(argc < 3) {
	  fprintf(stderr, "Ussage: %s + IP Address + port No.\n", argv[0]);
	  exit(0);
	}

  sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if(sockfd < 0) {
	  printf("SOCKET(-1) error ---> %s.\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

	if(sockfd == 0) {
	  printf("SOCKET(0) error ---> %s.\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  bzero((char *) &serv_addr, sizeof(serv_addr)); 
  server = gethostbyname(argv[1]); 
	if(server == NULL) {
	  fprintf(stderr, "No such host.\n");
	  printf("%s\n", strerror(errno)); 
	  exit(EXIT_FAILURE);
	}

  portno = atoi(argv[2]);
  serv_addr.sin_family = AF_INET; 
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length); 
  serv_addr.sin_port = htons(portno);

	if(connect(sockfd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) { 
	  fprintf(stderr, "%s. CONNECT()\n", strerror(errno));
	  exit(EXIT_FAILURE);
	} 
  else { 
		printf("Made a connection to %s\n", inet_ntoa(serv_addr.sin_addr)); 
	}

  gtk_init(&argc, &argv);

  void callback(GtkWidget *widget, gpointer data) {

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view2));
    main_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

    gtk_text_buffer_get_bounds(buffer, &start, &end);
    text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    // Insert the typed text into main view once it's sent
    gtk_text_buffer_get_start_iter(main_buffer, &iter);
    gtk_text_buffer_insert(main_buffer, &iter, text, -1);
    gtk_text_buffer_insert(main_buffer, &iter, "\n", -1);

    // Delete what you've typed once you've sent it
    gtk_text_view_get_buffer(GTK_TEXT_VIEW(view2));
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gtk_text_buffer_delete(buffer, &start, &end);

  }

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), NULL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 500);
  gtk_widget_show(window);

  table = gtk_table_new(10, 10, TRUE);
  gtk_container_add(GTK_CONTAINER(window), table);
  gtk_widget_show(table);

  menubar = gtk_menu_bar_new();
  gtk_table_attach_defaults(GTK_TABLE(table), menubar, 0, 10, 0, 1);
  gtk_widget_show(menubar);

  filemenu = gtk_menu_new();
  gtk_widget_show(filemenu);

  file = gtk_menu_item_new_with_label("File");
  quit = gtk_menu_item_new_with_label("Quit");
  gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);
  gtk_widget_show(quit);
  gtk_widget_show(file);

  view = gtk_text_view_new();
  gtk_widget_show(view);

  button = gtk_button_new_with_label("Send");
  gtk_table_attach_defaults(GTK_TABLE(table), button, 8, 9, 9, 10);
  gtk_widget_show(button);

  //gdk_color_parse("blue2", &color);
  //gtk_widget_modify_bg(GTK_WIDGET(menubar), GTK_STATE_NORMAL, &color);

  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_SHADOW_IN);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), view);
  gtk_table_attach_defaults(GTK_TABLE(table), scrolledwindow, 1, 9, 2, 7);
  gtk_widget_show(scrolledwindow);

  view2 = gtk_text_view_new();
  gtk_widget_show(view2);

  scrolledwindow2 = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow2), GTK_SHADOW_OUT);
  gtk_container_add(GTK_CONTAINER(scrolledwindow2), view2);
  gtk_table_attach_defaults(GTK_TABLE(table), scrolledwindow2, 1, 8, 9, 10);
  gtk_widget_show(scrolledwindow2);

  int send() {
    for( ; ; ) {

      char buffer[4096];
      //printf("Message: ");
      fgets(buffer, sizeof(buffer), stdin);
      printf("message: %s: ", buffer);
      ssize_t bytes_written = write(sockfd, buffer, strlen(buffer));

      if(bytes_written < 0) {
        printf("WRITE(-1) error ---> %s.\n", strerror(errno));
      }

      if(bytes_written == 0) {
        //printf("WRITE(0) error ---> %s.\n", strerror(errno));
        printf("Nothing was written.\n");
      }

      if(bytes_written) {
        printf("WRITE was successful.\n");
      }

      ssize_t bytes_read = read(sockfd, buffer, sizeof(buffer));
      if(bytes_read < 0) {
        //fprintf(stderr, "Error reading message from %s\n", inet_ntoa(cli_addr.sin_addr));
        printf("READ(-1) error ---> %s.\n", strerror(errno));
        exit(EXIT_FAILURE);
      }

      //Test to see if the buffer is blank.
      if(bytes_read == 0) {
        printf("READ(0) error ---> %s.\n", strerror(errno));
      }
      else {
        printf("server: %s", buffer);
      }
    }

  }

  g_signal_connect_swapped(G_OBJECT(quit), "activate", G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(G_OBJECT(button), "button_press_event", G_CALLBACK(callback), NULL);

  //g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(send), context_object);

  g_signal_connect_swapped(G_OBJECT(window), "destroy-event", G_CALLBACK(destroy_event), NULL);

  g_signal_connect_swapped(G_OBJECT(window), "delete-event", G_CALLBACK(delete_event), NULL);

  gtk_main();

  return 0;
}
