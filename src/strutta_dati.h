#include <iostream>
#include <fstream>

using namespace std;


//#define DEBUG_MODE

#ifdef DEBUG_MODE

extern unsigned int MASK ;

#define DBG(A, B) {if ((A) & MASK) {B; } }

#else

/* Definendo questa macro si disattivano le assert se la macro
    DEBUG_MODE non è definita */
#define NDEBUG
#define DBG(A, B)

#endif

#define D0(a)
#define D1(a) DBG(1, a)
#define D2(a) DBG(2, a)

#include <cassert>

/** dimensione massima delle stringhe */
const int DIM_STRINGHE=20;
/** denaro di partenza del gioco */
const int DENARO=10000;
/** massimo numero di carte presenti nel gioco */
const int MAX_BOARD=40;
/** massima dimensione di una stringa per i messaggi all'utente */
const int DIM_MESSAGGIO=100;
/** grandezza un pixel di una carta sulla mappa utile per gestire i movimenti
* delle pedine
*/
const int PIXEL_CARTA=41;
/** limite sinistro per la pedina in pixel */
const int LIMITE_SX_PX=15;
/** limite superiore per la pedina in pixel */
const int LIMITE_UP_PX=25;
/** limite destro per la pedina in pixel */
const int LIMITE_DX_PX=425;
/** limite inferiore per la pedina in pixel */
const int LIMITE_DO_PX=450;
/** descrive la lunghezza massima della stringa contenente il nome del
* file dalla foto di ogni carta
*/
const int DIM_STR_FILE=20;


/** enumerato che rappresenta i possibili personaggi utilizzabili nel gioco */
enum personaggio_t {S,L,B,M};
/** enumerato che rappresenta il colore di ogni carta
* nello specifico i valari finali stanno:
* SC: senza colore (insieme di carte che sono acquistabili ma che non hanno colore)
* NC: non comprabili insieme di carte quali la prigione o lo start che non sono acquistabili
* 	e non hanno colore
* gli altri sono i colori relativi ad ogni carta presenti sulla tavola da gioco
*/
enum colore_t{V,AZ,RO,AR,ROS,G,VE,BL,SC,NC};
/** enumerato che stabilisce il  movimento direzionale che una certa pedina
* deve seguire nel suo tragitto
*/
enum movimento_t{SX,UP,DX,DO};


/** struttura dati che rappresenta il singolo giocatore con tutte
* le caratteristiche ad esso connesse quali il nome, il valore_residuo
* il numero di proprietà, il fatto che si trova in prigione o meno
* e il personaggio da lui scelto per giocare
*/
struct giocatore_t
{
	char nome[DIM_STRINGHE];
	int valore_residuo;
	int conta_propr;
	bool prigione;
	personaggio_t pers;
};

/** struttura dati che mi rappresenta una carta con tutte la carattestiche ad essa 
* connesse. Stuttura implementata tramite un array di 40 elementi(MAX_BOARD)\
* il quali conterrà già tutti i singoli parametri di ogni carta perchè
* caricati tramite un file di setup
*/
struct carta_t
{
	char nome_carta[DIM_STRINGHE];
	int prezzo;
	int num_azioni;
	int val_azioni;
	int affitto;
	int valore_ipoteca;
	char nome_f[DIM_STR_FILE];
	giocatore_t *prop;
	colore_t colore;
};

/** Struttura dati che rappresenta un turno all'interno del quale si trovano
* il nome del giocatore di che sta giocando in un determinato turno
* la sua posizione nella carte, quindi su che carta si trova, le coordinate x,y
* che mi indicano la posizione in pixel sulla mappa del giocatore
* struttura dati implementata tramite lista circolare semplice che conterrà un numero
* di elementi pari al numero di giocatori
*/
struct turno_t
{
	giocatore_t giocatore;
	/** variabile che mi stabilisce su quale carta un certo giocatore si trova
	* è l'elemento di unione tra la struttura turno_t e carta_t permette a un certo
	* giocatore di muoversi nell'insieme delle carte, in sostanza rappresenta un indice dell'array
	* con cui la struttura carta_t è implementata
	*/
	int posizione_nelle_carte;
	movimento_t move;
	int coord_x;
	int coord_y;
	turno_t *pross;
};

typedef turno_t *turni;
