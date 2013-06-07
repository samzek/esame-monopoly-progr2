#include "strutta_dati.h"
#include "manip.h"
#include "imp_prob.h"

using namespace std;

extern GtkBuilder *builder;

/** Funzione di gestione degli imprevisti e probabilità decide cosa scegliere in base alla carta
* su cui il giocatore si trova
*/
void gestione_imprevisti_probabilita(turno_t *&punta_turni,carta_t tavola[],const char *nome_carta)
{
	GtkWidget *ins_dialog=GTK_WIDGET(gtk_builder_get_object(builder,"messaggi"));
	GtkLabel *label=GTK_LABEL(gtk_builder_get_object(builder,"label_mes"));
	
	if(strcmp(nome_carta,"imprevisti")==0)
		seleziona_imprevisti(label,punta_turni,tavola);
		
		
	if(strcmp(nome_carta,"probabilita")==0)
		seleziona_probabilita(label,punta_turni,tavola);	
		
	gtk_widget_show_all(ins_dialog);
}
/** seleziona in maniera pseudocasuale un imprevisto e modifica i parametri del giocatore o la posizione 
* in base a quello che l'imprevisto prevede
*/
void seleziona_imprevisti(GtkLabel *label,turno_t *&punta_turni,carta_t tavola[])
{
	srand(time(0));
	int selezione=rand()%10+1;
	char messaggio[DIM_MESSAGGIO];
	switch(selezione)	
	{
		case 1:
			strcpy(messaggio,"Sei stato condannato per evasione fiscale! paga 3000 di multa e vai in galera");
			punta_turni->giocatore.valore_residuo-=3000;
			punta_turni->coord_x=15;
			punta_turni->coord_y=450;
			punta_turni->giocatore.prigione=true;
			punta_turni->move=SX;
			punta_turni->posizione_nelle_carte=10;
			break;
		case 2:
			strcpy(messaggio,"HAHAHAH! La tua pagina di facebook è stata chiusa paga 200");
			punta_turni->giocatore.valore_residuo-=200;
			break;
		case 3:
			strcpy(messaggio,"OHH NOOOO!!! I server della tua azienda si sono bloccati paga 1000 per spese di riparazione");
			
			punta_turni->giocatore.valore_residuo-=1000;
			break;
		case 4:
			strcpy(messaggio,"Le serate la Las Vegas non sono mai redditizie hai perso 1000");
			punta_turni->giocatore.valore_residuo-=1000;
			break;
		case 5:
			strcpy(messaggio,"Mi dispiace il tuo canale YOUTUBE è stato chiuso perdi 150");
			punta_turni->giocatore.valore_residuo-=150;
			break;
		case 6:
			strcpy(messaggio,"Lo sapevi che sposarti non ti avrebbe portato niente di buono paga 550 di alimenti a tua moglie");
			punta_turni->giocatore.valore_residuo-=550;
			break;
		case 7:
			strcpy(messaggio,"Linus Torvalds ti ha denunciato per avergli mandato codice non testato a dovere! PAGA 2000!");
			punta_turni->giocatore.valore_residuo-=2000;
			break;
		case 8:
			strcpy(messaggio,"Bella partita a poker ieri sera!! Guadagni 1000!");
			punta_turni->giocatore.valore_residuo+=1000;
			break;
		case 9:
			strcpy(messaggio,"Il CdA richiede un nuovo investimento di capitale! paga 500");
			punta_turni->giocatore.valore_residuo-=500;
			break;
		case 10:
			strcpy(messaggio,"Ops! Sei stato rapinato, hai perso 200");
			punta_turni->giocatore.valore_residuo-=200;
			break;
	}
	gtk_label_set_text(label,messaggio);
}
/** Seleziona la carta probabilità in maniera randomizzata e setta i vari attributi del giocatore
* o eventualmente la posizione delle pedine
*/
void seleziona_probabilita(GtkLabel *label,turno_t *&punta_turni,carta_t tavola[])
{
	srand(time(0));
	int selezione=rand()%10+1;
	char messaggio[DIM_MESSAGGIO];
	int somma_az=0;
	switch(selezione)
	{
		case 1:
			strcpy(messaggio,"Un nuovo prodotto esce sul mercato!! guadagni 700");
			punta_turni->giocatore.valore_residuo+=700;
			break;
		case 2:
			strcpy(messaggio,"Complimenti! hai vinto il premio turing! guadagni 500");
			punta_turni->giocatore.valore_residuo+=500;
			break;
		case 3:
			strcpy(messaggio,"Hai bisogno di nuove informazione sui tuoi avversari spostati fino a Wikipedia");
			punta_turni->posizione_nelle_carte=20;
			punta_turni->coord_x=5;
			punta_turni->coord_y=25;
			punta_turni->move=UP;
			break;
		case 4:
			strcpy(messaggio,"Ummmmm! Oggi sei vestito particolarmente bene, guadagni 80");
			punta_turni->giocatore.valore_residuo+=80;
			break;
		case 5:
			strcpy(messaggio,"Hai raggiunto 200.000 iscritti su YOUTUBE guadagni 250");
			punta_turni->giocatore.valore_residuo+=250;
			break;
		case 6:
			strcpy(messaggio,"Tuo figlio finalmente si sposa, paga la dote di 600 ");
			punta_turni->giocatore.valore_residuo-=600;
			break;
		case 7:
			strcpy(messaggio,"La pubblicità su Facebook da ottimi risultati guadagni 1000");
			punta_turni->giocatore.valore_residuo+=1000;
			break;
		case 8:
			strcpy(messaggio,"Il revisore contabile, scopre che hai truffato l'azienda paga 10 per ogni azione");
			
			for(int i=0;i<MAX_BOARD;i++)
				if(tavola[i].prop==&(punta_turni->giocatore))
					somma_az+=tavola[i].num_azioni;
			punta_turni->giocatore.valore_residuo-=somma_az*10;
			break;
		case 9:
			strcpy(messaggio,"Fantastici utili a fine anno!! guadagni 2000");
			punta_turni->giocatore.valore_residuo+=2000;
			break;
		case 10:
			strcpy(messaggio,"Niente da  fare perdi costantemente quote di mercato, perdi 1000");
			punta_turni->giocatore.valore_residuo-=1000;
			break;
	}
	gtk_label_set_text(label,messaggio);	
}
