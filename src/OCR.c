#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <err.h>
#include <SDL.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "Display.h"
#include "pixel_operations.h"
#include "detection_character.h"
#include "reseaux.h"
#include "filters.h"

typedef struct Display
{
    char* path;
    SDL_Surface* image_input;
} Display;

typedef struct UserInterface
{
    GtkWindow* window;              // Main window
    GtkDrawingArea* area;           // Drawing area
    GtkLabel* label;                // Label used to display the score
    GtkLabel* charge;
    GtkFileChooser* entry;
} UserInterface;

typedef struct Window
{
    UserInterface ui;
    Display image;
} Window;


gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    Window *OCR = user_data;
    
    char *str = malloc(sizeof(char)*10);
    
    if(gtk_file_chooser_get_filename(OCR->ui.entry) != NULL)
    {
		OCR->image.image_input = IMG_Load(gtk_file_chooser_get_filename(OCR->ui.entry));
		if(!OCR->image.image_input)
		{
			OCR->image.image_input = IMG_Load("my_image.jpg");
			g_snprintf(str,10,"%s","ERREUR");
			gtk_label_set_label(OCR->ui.label,str);
		}
		else
		{
			OCR->image.path = gtk_file_chooser_get_filename(OCR->ui.entry);
			g_snprintf(str,10,"%s","");
			gtk_label_set_label(OCR->ui.label,str);
		}
	}
    
    int h = OCR->image.image_input->h;
	int w = OCR->image.image_input->w;
	
	int begin_y = (879 - h)/2;
	int begin_x = (620 - w)/2;
    
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_fill(cr);
    
    for(int i = 0; i < h; i++){
		for(int j = 0; j < w; j++){
				Uint32 pixel = get_pixel(OCR->image.image_input, j, i);
				Uint8 r, g, b;
				SDL_GetRGB(pixel, OCR->image.image_input->format, &r, &g, &b);
				float r1 = r;
				float g1 = g;
				float b1 = b;
				cairo_set_source_rgb(cr, r1/255, g1/255, b1/255);
				cairo_rectangle(cr, begin_x+j, begin_y+i, 1, 1);
				cairo_fill(cr);
		}
	}
    return FALSE;
}

void on_start(GtkButton *button, gpointer user_data)
{
	Window *OCR = user_data;
	
	FILE* fichier = NULL;

    fichier = fopen("Documents/Resultat.txt", "w+");
	
	/*OCR->image.image_input = grayscale(OCR->image.image_input);
    OCR->image.image_input = black_and_white(OCR->image.image_input);
    OCR->image.image_input = contrast(OCR->image.image_input);*/
    
    int h = OCR->image.image_input->h;
	int w = OCR->image.image_input->w;
	
	if(h == w)
	{
		g_print("%c", ocr(OCR->image.image_input));
	}
	else
	{
		SDL_Surface** liste = detection(OCR->image.image_input, 10, 5);
		size_t i = 0;
		while(liste[i] != NULL){
			fprintf(fichier, "%c", ocr(liste[i]));
			i++;
		}
		fclose(fichier);
	}
}

// Main function.
int main (int argc, char *argv[])
{
    gtk_init(NULL, NULL);

    GtkBuilder* builder = gtk_builder_new();
    GError* error = NULL;
    
    if (gtk_builder_add_from_file(builder, "OCR.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }
    
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "org.gtk.duel"));
    
    GtkLabel* valid = GTK_LABEL(gtk_builder_get_object(builder, "image_load"));
    GtkLabel* fonction = GTK_LABEL(gtk_builder_get_object(builder, "charg"));
    
    GtkFileChooser* entry = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "path"));
    GtkButton* start_button = GTK_BUTTON(gtk_builder_get_object(builder, "begin"));
    
    GtkDrawingArea* area = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "area"));
    
    Window OCR =
    {
		.image =
		{
			.path = "my_image.jpg",
			.image_input = IMG_Load("my_image.jpg"),
		},
		.ui =
		{
			.window = window,
			.area = area,
			.label = valid,
			.charge = fonction,
			.entry = entry,
		},
	};


    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), &OCR);
    g_signal_connect(start_button, "clicked", G_CALLBACK(on_start), &OCR);
    g_signal_connect(area, "draw", G_CALLBACK(on_draw), &OCR);
    

    gtk_main();
    return 0;
}
