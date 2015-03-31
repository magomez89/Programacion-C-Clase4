#include "../includes/concesionario.h"
#include <gtk/gtk.h>
#include <string.h>

// GLOBAL AND PRIVATE VARIABLES
static struct concesionario *con;
static int id;
static int selected;

// Specifications of functions
int leeFichero(char *file, struct concesionario *con);
int escribeFichero(char *file, struct concesionario *con);

// --> BEGIN FUNCTIONS ADD OR DELETE DATA
static void funcion_delete(GtkWidget *widget, gpointer data)
{
	curso_concesionario_attr_unset_coche(con, selected);
	gtk_combo_box_text_remove((GtkComboBoxText *)data, selected);
	gtk_combo_box_set_active(GTK_COMBO_BOX(data), 0);
	selected = 0;
}

static void add_car(GtkButton *widget, gpointer data)
{
	GtkWidget **entry;
	struct coche *c;
	GtkWidget *entry_ptr_marca;
	GtkWidget *entry_ptr_matricula;
	const gchar *matricula, *marca;
	
	// Casting gpointer data to GtkWidget **
	entry = (GtkWidget **)data;
	// Save first position entry in entry_ptr_marca
	entry_ptr_marca = entry[0];
	entry_ptr_matricula = entry[1];
	
	// Save in gchar text of type GtkEntry* (casting)
	marca = gtk_entry_get_text((GtkEntry *)entry_ptr_marca);
	matricula = gtk_entry_get_text((GtkEntry *) entry_ptr_matricula);
	
	c = curso_coche_alloc();
	if(c == NULL)
		return;
	curso_coche_attr_set_u32(c, CURSO_COCHE_ATTR_ID, id++);
	curso_coche_attr_set_str(c, CURSO_COCHE_ATTR_MATRICULA, matricula);
	curso_coche_attr_set_str(c, CURSO_COCHE_ATTR_MARCA, marca);
	curso_concesionario_attr_set_coche(con, CURSO_CONCESIONARIO_ATTR_COCHE, c);

}

// <-- END FUNCTIONS ADD OR DELETE DATA

static void combo_selected(GtkWidget *combo, gpointer data)
{
	selected =  gtk_combo_box_get_active((GtkComboBox *)combo);
}

// --> BEGIN FUNCTIONS FILES

static void funcion_load(char *fsel)
{
	curso_concesionario_free(con);
	con = curso_concesionario_alloc();
	leeFichero(fsel, con);
	id = curso_concesionario_attr_get_u32(con,
		CURSO_CONCESIONARIO_ATTR_NUM_COCHES);
	curso_concesionario_attr_set_str(con, CURSO_CONCESIONARIO_ATTR_DUENO,
					 "Manuel Antonio");

}

int leeFichero(char *file, struct concesionario *con)
{
	FILE *fichero;
	char line[256];
	struct coche *c;

	// Open file. Read mode	
	fichero = fopen(file, "r");
	
	if (fichero == NULL){
		printf("Error file NOT OPEN\n");
		return -1;
	}
	
	// Iteration to get information about cars and save in the structs
	while (fgets(line, sizeof(line),fichero)){
		char *p;
		int i = 0;
		c = curso_coche_alloc();
		p = strtok(line, ",");
		while (p != NULL){
			switch (i){
			case 0:
				curso_coche_attr_set_str(c,
					 CURSO_COCHE_ATTR_MARCA, p);
				break;
			case 1:
				curso_coche_attr_set_str(c, 
						CURSO_COCHE_ATTR_MATRICULA, p);
				break;
			case 2:
				curso_coche_attr_set_u32(c, 
						CURSO_COCHE_ATTR_ID, atoi(p));
				break;
			}
			i++;
			p = strtok(NULL, ",");
		}
		curso_concesionario_attr_set_coche(con,
					CURSO_CONCESIONARIO_ATTR_COCHE, c);
	}
	
	if (fclose(fichero)){
		printf("ERROR: file NOT CLOSED \n");
		return -1;
	}
	return 0;
}

static void funcion_save(char *filename)
{
	escribeFichero(filename, con);
}

int escribeFichero(char *file, struct concesionario *con)
{
	FILE *fichero;
	char line[256];
	struct coche *c;
	int i;
	
	// Open file. Write mode
	fichero = fopen(file, "w");

	if (fichero == NULL) {
		printf("ERROR. Not open\n");
		return -1;
	}
	
	// Traversing List Cars to save in to the file the cars data
	for (i = 0; i < curso_concesionario_attr_get_u32(con,
				CURSO_CONCESIONARIO_ATTR_NUM_COCHES); i++) {
		// Get car
		c = curso_concesionario_attr_get_coche(con,
			CURSO_CONCESIONARIO_ATTR_COCHE, i);
		char id[10];
		sprintf(id, "%d", curso_coche_attr_get_u32(c, CURSO_COCHE_ATTR_ID));
		// Write the Number license into the file
		fputs(curso_coche_attr_get_str(c, CURSO_COCHE_ATTR_MATRICULA), fichero);
		fputc(',', fichero);
		// Write the Brand into the file
		fputs(curso_coche_attr_get_str(c, CURSO_COCHE_ATTR_MARCA), fichero);
		fputc(',', fichero);
		fputs(id, fichero);
		fputc('\n', fichero);
	}
	if (fclose(fichero)) {
		printf("ERROR: file NOT CLOSED\n");
		return -1;
	}
	return 0;
}

//<--- END FUNCTIONS FILES


static void funcion_quit(GtkButton *boton, gpointer data)
{
	curso_concesionario_free(con);
	gtk_main_quit();
}


// --> BEGIN FUNCTIONS WINDOWS ( HANDLER)

static void show_info_window(GtkButton *widget, gpointer data)
{
	GtkWidget *window, *vbox, *image;
	static GtkWidget *ginfo;
	char info[4000];

	// Take info from curso_concesionario_snprintf
	curso_concesionario_snprintf(info, sizeof(info), con);
	// Set text label to previous info
	ginfo = gtk_label_new(info);
	image = gtk_image_new_from_file("../images/concesionario2.jpg");

	// Creation window and vbox
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	vbox = gtk_vbox_new(TRUE, 2);
	gtk_window_set_title((GtkWindow *)window, "Information");
	
	// Add vbox to window
	gtk_container_add((GtkContainer *)window, vbox);
	// Packaging widgets to vBox
	gtk_box_pack_start((GtkBox *)vbox, image, TRUE, TRUE, 0);
	gtk_box_pack_start((GtkBox *)vbox, ginfo, TRUE, TRUE, 2);
	gtk_widget_show_all(window);
}

static void show_delete_window(GtkButton *widget, gpointer data)
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *button, *combo, *image;

	// Info for deleting cars
        int i, num_coches;
        struct coche *c;
	char info_coche[4000];
 
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	vbox = gtk_vbox_new(FALSE, 2);
	combo = gtk_combo_box_text_new();
	image = gtk_image_new_from_file("../images/concesionario2.jpg");
	
	gtk_window_set_title((GtkWindow *)window, "Delete car");
	gtk_window_set_default_size(GTK_WINDOW(window), 250, 250);
	button = gtk_button_new_with_label("Delete");

	// Adding car to combo
	c = curso_coche_alloc();
	num_coches = curso_concesionario_attr_get_u32(con,
				 CURSO_CONCESIONARIO_ATTR_NUM_COCHES);
	
	// Iteration Traversing cars to add text to combo
	for(i = 0; i < num_coches; i++)
	{
		c = curso_concesionario_attr_get_coche(con,
				CURSO_CONCESIONARIO_ATTR_COCHE, i);
		
		curso_coche_snprintf(info_coche, sizeof(info_coche), c);
		// Adding text to combo.
		gtk_combo_box_text_append_text((GtkComboBoxText *)combo,
				(gchar *)info_coche);
	}
	
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
	selected = 0;
	// Size of combo is from -1 to max of cars
	gtk_widget_set_size_request(combo, -1, 50);
	gtk_widget_set_size_request(button, -1, 100);
	
	// Packaging widgets to vbox
	gtk_box_pack_start((GtkBox *)vbox, image, TRUE, TRUE, 0);
	gtk_box_pack_start((GtkBox *)vbox, combo, TRUE, TRUE, 2);
	gtk_box_pack_start((GtkBox *)vbox, button, TRUE, TRUE, 2);
	
	gtk_container_add((GtkContainer *)window, vbox);
	gtk_widget_show_all(window);

	g_signal_connect((GObject *)combo, "changed",
			(GCallback)combo_selected, (gpointer) combo);
	g_signal_connect((GObject *)button, "clicked",
			(GCallback)funcion_delete, (gpointer) combo);
}

static void show_load_window(GtkWidget *button, gpointer window)
{
	GtkWidget *dialog;
	gint res;
	
	dialog = gtk_file_chooser_dialog_new("Select a file",
		GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL,
		GTK_RESPONSE_CANCEL, NULL);

	res = gtk_dialog_run(GTK_DIALOG (dialog));

	if (res == GTK_RESPONSE_OK) {
		char *filename;
		filename = gtk_file_chooser_get_filename(
				GTK_FILE_CHOOSER(dialog));
		funcion_load(filename);
		g_free(filename);
	}
	gtk_widget_destroy(dialog);
}


static void show_save_window(GtkWidget *button, gpointer window)
{
	GtkWidget *dialog;
	GtkFileChooser *chooser;
	gint res;

	dialog = gtk_file_chooser_dialog_new("Save file",
			GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL,
			GTK_RESPONSE_CANCEL, NULL);

	chooser = GTK_FILE_CHOOSER (dialog);
	res = gtk_dialog_run (GTK_DIALOG(dialog));
	
	if (res == GTK_RESPONSE_OK) {
		char *filename;
		filename = gtk_file_chooser_get_filename(chooser);
		funcion_save(filename);
		g_free(filename);
	}
	gtk_widget_destroy(dialog);
}


static void show_about_window(GtkWidget * widget, gpointer data)
{
	GtkWidget *dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Authorised Car dealer");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.1");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),
		"Concesionario is a simple tool to manage cars");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

// <-- END FUNCTIONS WINDOWS ( HANDLER)

int main(int argc, char *argv[]){
	// GtkWidget window, box and button
	GtkWidget *window;
	GtkWidget *vbox, *hbox;
	GtkWidget *button1, *button2, *button3, *image;
	GtkWidget *hseparator, *hseparator2, *hseparator0;
	// GtkWidget menu
	GtkWidget *menubar, *filemenu;
	GtkWidget *file, *save, *load, *quit;
	GtkWidget *editmenu, *edit, *delete;
	GtkWidget *viewmenu, *view, *infoCon, *helpmenu;
	GtkWidget *about, *help;
	// GtkWidget auxiliar
	GtkWidget **entry;
	GtkWidget *entry1, *entry2;
	//GtkWidget concesionario
	GtkWidget *marca, *matricula;
		
	gtk_init(&argc, &argv);
	entry = calloc(1, 2 *sizeof(GtkWidget));
	entry1 = gtk_entry_new();
	entry2 = gtk_entry_new();
	entry[0] = entry1;
	entry[1] = entry2;

	marca = gtk_label_new("Brand:");
	matricula = gtk_label_new("Number Plate:");
	hseparator0 = gtk_hseparator_new();
	hseparator = gtk_hseparator_new();
	hseparator2 = gtk_hseparator_new();

	con = curso_concesionario_alloc();
	if (con == NULL)
		return -1;

	curso_concesionario_attr_set_str(con, CURSO_CONCESIONARIO_ATTR_DUENO, "Manuel Antonio");
	// Creation of menu
	menubar = gtk_menu_bar_new();
	filemenu = gtk_menu_new();
	editmenu = gtk_menu_new();
	viewmenu = gtk_menu_new();
	helpmenu = gtk_menu_new();
	file = gtk_menu_item_new_with_label("File");
	save = gtk_menu_item_new_with_label("Save");
	load = gtk_menu_item_new_with_label("Load");
	quit = gtk_menu_item_new_with_label("Quit");
	edit = gtk_menu_item_new_with_label("Edit");
	delete = gtk_menu_item_new_with_label("Deleting car");
	view = gtk_menu_item_new_with_label("View");
	infoCon = gtk_menu_item_new_with_label("Information");
	about = gtk_menu_item_new_with_label("About");
	help = gtk_menu_item_new_with_label("Help");

	// Add sub_menu to menu_item (file)
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
	// Add label to submenu filemenu
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), load);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
	// Add item to menubar
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit), editmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), delete);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), edit);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view), viewmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), infoCon);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), view);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), helpmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), about);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
	gtk_window_set_title((GtkWindow *)window, "Authorised car dealer");

	// Creation of widgets
	button1 = gtk_button_new_with_label("Add car");
	button2 = gtk_button_new_with_label("Information");
	button3 = gtk_button_new_with_label("Delete car");
	image = gtk_image_new_from_file("../images/concesionario1.jpg");
	vbox = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 0);

	// PACKAGING
	// Packaging widgets to vbox
        gtk_box_pack_start((GtkBox *)vbox, menubar, FALSE, FALSE, 3);
	gtk_box_pack_start((GtkBox *)vbox, image, TRUE, TRUE, 0);
	gtk_box_pack_start((GtkBox *)vbox, hbox, TRUE, TRUE, 2);
	gtk_box_pack_start((GtkBox *)vbox, hseparator0, FALSE, TRUE, 10);
	gtk_box_pack_start((GtkBox *)vbox, button1, TRUE, TRUE, 2);
	gtk_box_pack_start((GtkBox *)vbox, hseparator, FALSE, TRUE, 10);
	gtk_box_pack_start((GtkBox *)vbox, button2, TRUE, TRUE, 2);
	gtk_box_pack_start((GtkBox *)vbox, hseparator2, FALSE, TRUE, 10);
	gtk_box_pack_start((GtkBox *)vbox, button3, TRUE, TRUE, 2);

	// Packaging widget to hbox
	gtk_box_pack_start((GtkBox *)hbox, marca, TRUE, TRUE, 2);
	gtk_box_pack_start((GtkBox *)hbox, entry1, TRUE, TRUE, 2);
	gtk_box_pack_start((GtkBox *)hbox, matricula, TRUE, TRUE, 2);
	gtk_box_pack_start((GtkBox *)hbox, entry2, TRUE, TRUE, 2);

	// Add vbox to window
	gtk_container_add((GtkContainer *)window, vbox);
	
	// Handlers buttons
	g_signal_connect((GObject *)button1, "clicked",
			 (GCallback) add_car, (gpointer)entry);
	g_signal_connect((GObject *)button2, "clicked",
			 (GCallback)show_info_window, NULL);
	g_signal_connect((GObject *)button3, "clicked",
			 (GCallback)show_delete_window, NULL);
	
	// Handler window
	g_signal_connect((GObject *)window, "delete-event", 
			(GCallback)funcion_quit, NULL);
	// Handlers widget menu
	g_signal_connect((GObject *)save, "activate",
			(GCallback)show_save_window, NULL);
	g_signal_connect((GObject *)load, "activate",
			(GCallback)show_load_window, NULL);
	g_signal_connect((GObject *)quit, "activate",
			(GCallback)funcion_quit, NULL);
	g_signal_connect((GObject *)delete, "activate",
			(GCallback)show_delete_window, NULL);
	g_signal_connect((GObject *)infoCon, "activate",
			(GCallback)show_info_window, NULL);
	g_signal_connect((GObject *)about, "activate",
			(GCallback)show_about_window, NULL);

	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}

