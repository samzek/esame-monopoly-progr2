#include "strutta_dati.h"
#include "manip.h"
#include "fileIO.h"
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <ctime>

/**Funzione "di debugging" che permette di stampa 
* l'intero array delle carte
*/
void stampa(carta_t tavola[],int i)
{
	for(int j=0;j<i;j++)
		cout<<tavola[j].nome_carta<<endl;
}
/** Funziona usata nella funziona ::carica_board e permette
* di decide quale colore attribuire a una determinata 
* carta
*/
void imposta_colore(carta_t tavola[],char colore[],int i)
{
	if(strcmp(colore,"V")==0)
		tavola[i].colore=V;
	if(strcmp(colore,"AZ")==0)
		tavola[i].colore=AZ;
	if(strcmp(colore,"RO")==0)
		tavola[i].colore=RO;
	if(strcmp(colore,"AR")==0)
		tavola[i].colore=AR;
	if(strcmp(colore,"ROS")==0)
		tavola[i].colore=ROS;
	if(strcmp(colore,"G")==0)
		tavola[i].colore=G;
	if(strcmp(colore,"VE")==0)
		tavola[i].colore=VE;
	if(strcmp(colore,"BL")==0)
		tavola[i].colore=BL;
	if(strcmp(colore,"SC")==0)
		tavola[i].colore=SC;
	if(strcmp(colore,"NC")==0)
		tavola[i].colore=NC;
}
/** Questa funzione ritorna il puntatore
* all'ultimo elemento della lista dei turni
*/
turno_t* ultimo_turno( turni testa )
{
    turno_t* pp = testa;
    turno_t* prec = testa->pross;
    while( prec->pross != pp )
    {
    		prec = prec->pross;
    }
    return prec;
}
/** Si occupa di creare la lista circolare dei turni.
*
*crea i turni in base al numero di giocatori inseriti
* il numero di giocatori corrisponde al numero di elementi
* della lista dei turni.
*/
void crea_turni(turni &testa,int num_giocatori)
{
	assert(num_giocatori!=0);
	
	for(int i=0;i<num_giocatori;i++)
	{
		turno_t *p=new turno_t;
		p->posizione_nelle_carte=0;
		p->move=SX;
		p->coord_x=LIMITE_DX_PX;
		p->coord_y=LIMITE_DO_PX;
		strcpy(p->giocatore.nome,"VUOTO");
		p->giocatore.valore_residuo=DENARO;
		p->giocatore.conta_propr=0;
		p->giocatore.prigione=false;
		if(testa==NULL)
		{
			testa=p;
			p->pross=testa;
		}
		else
		{
			turno_t *pp=ultimo_turno(testa);
			p->pross=pp->pross;
			pp->pross=p;
		}
	}
}
/** Funzione "di Debugging" che permette di stampare
* l'intera lista dei turni
*/
void stampa_turni(turni testa)
{
	turno_t* curr=testa;
	if(testa==NULL)
		cout<<"lista vuota"<<endl;
	else
	{
		do
		{
			cout<<"identificatore: "<<curr->giocatore.nome<<endl;
			curr=curr->pross;
		}
		while(curr!=testa);
	}
}
/** Elimina ogni elemento presente nella lista dei turni
*/
void elimina_lista(turni &testa)
{
	if(testa==NULL)
		return;
	//fermo la circolarità della lista
	turno_t *app=ultimo_turno(testa);
	app->pross=NULL;
     while (testa != NULL) {
           turno_t *q = testa->pross ;

           delete testa ;  //Attenzione: dopo la delete non posso dereferenziare testa!
           testa = q ;  
     }
}
/** Modifica il parametro nome della struttura ::giocatore_t andando a inserire
* i valori che l'utente ha inserito da tastiera nelle entry
*/
bool aggiungi_elemento(const char *stringa,turno_t * &p,personaggio_t persona,int &conta)
{
	D1(cout<<"INIZIO DEBUG ACQUSIZIONE NOMI: "<<endl);
	
	 istringstream s(stringa);
	 char nomeG[DIM_STRINGHE];
	 s>>nomeG;
	 if((nomeG[0]>='a' && nomeG[0]<='z') || (nomeG[0]>='A' && nomeG[0]<='Z'))
	 {
	 	strcpy(p->giocatore.nome,nomeG);
	 	D1(cout<<"NOME: "<<p->giocatore.nome<<endl);
	 	p->giocatore.pers=persona;
	 	p=p->pross;
	 	conta++;
	 	return false;
	 }	 
	 s.clear();
	 return true;
}

/** Individua la carta sulla tavola in cui un certo giocatore si trova in determinato momento
* i valore di ritorno è un puntatore al proprietario di quella determinata carta
*/
giocatore_t *individua_carta(turno_t *punta_turni,const carta_t tavola[], int &prezzo,char nome[],colore_t &colore,int &prezzo_azioni)
{
	strcpy(nome,tavola[punta_turni->posizione_nelle_carte].nome_carta);
	prezzo=tavola[punta_turni->posizione_nelle_carte].prezzo;
	colore=tavola[punta_turni->posizione_nelle_carte].colore;
	prezzo_azioni=tavola[punta_turni->posizione_nelle_carte].val_azioni;
	return tavola[punta_turni->posizione_nelle_carte].prop;
}

/** Gestisce il lancio del dato attraverso la generazione di 2 numeri pseudocasuali
* gestisce lo spostamento in termini di pixel che una certa pedina deve compiere dopo
* il lancio dei dati
*/
int gestione_spostamenti(turno_t *punta_turni,int &coord_x,int &coord_y,int &dado_1,int &dado_2)
{	
	D1(cout<<"INIZIO DEBUG SPOSTAMENTI: "<<endl);
	/** Questa variabile rappresenta un piccolo arrotondamento di pixel da effettuare quando si passa
	* da uno dei 4 angoli in quanto queste caselle non sono proporzionali con le altre e questo comporta 
	* durante la fase di spostamento dei piccoli errori e spostamenti inesatti delle pedine che col continuare
	* del gioco si propagano generando spostamenti visibilmente errati i valori sono scelti in maniera
	* arbitraria in base alla certa "curva da effettuare"
	*/
	int offset;
	srand(time(0));
	dado_1=rand()%6+1;
	dado_2=rand()%6+1;
	int dado=dado_1+dado_2;
	punta_turni->posizione_nelle_carte+=dado;
	D1(cout<<"posi nelle carte: "<<punta_turni->posizione_nelle_carte<<endl);
	if(punta_turni->posizione_nelle_carte>=MAX_BOARD)
	{
		punta_turni->posizione_nelle_carte=punta_turni->posizione_nelle_carte-MAX_BOARD;
		D1(cout<<"Nuova posizione se supera 40: "<<punta_turni->posizione_nelle_carte<<endl); 
	}
	D1(cout<<"Dado: "<<dado<<endl);
	
	if(punta_turni->move==SX)
	{
		coord_x-=(PIXEL_CARTA*dado);
		if(coord_x<LIMITE_SX_PX)
		{
			//utilizzo la funzione abs in quanto uno spostamento verso 
			//SX mi portebbe ad avere valori della x < 0
			int app=abs(coord_x);
			//cout<<"posi: "<<endl;
			//cout<<app<<endl;
			offset=30;
			coord_y-=(app+offset);
			coord_x=5;
			punta_turni->move=UP;
			if(coord_y<LIMITE_UP_PX)
				goto pross_cfr;
		}
	}
	else if(punta_turni->move==UP)
	{
		coord_y-=(PIXEL_CARTA*dado);
		if(coord_y<LIMITE_UP_PX)
		{
			pross_cfr:
			//utilizzo la funzione abs in quanto lo spostamento verso 
			//l'alto potrebbe generare un valore della y negativo
			int app=abs(coord_y);
			offset=35;
			coord_x+=(app+offset);
			coord_y=5;
			punta_turni->move=DX;
			if(coord_x>LIMITE_DX_PX)
				goto cfr_pross;
		}
	}
	else if(punta_turni->move==DX)
	{
		coord_x+=(PIXEL_CARTA*dado);
		if(coord_x>LIMITE_DX_PX)
		{
			cfr_pross:
			offset=20;
			coord_y+=(coord_x-425+offset);
			coord_x=440;
			punta_turni->move=DO;
		}
	}
	else if(punta_turni->move==DO)
	{
		coord_y+=(PIXEL_CARTA*dado);
		if(coord_y>LIMITE_DO_PX)
		{
			offset=30;
			coord_x-=(coord_y-450+offset);
			coord_y=450;
			punta_turni->move=SX;
		}
	}
	D1(cout<<"coordinate: "<<"X: "<<coord_x<<" Y: "<<coord_y<<endl);
	
	return dado;
}
/** Funzione che gestisce il pagamento dell'affitto da parte di un certo utente
* nel caso in cui esso dovesse transitare su una casella di proprietà di un altro giocatore
*/
bool pagamento_affitto(turno_t *punta_turni,carta_t tavola[],bool &penalita,int &costo_affitto)
{
	int affitto=tavola[punta_turni->posizione_nelle_carte].affitto;
	//pagamento obbligato alla banca(da aggiungere)
	if(strcmp(tavola[punta_turni->posizione_nelle_carte].nome_carta,"ZoomPass")==0 ||
		strcmp(tavola[punta_turni->posizione_nelle_carte].nome_carta,"PayPal")==0)
		{
		punta_turni->giocatore.valore_residuo-=affitto;
		penalita=true;
		costo_affitto=affitto;
		return true;
		}
	if(tavola[punta_turni->posizione_nelle_carte].prop==NULL)
		return false;
	else
	{
		char nome[DIM_STRINGHE];
		strcpy(nome,tavola[punta_turni->posizione_nelle_carte].prop->nome);
		if(strcmp(nome,punta_turni->giocatore.nome)==0)
			return false;
		else
		{
			if(tavola[punta_turni->posizione_nelle_carte].num_azioni!=0)
			{
				affitto+=tavola[punta_turni->posizione_nelle_carte].num_azioni*
						tavola[punta_turni->posizione_nelle_carte].val_azioni;	
				cout<<"affitto con azioni: "<<affitto<<endl;
			}
		}	
	}
	punta_turni->giocatore.valore_residuo-=affitto;
	tavola[punta_turni->posizione_nelle_carte].prop->valore_residuo+=affitto;
	costo_affitto=affitto;
	return true;
}
/** verifica che un certo giocatore in un determinato momento si ancora 
* in graado di continuare la partita
*/
bool verifica_fallimento(int valore_residuo,int conta_prop)
{
	if(valore_residuo<=0 && conta_prop==0)
		return true;
	else
		return false;
}
/** funzione utilizzata dal segnale ::handler_dichiara_fallimento che estrae il giocatore
* dalla lista dei turni il giocatore che in quell'istante ha premuto il pulsante
* dichiara fallimento
*/
void estrai_giocatore(turni &testa,turno_t *&punta_turni,int &num_gc)
{
	assert(testa!=NULL && num_gc!=0);
	
	turno_t *app=testa;
	if(testa==punta_turni)
	{
		turno_t *ultimo=ultimo_turno(testa);
		testa=testa->pross;
		punta_turni=punta_turni->pross;	
		ultimo->pross=testa;
	}
	else
	{
		app=app->pross;
		turno_t *prec=testa;
		while(app!=punta_turni)
		{
			app=app->pross;
			prec=prec->pross;
		}
		prec->pross=punta_turni->pross;
		punta_turni=punta_turni->pross;	
	}
	num_gc--;
	delete app;
}
/** azzera completamente tutte le proprietà di ogni giocatore, utilizzata dal segnale ::handler_new_match */
void azzera_proprieta(carta_t tavola[])
{
	for(int i=0;i<MAX_BOARD;i++)
		if(tavola[i].prop!=NULL)
			tavola[i].prop=NULL;
}
/** svuota tutte le proprietà di un certo giocatore che decide o è costretto a dichiarare fallimento 
* utilizzata dal segnale ::handler_dichiara_fallimento
*/
void vendi_tutto(carta_t tavola[],turno_t *punta_turni)
{
	for(int i=0;i<MAX_BOARD;i++)
		if(tavola[i].prop!=NULL && strcmp(tavola[i].prop->nome,punta_turni->giocatore.nome)==0)
					tavola[i].prop=NULL;
}
/** Funzione utilizzata per andare a verificare che un determinato giocatore abbia o meno
* tutte le carte dello stesso colore, ritorna un valore booleano.
*/
bool controlla_colore_carte(turno_t *punta_turni,carta_t tavola[],int indice)
{
	int conta_stesso_colore=0;
	
	assert(tavola[indice].prop==&(punta_turni->giocatore));
	
		colore_t cl=tavola[indice].colore;
		for(int i=0;i<MAX_BOARD;i++)		
			if(tavola[i].prop==&(punta_turni->giocatore) && cl==tavola[i].colore)
				conta_stesso_colore++;
				
	if(conta_stesso_colore==2 && (tavola[indice].colore==V ||
							tavola[indice].colore==BL))
		return true;
	else
	{
		if(conta_stesso_colore==3)
			return true;
	}
	return false;
}
/** setaccia all'interno della lista circolare tutti i nomi dei giocatori
* andando a vedere se uno di quello inseriti in partenza è uguale a quello 
* inserito nella entry dello scambia carte
*/
bool cerca_nome(turni testa,const char *nome,turno_t *&app,int num_gc)
{
	int i=0;
	while(i<num_gc)
	{
		if(strcmp(app->giocatore.nome,nome)==0)
		{
			return true;
		}
		//cout<<"nome in lista: "<<app->giocatore.nome<<endl;
		//cout<<"nome inserito: "<<nome<<endl;
		app=app->pross;
		i++;
	}
	return false;
}
/** vende una determinata carta scelta dalla combo box e ne azzera i parametri */
bool vendi_carta(turno_t *punta_turni,carta_t tavola[],const char *nome,int &indice)
{
	int i;
	for(i=0;i<MAX_BOARD;i++)
		if(strcmp(tavola[i].nome_carta,nome)==0)
		{
			break;
		}
	if(i==MAX_BOARD)
	{
		return false;
	}
	tavola[i].prop=NULL;
	
	tavola[i].num_azioni=0;
	
	punta_turni->giocatore.valore_residuo+=tavola[i].valore_ipoteca;
	
	punta_turni->giocatore.conta_propr--;
	
	indice=i;
	
	return true;

}
/** effettua lo scambio delle carte tra 2 giocatori*/
void effettua_scambio(carta_t tavola[],const char *nome1,const char *nome2,int &indice_carta_1,int &indice_carta_2)
{
	giocatore_t *app;
	for(int i=0;i<MAX_BOARD;i++)
	{
		if(strcmp(tavola[i].nome_carta,nome1)==0)
			indice_carta_1=i;
		if(strcmp(tavola[i].nome_carta,nome2)==0)
			indice_carta_2=i;
	}
	app=tavola[indice_carta_1].prop;
	tavola[indice_carta_1].prop=tavola[indice_carta_2].prop;
	tavola[indice_carta_2].prop=app;
	
	tavola[indice_carta_1].num_azioni=0;
	tavola[indice_carta_2].num_azioni=0;
}
/** funzione utilizzata da::carica_partita per andare a scegliere a quale personaggio
* o movimento corrispondesse un certo valore riportato nel file di salvataggio
*/
void seleziona_personaggio_movimento(turno_t *app,int val,bool scelta_pers_active)
{
	switch(val)
	{
		case 0:
			if(scelta_pers_active)
				app->giocatore.pers=S;
			else
				app->move=SX;
			break;
		case 1:
			if(scelta_pers_active)
				app->giocatore.pers=L;
			else
				app->move=UP;
			break;
		case 2:
			if(scelta_pers_active)
				app->giocatore.pers=B;
			else
				app->move=DX;
			break;
		case 3:
			if(scelta_pers_active)
				app->giocatore.pers=M;
			else
				app->move=DO;
			break;
	}
}
/** Funzione che in caso cui un giocatore goda delle possibilità di comprare azioni
* ma vende una delle carte che compongono l'insieme delle carte dello stesso colore
* ogni azione anche nelle carte precendenti viene persa
*/
void svuota_azioni_stesso_colore(carta_t tavola[],int indice)
{
	for(int i=0;i<MAX_BOARD;i++)
		if(tavola[i].colore==tavola[indice].colore)
			tavola[i].num_azioni=0;
}
