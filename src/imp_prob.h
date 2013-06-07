#include <iostream>
#include <cstring>
#include <gtk/gtk.h>
#include <cstdlib>
#include <ctime>

void gestione_imprevisti_probabilita(turno_t *&punta_turni,carta_t tavola[],const char *nome_carta);

void seleziona_imprevisti(GtkLabel *label,turno_t *&punta_turni,carta_t tavola[]);

void seleziona_probabilita(GtkLabel *label,turno_t *&punta_turni,carta_t tavola[]);
