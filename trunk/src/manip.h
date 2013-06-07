void stampa(carta_t tavola[],int i);

void imposta_colore(carta_t tavola[],char colore[],int i);

void crea_turni(turni &testa,int num_giocatori);

void stampa_turni(turni testa);

turno_t *ultimo_turno(turni testa);
 
void elimina_lista(turni &testa);

bool aggiungi_elemento(const char *stringa,turno_t * &p,personaggio_t persona,int &conta);

giocatore_t * individua_carta(turno_t *punta_turni,const carta_t tavola[], int &prezzo,char nome[],colore_t &colore,int &prezzo_azioni);

int gestione_spostamenti(turno_t *punta_turni,int &coord_x,int &coord_y,int &dado_1,int &dado_2);

bool pagamento_affitto(turno_t *punta_turni,carta_t tavola[],bool &penalita,int &costo_affitto);

bool verifica_fallimento(int valore_residuo,int num_prop);

void estrai_giocatore(turni &testa,turno_t * &punta_turni,int &num_gc);

void azzera_proprieta(carta_t tavola[]);

void vendi_tutto(carta_t tavola[],turno_t *punta_turni);

bool controlla_colore_carte(turno_t *punta_turni,carta_t tavola[],int indice);

bool cerca_nome(turni testa,const char *nome,turno_t * &app,int num_gc);

bool vendi_carta(turno_t *punta_turni,carta_t tavola[],const char *nome,int &indice);

void effettua_scambio(carta_t tavola[],const char *nome1,const char *nome2,int &indice_carta_1,int &indice_carta_2);

void seleziona_personaggio_movimento(turno_t *app,int val,bool scelta_pers_active);

void svuota_azioni_stesso_colore(carta_t tavola[],int indice);

