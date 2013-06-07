#include <iostream>
#include <fstream>

bool carica_board(carta_t tavola[]);

bool salva_partita(turni testa,carta_t tavola[],int num_gc,turno_t *punta_turni);

bool carica_partita(turni &testa,carta_t tavola[],int &num_gc,turno_t *&punta_turni);
