#include <gtk/gtk.h>


void disegna_rettangolo(GtkWidget *widget, cairo_t *cr, double x, 
				double y, double larghezza, double altezza,int i,turno_t *app1);

void scrivi_nome_carta(turno_t *app1,cairo_t *cr,int posi,int x);

void crea_disegno(GtkWidget *widget,cairo_t *cr,int num_gc);



