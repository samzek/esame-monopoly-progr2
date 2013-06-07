#include "strutta_dati.h"
#include "fileIO.h"
#include "manip.h"
#include <cstring>


/**carico l'array con il quale è implementata la struttura dati
* carta_t con valori che si trovano in file di setup il quale 
* contiene tutti i parametri che ogni carta deve possedere
* prima di iniziare il gioco
*/
bool carica_board(carta_t tavola[])
{
	D1(cout<<"INIZIO DEBUG CARICAMENTO TAVOLA"<<endl);
	
	ifstream f("setup_board_carte.txt");
	if(!f)
	{
		return false;
	}
	else
	{	
		char colore[4];
		int i=0;
		while(f>>tavola[i].nome_carta)
		{
			f>>tavola[i].prezzo>>tavola[i].val_azioni>>tavola[i].affitto;
			f>>colore>>tavola[i].nome_f;
			imposta_colore(tavola,colore,i);
			tavola[i].num_azioni=0;
			tavola[i].valore_ipoteca=tavola[i].prezzo/2;
			D1(cout<<"elementi:"<<endl);
			D1(cout<<tavola[i].nome_carta<<endl);
			D1(cout<<tavola[i].prezzo<<endl);
			D1(cout<<tavola[i].val_azioni<<endl);
			D1(cout<<tavola[i].affitto<<endl);
			D1(cout<<tavola[i].colore<<endl);
			i++;
		}
	}
	
	D2(cout<<"Carica board terminata con successo"<<endl);
	
	return true;
}
/** salva la partita corrente sul file salva_partita.txt */
bool salva_partita(turni testa,carta_t tavola[],int num_gc,turno_t *punta_turni)
{
	assert(num_gc!=0 && testa!=NULL);
	
	turno_t *app=testa;
	turno_t *ultimo=ultimo_turno(testa);
	ultimo->pross=NULL;
	
	ofstream f("salva_partita.txt");
	if(!f)
		return false;
	else
		{
			f<<num_gc<<endl;
			f<<punta_turni->giocatore.nome<<endl;
			
			while(app!=NULL)
			{
				f<<app->giocatore.nome<<endl;
				f<<app->giocatore.valore_residuo<<endl;
				f<<app->giocatore.conta_propr<<endl;
				f<<app->giocatore.pers<<endl;
				f<<app->giocatore.prigione<<endl;
				f<<app->posizione_nelle_carte<<endl;
				f<<app->coord_x<<endl;
				f<<app->coord_y<<endl;
				f<<app->move<<endl;
				for(int i=0;i<MAX_BOARD;i++)
					if(tavola[i].prop==&(app->giocatore))
					{
						f<<i<<endl;
						f<<tavola[i].num_azioni<<endl;
					}
				f<<-1<<endl;
			app=app->pross;
			}	
		}
		ultimo->pross=testa;
		return true;
}
/** carica l'ultima partita salvata */
bool carica_partita(turni &testa,carta_t tavola[],int &num_gc,turno_t *&punta_turni)
{
	D1(cout<<"INIZIO DEBUG CARICAMENTO: "<<endl);

	ifstream f("salva_partita.txt");
	if(!f)
		return false;
	else
		{
			char nome[DIM_STRINGHE];
			elimina_lista(testa);
			f>>num_gc;
			crea_turni(testa,num_gc);
			stampa_turni(testa);
			f>>nome;
			turno_t *app=testa;
			turno_t *ultimo=ultimo_turno(testa);
			ultimo->pross=NULL;
			
			while(app!=NULL)
			{
				f>>app->giocatore.nome;
				D1(cout<<"nome GC: "<<app->giocatore.nome<<endl);
				f>>app->giocatore.valore_residuo;
				D1(cout<<"valore_residuto"<<app->giocatore.valore_residuo<<endl);
				f>>app->giocatore.conta_propr;
				D1(cout<<"Numero proprietà: "<<app->giocatore.conta_propr<<endl)
				int pers;
				f>>pers;
				seleziona_personaggio_movimento(app,pers,true);
				f>>app->giocatore.prigione;
				f>>app->posizione_nelle_carte;
				D1(cout<<"Posizione nelle carte: "<<app->posizione_nelle_carte<<endl);
				f>>app->coord_x;
				f>>app->coord_y;
				int move;
				f>>move;
				seleziona_personaggio_movimento(app,move,false);
				int i;
				D2(cout<<"Fino a qui OK caricamento dati giocatore completato"<<endl);
				while(true)
				{
					f>>i;
					if(i==-1)
						break;
					if(i>=40)
						return false;
					cout<<i<<endl;
					tavola[i].prop=&(app->giocatore);
					D1(cout<<"Proprietario:"<<tavola[i].prop->nome<<endl);
					f>>tavola[i].num_azioni;
					D1(cout<<"Numero di azioni: "<<tavola[i].num_azioni<<endl);
				}
				D2(cout<<"Ok caricamento proprietà"<<endl);
				app=app->pross;
			}
			app=testa;
			while(app!=NULL)
			{
				if(strcmp(nome,app->giocatore.nome)==0);
				{
					punta_turni=app;
					D1(cout<<"carico turno: "<<punta_turni->giocatore.nome<<endl);
					break;
				}
				app=app->pross;
			}
			ultimo->pross=testa;
			D2(cout<<"sistemo ultimo turno riportandolo circolare la lista ok!"<<endl);
		}
		
		return true;
}
