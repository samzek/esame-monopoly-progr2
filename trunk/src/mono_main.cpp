/**@mainpage 
*Questo programma permette a un massimo di 4 utenti di giocare a 
* una versione particolare e non riconosciuta di un gioco da tavolo famosissimo
* di nome Monopoly. 

* In questa versione alternativa il giocatore non si troverà
* a fronteggiare le solite vie del classico Monopoly ma bensì aziende di informatica,
* social network,smartphone e tanto altro.
*
* Metti alla prova le tue doti di imprenditore e informatico, compra, vendi, scambia
* le carte giuste e vinci la partita.
*
* Il gioco è strutturato su 5 file: mono_main.cpp,imp_prob.cpp, fileIO.cpp, manip.cpp, classifica.cpp
* Il programma sfrutta un file di setup per permette anticipatamente in fase di caricamento di sapere
* quali i dettaglio di ogni carta utilizzabile nel gioco.
*
* Per lanciare il programma da terminare digitare il comando:
*
* ./Monopoly
*
* Per maggiori informazioni su implementazione e una visita 
* corretta della documentazione si consiglia di partire dalla funzione ::main
*
*  @author Zecchini Samuele
*/
#include <gtk/gtk.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include "strutta_dati.h"
#include "manip.h"
#include "fileIO.h"
#include "imp_prob.h"
#include "classifica.h"

using namespace std;

#ifdef DEBUG_MODE
/* Maschera per il tracing/debugging */
unsigned int MASK = 3 ;
#endif

/** Creazione dell'oggetto builder che permette di interagire con la GUI*/
GtkBuilder *builder;
/** inizializzazione della testa della lista che gestisce i turni*/
turni testa=NULL;
/** puntatore che tiene traccia di quale turno è in corso e di
* e di chi sta giocando in un determinato momento
*/
static turno_t *punta_turni;
/*Variabile privata al file che conta il numero di giocatori presenti
* nella partita
*/
static int num_gc;
/** Implementazione tramite array delle struttura dati ::carta_t
* mi rappresenta l'insieme delle carte o più precisamente l'insieme delle aziende 
* acquistabili e non all'interno del gioco. Struttura su cui un 
* un giocatore si può muovere tramite il campo posizione_nelle_carte
* contenuto nella struttura ::turno_t. Array caricato da un file di testo 
* di setup chiamato "setup_board_carte.txt"
*/
carta_t tavola[MAX_BOARD];
/** puntatore di appoggio utile quando effettuo operazioni che prevedono la partecipazione
* di più personaggi contemporaneamente, quale ad esempio quella ::handler_scambia_carte
* in cui ho bisogno di avere un riferimento su più giocatori 
*/
static turno_t *app;

/**inzializza la sensibilita dei pulsanti
* funzione privata riservata al singolo file mono_main.cpp
* permette di attivare o disattivare un pulsante in base al
* valore attribuito al parametro booleno sensitive
*/
static void inizializza_pulsanti(GtkBuilder *builder,bool sensitive)
{
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"dado")),sensitive);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"fine turno")),sensitive);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"carte")),sensitive);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"scambia")),sensitive);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"compra")),sensitive);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"compra_az")),sensitive);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"classifica")),sensitive);

}
/** Funzione privata che permette di aggiornare l'oggetto label
* che visualizza il valore residuo di ogni giocatore in base 
* al valore attribuito alla variabile val_res
*/
static void aggiorna_label_credito(GtkBuilder *builder,int val_res)
{
	GtkLabel *label=GTK_LABEL(gtk_builder_get_object(builder,"credito"));
	ostringstream oss;
	oss<<"credito residuo: "<<val_res;
	gtk_label_set_text(label,oss.str().c_str());
	oss.str("");
	oss.clear();

}
/** controlla lo stato dei radio button all'interno della finestra
* di selezione dei personaggi, contestualemente mi crea la lista circolare
* dei turni in base al numero di giocatori scelto
*/
static void controlla_radio(GtkBuilder *builder)
{
 	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "2 giocatori"))))
 	{
 	num_gc=2;
	crea_turni(testa,num_gc);
 	}
    else if(gtk_toggle_button_get_active(
	 GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "3 giocatori"))))
	 {
	 num_gc=3;
	 crea_turni(testa,num_gc);
	 }
    else if(gtk_toggle_button_get_active(
	 GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "4 giocatori"))))
	 {
	 num_gc=4;
	 crea_turni(testa,num_gc);
	 }
	 punta_turni=testa;
	D1(cout<<"STAMPA DEI TURNI:";stampa_turni(testa));
}

/**Funzione di appoggio privata che permette di comunicare messaggi 
* generici all'utente tramite un opportuna dialog box
*/
static void mostra_messaggi(const char *str_princ)
{
    GtkLabel *label_principale = 
	GTK_LABEL(gtk_builder_get_object(builder, "label_mes")) ; 
	char stringa[100];
	strcpy(stringa,str_princ);
    gtk_label_set_text(label_principale,stringa);
    GtkWidget *error_alert = 
	GTK_WIDGET(gtk_builder_get_object(builder, "messaggi")) ;
    gtk_widget_show_all(error_alert) ;
}

/**Funzione privata che acquisisce i nomi dalle entry apposite e aggiunge gli elementi
* alla lista dei turni e in caso di fallimento dovuto ad un errato inserimento
* ritorna false e visualizza un messaggio di errore svuotando anche la lista.
* La funzione ritorna un valore booleano, il valore corrisponde o meno al verificarsi
* di un errore in questa fase quindi: true errore presente, false errore assente
*/
static bool acquisisci_nomi(GtkBuilder *builder,bool errore)
{
	 turno_t *q=testa;
	 int conta=0;
	 const char *steve=gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"steve")));
	 errore=aggiungi_elemento(steve,q,S,conta);
	 if(errore)
	 {
	 	elimina_lista(testa);
	 	return true;
	 
	 }
	 const char *linus=gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"linux")));
	 errore=aggiungi_elemento(linus,q,L,conta);
	 if(errore)
	 {
	 	elimina_lista(testa);
	 	return true;
	 
	 }
	 //se il sono stati inseriti un numero corretto 
	 // di valori nella lista allora la funzione ritorna
	 //false e comunica che non ci sono stati errori
	 else if(conta==num_gc)
	 	return false;
	 const char *bil=gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"bill")));
	 errore=aggiungi_elemento(bil,q,B,conta);
	 if(errore && conta!=num_gc)
	 {
	 	elimina_lista(testa);
	 	return true;
	 }
	 else if(conta==num_gc)
	 	return false;
	 const char *mark=gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"mark")));
	 errore=aggiungi_elemento(mark,q,M,conta);
	 if(errore && conta!=num_gc)
	 {
	 	elimina_lista(testa);
	 	return true;
	 }
	 else if (conta==num_gc) 
	 	return false; 
}

/**Segnale che chiude la finestra in cui si trova il pulsante premuto */
extern "C" void hide_main_window(GtkButton *button, gpointer data)
{
	gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

/**funzione ausiliaria privata che setta l'immagine del giocatore
*a cui spetta giocare in determinato turno
*/
static void setta_immagine(GtkBuilder *builder,turno_t *punta_turni,const char *str)
{
	GtkImage *image=GTK_IMAGE(gtk_builder_get_object(builder,str));
	switch(punta_turni->giocatore.pers)
	{
		case S:
			gtk_image_set_from_file(image,"Steven-Jobs-9354805-2-402.jpg");
			break;
		case L:
			gtk_image_set_from_file(image,"torvalds.jpg");
			break;
		case B:
			gtk_image_set_from_file(image,"images.jpeg");
			break;
		case M:
			gtk_image_set_from_file(image,"mark.jpeg");
			break;	
	}
	
	ostringstream oss;
	oss<<punta_turni->giocatore.nome;
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"nome_giocatore")),oss.str().c_str());
	oss.str("");
	oss.clear();
}

/** Funzione ausiliaria privata che permette di settare tutte le label 
* con i paramentri relativi ad una determinata carta su cui si trova
* il giocatore
*/
static void setta_label_immagine(GtkBuilder *builder)
{
	int prezzo;
	int prezzo_azioni;
	char nome[DIM_STRINGHE];
	colore_t colore;
	GtkLabel *label_nome=GTK_LABEL(gtk_builder_get_object(builder,"nome_carta"));
	GtkLabel *label_prezzo=GTK_LABEL(gtk_builder_get_object(builder,"prezzo"));
	GtkLabel *label_proprietario=GTK_LABEL(gtk_builder_get_object(builder,"prop"));
	GtkLabel *label_prezzo_azioni=GTK_LABEL(gtk_builder_get_object(builder,"prezzo_azioni"));
	
	giocatore_t *giocatore=individua_carta(punta_turni,tavola,prezzo,nome,colore,prezzo_azioni);
	//cout<<colore<<endl;
	ostringstream oss;
	oss<<nome;
	gtk_label_set_text(label_nome,oss.str().c_str());
	oss.str("");
	oss.clear();
	oss<<prezzo;
	gtk_label_set_text(label_prezzo,oss.str().c_str());
	oss.str("");
	oss.clear();
	oss<<prezzo_azioni;
	gtk_label_set_text(label_prezzo_azioni,oss.str().c_str());
	oss.str("");
	oss.clear();
	if(giocatore==NULL)
		gtk_label_set_text(label_proprietario,"nessuno");
	else
	{
		oss<<giocatore->nome;
		gtk_label_set_text(label_proprietario,oss.str().c_str());
		oss.str("");
		oss.clear();
	}
	setta_immagine(builder,punta_turni,"attore");
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"foto")),tavola[punta_turni->posizione_nelle_carte].nome_f);
}

/* Funzione di appoggio privata che ripristina la posizione delle pedine
* in base al valore attribuito alle variabili coord_x e coord_y
*/
static void ripristina_posizione_pedine(GtkBuilder *builder,GtkWidget *widget,int coord_x,int coord_y)
{
	gtk_layout_move(GTK_LAYOUT(gtk_builder_get_object(builder,"layout1")),widget,coord_x,coord_y);
	gtk_widget_set_visible(widget,true);
}
/** Segnale di conferma dei personaggi scelti in base al numero di giocatori.
* visualizza le miniature delle pedine e le sistema in posizione corretta
* tramite ::setta_label_immagine e ::ripristina_posizione_pedine
* di partenza
*/
extern "C" void handler_ok_personaggi(GtkButton *button,gpointer user_data)
{
	elimina_lista(testa);
	controlla_radio(builder);
	
	assert(punta_turni->giocatore.valore_residuo>0);
	
	bool errore=false;
	errore=acquisisci_nomi(builder,errore);
	if(errore)
		mostra_messaggi("Attenzione! Manca il nome a uno o più giocatori");
	else
	{
		inizializza_pulsanti(builder,true);
		setta_label_immagine(builder);
		
		GtkWidget *P1=GTK_WIDGET(gtk_builder_get_object(builder,"P1"));
		ripristina_posizione_pedine(builder,P1,punta_turni->coord_x,punta_turni->coord_y);
		
		GtkWidget *P2=GTK_WIDGET(gtk_builder_get_object(builder,"P2"));
		ripristina_posizione_pedine(builder,P2,punta_turni->coord_x,punta_turni->coord_y);
		
		if(num_gc==3 || num_gc>3){
			GtkWidget *P3=GTK_WIDGET(gtk_builder_get_object(builder,"P3"));
			ripristina_posizione_pedine(builder,P3,punta_turni->coord_x,punta_turni->coord_y);
			}
		if(num_gc==4){
			GtkWidget *P4=GTK_WIDGET(gtk_builder_get_object(builder,"P4"));
			ripristina_posizione_pedine(builder,P4,punta_turni->coord_x,punta_turni->coord_y);
			}
	}
	gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
	
	aggiorna_label_credito(builder,punta_turni->giocatore.valore_residuo);
	
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"dichiara_fallimento")),true);
}

/** segnale che rende utilizzabile il quarto personaggio all'utente
* (solo in caso di scelta di 4 giocatore)
*/
extern "C" void handler_4_giocatori(GtkButton *button,gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"mark")),true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"bill")),true);
}

/** segnale che rende utilizzabile i primi 3 personaggi agli utenti (solo per 3 giocatori)
*/
extern "C" void handler_3_giocatori(GtkButton *button,gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"bill")),true);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"mark")),false);
}

/** Segnale che permette di scegliere solo tra i primi 2 personaggi
* nel caso si giochi in 2 soli giocatori
*/
extern "C" void handler_2_giocatori(GtkButton *button,gpointer user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"bill")),false);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"mark")),false);
}

/** Segnale che apre la dialogbox per la scelta dei personaggi 
* e azzera le proprietà della precendente partita
*/
extern "C" void handler_new_match(GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *ins_dialog = GTK_WIDGET(gtk_builder_get_object(builder,"scegli_personaggio"));
	gtk_widget_show_all(ins_dialog);
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "2 giocatori"))))
	{
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"bill")),false);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"mark")),false);
	}
	
	//azzera le proprieta accumulate nella precendente partita
	azzera_proprieta(tavola);
	
	D2(cout<<"inizia nuova partita completato con successo"<<endl);
}
/** Funzione privata che consente la scelta della miniatura del giocatore
* a cui spetta giocare
*/
static GtkWidget * scegli_widget(GtkBuilder *builder,turno_t *punta_turni)
{
	switch(punta_turni->giocatore.pers)
	{
		case S:
			return GTK_WIDGET(gtk_builder_get_object(builder,"P1"));	
		case L:
			return GTK_WIDGET(gtk_builder_get_object(builder,"P2"));
		case B:
			return GTK_WIDGET(gtk_builder_get_object(builder,"P3"));
		case M:
			return GTK_WIDGET(gtk_builder_get_object(builder,"P4"));	
	}

}
/** funzione privata che controlla il fallimento di un determinato 
* giocatore comunicando che sia il caso di reale fallimento
* sia il caso di obbligo di vendita di alcune proprietà, utilizzando
* la funzione ::verifica_fallimento
*/
static void controlla_fallimento(GtkBuilder *builder)
{
	bool fallimento=verifica_fallimento(punta_turni->giocatore.valore_residuo,punta_turni->giocatore.conta_propr);
	if(fallimento){
		mostra_messaggi("Hai Perso!! è il momento di dichiarare fallimento");
		inizializza_pulsanti(builder,false);
	}
	else
		if(punta_turni->giocatore.valore_residuo<0)
		{
			mostra_messaggi("Mi dispiace ma il tuo credito è inferiore a 0 vedi alcune delle carte dal menu visualizza carte");
		inizializza_pulsanti(builder,false);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"carte")),true);
		}
}
/** Funzione privata che mi consente di settare l'immagine del dado 
* in base al valore che la selezione di numeri casuali ha generato
*/
static void aggiorna_immagine_dadi(GtkImage *dado_image,int dado)
{	
	switch(dado)
	{
		case 1:
			gtk_image_set_from_file(dado_image,"1.jpg");
			break;
		case 2:
			gtk_image_set_from_file(dado_image,"2.jpg");
			break;
		case 3:
			gtk_image_set_from_file(dado_image,"3.jpg");
			break;
		case 4:
			gtk_image_set_from_file(dado_image,"4.jpg");
			break;
		case 5:
			gtk_image_set_from_file(dado_image,"5.jpg");
			break;
		case 6:
			gtk_image_set_from_file(dado_image,"6.jpg");
			break;
	}

}
/** Segnale che gestisce il lancio dei dadi tutti gli spostamenti al lancio 
* connessi tramite ::gestione_spostamenti una funzione del file manip.cpp il settaggio 
* dei tutte le labele immagini dovuto allo spostamento il controllo sulla 
* prigione e sugli imprevisti
* e probabilità, il pagamento di un determinato affito al proprietario della carta
* su cui si è andati a posizionare tramite la funzione ::pagamento_affitto che ritorna
* un valore booleano nel caso sia riuscito o meno e controlla anche che il pagamento
* dell'affito non porti il determinato giocatore al fallimento tramite ::controlla_fallimento
*/
extern "C" void handler_lancia_dado(GtkButton *button,gpointer data)
{
	int dado_1;
	int dado_2;
	int dado=gestione_spostamenti(punta_turni,punta_turni->coord_x,punta_turni->coord_y,dado_1,dado_2);
	
	D1(cout<<"Dado: "<<dado<<endl);
	
	GtkImage *dado1=GTK_IMAGE(gtk_builder_get_object(builder,"dado1"));
	aggiorna_immagine_dadi(dado1,dado_1);
	
	GtkImage *dado2=GTK_IMAGE(gtk_builder_get_object(builder,"dado2"));
	aggiorna_immagine_dadi(dado2,dado_2);
	
	
	GtkWidget *widget=scegli_widget(builder,punta_turni);
	
	
	GtkLayout *layout=GTK_LAYOUT(gtk_builder_get_object(builder,"layout1"));
	
	if(strcmp(tavola[punta_turni->posizione_nelle_carte].nome_carta,"Prigione")==0)
	{
		mostra_messaggi("Sei stato scoperto a violare i server del governo! Vai in prigione");
		punta_turni->coord_x=15;
		punta_turni->coord_y=450;
		punta_turni->giocatore.prigione=true;
		punta_turni->move=SX;
		punta_turni->posizione_nelle_carte=10;
	}
	
	if(strcmp(tavola[punta_turni->posizione_nelle_carte].nome_carta,"imprevisti")==0 || 
		strcmp(tavola[punta_turni->posizione_nelle_carte].nome_carta,"probabilita")==0)
		{
		gestione_imprevisti_probabilita(punta_turni,tavola,tavola[punta_turni->posizione_nelle_carte].nome_carta);
		}
		
	
	gtk_layout_move(layout,widget,punta_turni->coord_x,punta_turni->coord_y);
	
	setta_label_immagine(builder);
	
	bool penalita=false;
	
	int costo_affitto;
	bool pagato=pagamento_affitto(punta_turni,tavola,penalita,costo_affitto);
	
	if(pagato)
	{	
		GtkWidget *ins_dialog = GTK_WIDGET(gtk_builder_get_object(builder,"affitto"));
		gtk_widget_show_all(ins_dialog);
		setta_immagine(builder,punta_turni,"Proprietario");
		ostringstream oss;
		if(penalita)
			oss<<"Devi pagare alla banca la penalità di "<<tavola[punta_turni->posizione_nelle_carte].affitto;
			
		else
		
		oss<<"Devi pagare a "<<tavola[punta_turni->posizione_nelle_carte].prop->nome<<" l'affitto di "<<costo_affitto;
		
		gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"mex_affitto")),oss.str().c_str()); 
	}
	
	aggiorna_label_credito(builder,punta_turni->giocatore.valore_residuo);
	
	controlla_fallimento(builder);
	
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"dado")),false);
}
/** Segnale che gestisce il passaggio del turno e tutto quello ad esso collegato
* aggioranamento delle label delle immagine e controllo delle prigione per il giocatore
* successivo che dovrà pagare la cauzione
*/
extern "C" void handler_fine_turno(GtkButton *button,gpointer data)
{
	punta_turni=punta_turni->pross;
	
	setta_label_immagine(builder);
	
	aggiorna_label_credito(builder,punta_turni->giocatore.valore_residuo);
	
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"dado")),true);
	
	
	if(punta_turni->giocatore.prigione)
	{
		mostra_messaggi("Devi pagare 500 di cauzione");
		punta_turni->giocatore.valore_residuo-=500;
		aggiorna_label_credito(builder,punta_turni->giocatore.valore_residuo);
		punta_turni->giocatore.prigione=false;
		controlla_fallimento(builder);
	}
}
/** Segnale che permette di comprare una determinata carta a costo che alcune condizioni
* siano rispettate, contiene anche un controllo che stabilisce se si posseggono
* tutte le carte dello stesso colore verificato tramite la funzione: ::controlla_colore_carte
*/
extern "C" void handler_compra_carta(GtkButton *button,gpointer data)
{
	D1(cout<<"DEBUG COMPRA CARTA: "<<endl)
	if(tavola[punta_turni->posizione_nelle_carte].prop==&(punta_turni->giocatore))
	{
		mostra_messaggi("Possiedi già questa carta");
		return;
	}
	if(punta_turni->giocatore.valore_residuo<tavola[punta_turni->posizione_nelle_carte].prezzo)
	{
		mostra_messaggi("Il tuo credito è insufficiente");
		return;
	}
	if(tavola[punta_turni->posizione_nelle_carte].colore==NC || 
		tavola[punta_turni->posizione_nelle_carte].prop!=NULL)
	{
		mostra_messaggi("Questa non è una carta acquistabile");
		return;
	}
	tavola[punta_turni->posizione_nelle_carte].prop=&(punta_turni->giocatore);
	
	D1(cout<<"proprietario: "<<endl);
	D1(cout<<tavola[punta_turni->posizione_nelle_carte].prop->nome<<endl);
	
	ostringstream oss;
	oss<<tavola[punta_turni->posizione_nelle_carte].prop->nome;
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"prop")),oss.str().c_str());
	
	punta_turni->giocatore.conta_propr++;
	punta_turni->giocatore.valore_residuo-=tavola[punta_turni->posizione_nelle_carte].prezzo;
	
	D2(cout<<"Modifiche apportate ai parametri del giocatore completate"<<endl);
	
	aggiorna_label_credito(builder,punta_turni->giocatore.valore_residuo);
	
	if(tavola[punta_turni->posizione_nelle_carte].colore!=SC)
	{
		bool carte_st_colore=controlla_colore_carte(punta_turni,tavola,punta_turni->posizione_nelle_carte);
		if(carte_st_colore)
		{
			mostra_messaggi("Possiedi tutte le proprità dello stesso colore, ora puoi comprare le azioni");
			gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"compra_az")),true);
		}
	}
	D2(cout<<"Acquisto completato con successo"<<endl);
}
/** Questo segnale mi permette di aprire una finestra di dialogo per chiedere all'utente
* se desidera realmente compiere una determinata azione 
*/
extern "C" void handler_visuallizza_dialog_conferma(GtkButton *button,gpointer data)
{
	GtkWidget *dialog=GTK_WIDGET(gtk_builder_get_object(builder,"dialog_conferm"));
	gtk_widget_show_all(dialog);
	GtkLabel *label=GTK_LABEL(gtk_builder_get_object(builder,"mex_conferma"));
	gtk_label_set_text(label,"Confermi di voler dichiarare fallimento?");
}
/** Segnale associato al pulsante dichiara fallimento che permette comunicare la propria
* sconfitta o il proprio ritiro, il giocatore che preme tale pulsante viene estratto
* dalla lista e si controlla che se è rimasto un solo giocatore, in tal caso 
* tale utente risulterà essere il vincitore della partita
*/
extern "C" void handler_dichiara_fallimento(GtkButton *button, gpointer data)
{
	assert(testa!=NULL);
	
	GtkWidget *widget=scegli_widget(builder,punta_turni);
	
	gtk_widget_set_visible(widget,false);
	
	vendi_tutto(tavola,punta_turni);
	
	estrai_giocatore(testa,punta_turni,num_gc);
	
	D2(cout<<"Giocatore estratto dalla lista con successo"<<endl);
	
	stampa_turni(testa);
	
	setta_label_immagine(builder);
	
	if(num_gc==1)
	{
		inizializza_pulsanti(builder,false);
		GtkWidget *ins_dialog = GTK_WIDGET(gtk_builder_get_object(builder,"affitto"));
		gtk_widget_show_all(ins_dialog);
		setta_immagine(builder,punta_turni,"Proprietario");
		ostringstream oss;
		
		oss<<punta_turni->giocatore.nome<<" vince la partita!";
		
		gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"mex_affitto")),oss.str().c_str());
		oss.str("");
		oss.clear();
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"dichiara_fallimento")),false);
		vendi_tutto(tavola,punta_turni);
		
		aggiorna_label_credito(builder,punta_turni->giocatore.valore_residuo);
	}
	
	gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
	
	D2(cout<<"Gestione fallimenti completata in maniera ottimale"<<endl);
}
/** segnale che permette di comprare le azioni di una certa azienda 
* nel caso in cui si rispettino alcune condizioni chiave
*/
extern "C" void handler_compra_azioni(GtkButton *button,gpointer data)
{
	D1(cout<<"INIZIO DEBUG COMPRA AZIONI: "<<endl);
	
	if(tavola[punta_turni->posizione_nelle_carte].prop!=&(punta_turni->giocatore))
	{
		mostra_messaggi("non possiedi questa carta");
		return;
	}
	if(tavola[punta_turni->posizione_nelle_carte].colore==SC)
	{
		mostra_messaggi("Questa carta non offre la possibilita di comprare azioni");
		return;
	}
	bool controllo=controlla_colore_carte(punta_turni,tavola,punta_turni->posizione_nelle_carte);
	if(!controllo)
	{
		mostra_messaggi("Non puoi comprare azioni senza possedere tutte le aziende dello stesso colore");
		return;
	}
	const char *num_az=gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"num_az")));
	istringstream is(num_az);
	char carattere[DIM_STRINGHE];
	int num;
	is>>carattere;
	
	//controllo entry non vuota
	if(strcmp(carattere,"")==0)
	{
		mostra_messaggi("Occorre inserire un valore nella entry");
		return;
	}
	else
	{
		//controllo inserimento valore significativo per quella entry
		for(int i=0;carattere[i]!='\0' && i<DIM_STRINGHE;i++)
			if(carattere[i]<'0' || carattere[i]>'9')
			{	
				mostra_messaggi("Occorre inserire valori numerici");
				return;
			}
		num=atoi(carattere);
		
		D1(cout<<"numero di azioni inserite: "<<num<<endl);
	}
	if(punta_turni->giocatore.valore_residuo<(num*tavola[punta_turni->posizione_nelle_carte].val_azioni))
		{
			mostra_messaggi("credito insufficiente per comprare nuove azioni");
			return;
		}
	tavola[punta_turni->posizione_nelle_carte].num_azioni=num;
	
	punta_turni->giocatore.valore_residuo-=num*tavola[punta_turni->posizione_nelle_carte].val_azioni;
	
	D1(cout<<"nuovo valore residuo: "<<punta_turni->giocatore.valore_residuo<<endl);
	D2(cout<<"Modifica parametri completata"<<endl);
	
	GtkWidget *ins_dialog = GTK_WIDGET(gtk_builder_get_object(builder,"affitto"));
	gtk_widget_show_all(ins_dialog);
	
	setta_immagine(builder,punta_turni,"Proprietario");
	
	ostringstream oss;
	
	oss<<"Hai comprato "<<num<<" azioni spesa totale: "<<num*tavola[punta_turni->posizione_nelle_carte].val_azioni;
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"mex_affitto")),oss.str().c_str());
	
	
	
	aggiorna_label_credito(builder,punta_turni->giocatore.valore_residuo);
	
	D2(cout<<"Compra azioni completata con successo"<<endl);
}
/** Funzione di supporto che carica la combo box con il nome
* di tutte le proprità possedute da un certo giocatore
*/
static void carica_combo(GtkComboBoxText *combo,turno_t *punta_turni)
{
	for(int i=0;i<MAX_BOARD;i++)
		if(tavola[i].prop==&(punta_turni->giocatore))
		{
			gtk_combo_box_text_insert_text(combo,-1,tavola[i].nome_carta);
		}
}
/** segnale che permette di visualizzare la dialog con l'elenco delle proprie carte
*/
extern "C" void handler_visualizza_carte(GtkButton *button,gpointer data)
{
	GtkWidget *ins_dialog=GTK_WIDGET(gtk_builder_get_object(builder,"visualizza_carte"));
	gtk_widget_show_all(ins_dialog);
	
	GtkComboBoxText *combo=GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder,"proprieta"));
	
	carica_combo(combo,punta_turni);
}
/** Segnale che in base alla scelta fatta nella ComboBox fa variare la label contenente
* il valore di vendita
*/
extern "C" void handler_cambia_ipo(GtkComboBox *widget,gpointer data)
{
	const char *nome=gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT(widget));
	int i;
	for(i=0;i<MAX_BOARD;i++)
		if(strcmp(tavola[i].nome_carta,nome)==0)
		{
			ostringstream oss;
			oss<<tavola[i].valore_ipoteca;
			gtk_label_set_text(GTK_LABEL(data),oss.str().c_str());
		}
}
/** Funzione privata che svuota la combo box */
static void rimuovi(GtkComboBoxText *combo,const char *nome_entry,const char *nome_label)
{
	gtk_combo_box_text_remove_all(combo);
	
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder,nome_entry)),"");
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,nome_label)),"0");
	
}
/** Sengale di uscita particolare associato al pulsante esci delle
* dialog che presentano ComboBox, svuota la combo e contralla che un certo 
* giocatore non debba dichiarare fallimento
*/
extern "C" void hide_and_remove(GtkButton *button,gpointer data)
{
	GtkComboBoxText *combo=GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder,"proprieta"));
	
	rimuovi(combo,"combo_entry","ipoteca");
	
	gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
	
	if(punta_turni->giocatore.valore_residuo>=0)
		inizializza_pulsanti(builder,true);
	else
	{
		if(punta_turni->giocatore.conta_propr>0 && punta_turni->giocatore.valore_residuo<=0)
			mostra_messaggi("Il gioco non può continuare se prima non vendi altre carte");
		else
		{
		mostra_messaggi("Hai perso! Devi dichiarare fallimento");
		inizializza_pulsanti(builder,false);
		}
	}
}
/** Segnale che gestisce la vendita delle carte alla banca 
* utilizza la funzione del modulo manip.h ::vendi_carta che ritorna
* un valore booleano nel caso la vendita sia riuscita o meno
*/
extern "C" void handler_vendi_carta(GtkButton *button,gpointer data)
{
	GtkComboBoxText *combo=GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder,"proprieta"));
	const char *nome=gtk_combo_box_text_get_active_text (combo);
	
	int indice_carta;
	bool vendita_riuscita=vendi_carta(punta_turni,tavola,nome,indice_carta);
	
	if(!vendita_riuscita)
	{
		mostra_messaggi("Devi selezionare una carta per potere vendere");
		return;
	}
	aggiorna_label_credito(builder,punta_turni->giocatore.valore_residuo);
	
	rimuovi(combo,"combo_entry","ipoteca");
	
	carica_combo(combo,punta_turni);

	setta_label_immagine(builder);
	
	bool verifica_st_cl=controlla_colore_carte(punta_turni,tavola,indice_carta);
	if(!verifica_st_cl)
	{
		mostra_messaggi("Vendendo questa carta hai perso tutte le azioni anche sulle altre dello stesso colore");
		svuota_azioni_stesso_colore(tavola,indice_carta);
	}
}
/** Segnale che apre la dialog per selezionare il giocatore con cui iniziare lo scambio */
extern "C" void handler_scambia_carte(GtkButton *button,gpointer data)
{
	GtkWidget *ins_dialog=GTK_WIDGET(gtk_builder_get_object(builder,"scelta_gc_scambio"));
	gtk_widget_show_all(ins_dialog);
}
/** Segnale che si occupa di gestire e controllare che la stringa inserita nella entry
* sia realmente il nome di un giocatore e fa partire la finestra di scambio in caso
* che il controllo ritorni valore positivo, utilizza la funzione fornita da manip.h
* ::cerca_nome
*/
extern "C" void handler_inizia_scambio(GtkButton *button,gpointer data)
{
	GtkEntry *entry=GTK_ENTRY(gtk_builder_get_object(builder,"giocatore_scambio_2"));
	
	const char *nome=gtk_entry_get_text(entry);
	
	if(strcmp(nome,punta_turni->giocatore.nome)==0)
	{
		mostra_messaggi("Non puoi scambiare carte con te stesso!");
		return;
	}
	app=testa;
	bool trovato=cerca_nome(testa,nome,app,num_gc);
	
	if(!trovato)
		mostra_messaggi("Giocatore inserito non presente");
	else
		{
			gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"nome_giocatore1")),punta_turni->giocatore.nome);
			gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"nome_giocatore2")),nome);
			
			GtkWidget *window=GTK_WIDGET(gtk_builder_get_object(builder,"scambia_carte"));
			gtk_widget_show_all(window);
			
			GtkComboBoxText *combo1=GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder,"combo_gc_1"));
			GtkComboBoxText *combo2=GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder,"combo_gc_2"));
			
			rimuovi(combo1,"entry_gc1","valore_carta_gc_1");
			rimuovi(combo2,"entry_gc2","valore_carta_gc_2");
			
			carica_combo(combo1,punta_turni);
			carica_combo(combo2,app);
		}	
	gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}
/** Segnale che si occupa del vero scambio delle carte attivato
* alla pressione del pulsante scambia e riuscito se e solo 
* se entrambi i giocatori hanno selezionato qualcosa dalle relative ComboBox
* contenenti l'elenco delle proprietà, utilizza la funzione fornita dall'interfaccia
* di manip.h ::effettua_scambio. Utilizza inoltre le funzioni ::carica_combo
* e ::rimuovi_combo per gestire la combo svutandola e ricaricandola
*/
extern "C" void handler_effettua_scambio(GtkButton *button,gpointer data)
{
	GtkComboBoxText *combo1=GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder,"combo_gc_1"));
	GtkComboBoxText *combo2=GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder,"combo_gc_2"));
	
	const char *nome_carta1=gtk_combo_box_text_get_active_text(combo1);
	
	if(strcmp(nome_carta1,"")==0){
		mostra_messaggi("Scegliere una carta da scambiare dalla prima lista");
		return;
	}
	const char *nome_carta2=gtk_combo_box_text_get_active_text(combo2);
	if(strcmp(nome_carta2,"")==0){
		mostra_messaggi("Sceglire una carta da scambiare dalla seconda lista");
		return;
		}
		
	int indice_carta1;
	int indice_carta2;
	effettua_scambio(tavola,nome_carta1,nome_carta2,indice_carta1,indice_carta2);
	
	assert(indice_carta1!=indice_carta2);
	D2(cout<<"Lo scambio è ritornato con successo"<<endl);
	
	rimuovi(combo1,"entry_gc1","valore_carta_gc_1");
	
	rimuovi(combo2,"entry_gc2","valore_carta_gc_2");

	carica_combo(combo1,punta_turni);
	
	carica_combo(combo2,app);
	
	setta_label_immagine(builder);
	
	bool verif_colore_gc1=controlla_colore_carte(punta_turni,tavola,indice_carta2);
	bool verif_colore_gc2=controlla_colore_carte(app,tavola,indice_carta1);
	
	//cout<<"Verifica cl: "<<verif_colore_gc1<<endl;
	if(verif_colore_gc1)
	{
		char str[DIM_MESSAGGIO];
		strcpy(str,punta_turni->giocatore.nome);
		strcat(str," ora possiedi tutte le carte di quel colore!");
		//cout<<str<<endl;
		mostra_messaggi(str);
	}
		
	if(verif_colore_gc2)
	{
		char str[DIM_MESSAGGIO];
		strcpy(str,app->giocatore.nome);
		strcat(str," ora possiedi tutte le carte di quel colore!");
		mostra_messaggi(str);
	}
	D2(cout<<"Lo scambio è stato completato"<<endl);
}
/** Segnale che si occupa del salvattaggio della partita utilizzando la funzione
* ::salva_partita fornita dal l'interfaccia del modulo fileIO.h e la funzione 
* ::mostra_messaggi per comunicare all'utente l'avvenuta riuscita o meno
* del salvataggio
*/
extern "C" void handler_salva_partita(GtkMenuItem *menuitem,gpointer data)
{
	if(testa==NULL)
	{
		mostra_messaggi("impossibile salvare, nessuna partita in corso");
		return;
	}
	if(!salva_partita(testa,tavola,num_gc,punta_turni))
		mostra_messaggi("Errore in fase di salvataggio della partita");
	else
		mostra_messaggi("Salvataggio completato");
}
/** segnale che si occupa della carica della partita utilizzando la funzione 
* ::carica_partita fornita dall'interfaccia del modulo fileIO.h
* utilizza le funzioni ::mostra_messaggi per comunicare con l'utente l'avvenuto
* caricamento o meno.
* Utilizza le funzioni ::aggiorna_label_credito, ::setta_label_immagine, 
* ::inizializza_pulsanti per modificare gli oggetti della GUI
*/
extern "C" void handler_carica_partita(GtkMenuItem *menuitem,gpointer data)
{
	if(!carica_partita(testa,tavola,num_gc,punta_turni))
		mostra_messaggi("Errore in fase di caricamento della partita");
	else
	{
		assert(testa!=NULL);
		
		mostra_messaggi("Caricamento completato");
		turno_t *app1=testa;
		turno_t *ultimo=ultimo_turno(testa);
		ultimo->pross=NULL;
		
		D2(cout<<"carica partita è ritornata un valore corretto"<<endl);
		
		while(app1!=NULL)
		{
			cout<<"crea widget"<<endl;
			GtkWidget *widget=scegli_widget(builder,app1);
			ripristina_posizione_pedine(builder,widget,app1->coord_x,app1->coord_y);
			app1=app1->pross;
		}
		ultimo->pross=testa;
		
		setta_label_immagine(builder);
		
		aggiorna_label_credito(builder,punta_turni->giocatore.valore_residuo);
		
		inizializza_pulsanti(builder,true);
		
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"dichiara_fallimento")),true);
	}
	D2(cout<<"Il caricamento della partita ha avuto successo"<<endl);
}
/** visualizza la finestra con l'insieme delle regole */
extern "C" void handler_visualizza_regole(GtkMenuItem *menuitem,gpointer data)
{
	GtkWidget *window=GTK_WIDGET(gtk_builder_get_object(builder,"regole "));
	gtk_widget_show_all(window);
}
/** link alla documentazione */
extern "C" void attiva_documentazione(GtkMenuItem *menuitem,gpointer data)
{
	system("firefox file:///home/samuele/esame-monopoly-progr2/Doc/html/index.html &");
}
/** visualizza la finestra che mostra la classifica attuale in base al 
* credito residuo
*/
extern "C" void handler_apri_classifica(GtkButton *button,gpointer data)
{
	GtkWidget *classifica=GTK_WIDGET(gtk_builder_get_object(builder,"window1"));
	gtk_widget_show_all(classifica);
}
/** Questo segnale richiama la funzione ::crea_disegno la quale mi disegna un istogramma
* in base ai crediti residui dei giocatori e fornisce un elenco delle proprietà
*/
extern "C" gboolean handler_disegna(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	if (num_gc == 0)
	return TRUE ;

    	crea_disegno(widget,cr,num_gc);

     return FALSE;
}
/** Main del programma, carica subito dal file di setup i valori delle carte 
* nell'array tavola utilizzando ::carica_board funzione fornita dall'interfaccia di fileIO.h
* inizializza la sensibilità dei pulsanti tramite la funzione ::inizializza_pulsanti
* aggiorna la label del credito residuo utilizzando la funzione ::aggiorna_label_credito
* Le funzionalita principali del programma sono gestite dai sengali:.
* ::handler_lancia_dado.
* ::handler_compra_carta.
* ::handler_compra_azioni.
* ::handler_scambia_carte.
* ::handler_visualizza_carte.
* ::handler_dichiara_fallimento.
* ::handler_disegna.
*/
int main(int argc,char *argv[])
{

	if(!carica_board(tavola))
		mostra_messaggi("errore in fase di caricamento del file di setup");
	//stampa(tavola,MAX_BOARD);
	
	/**inizializzazione gtk */
	gtk_init(&argc,&argv);
	
	builder=gtk_builder_new();
	
	/** aggiunga interfaccia grafica tris.glade */
	gtk_builder_add_from_file(builder,"monopoli4.glade",NULL);
	
	/** conessione dei segnali con l'interfaccia */
	gtk_builder_connect_signals(builder,NULL);
	
	aggiorna_label_credito(builder,DENARO);
	
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"attore")),"Omino_Monopoli.jpeg");
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"foto")),"varie3.jpeg");
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"dado1")),"1.jpg");
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"dado2")),"1.jpg");
	mostra_messaggi("Per iniziare una nuova partita: File -> Nuova partita");
	
	inizializza_pulsanti(builder,false);
	
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"dichiara_fallimento")),false);
	
	/** loop principale */
	gtk_main();

	return 0;
	
}
