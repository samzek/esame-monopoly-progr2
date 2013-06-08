#include "strutta_dati.h"
#include "manip.h"
#include "classifica.h"
#include <cstring>
#include <sstream>

using namespace std;

extern carta_t tavola[];
extern turni testa;

/** gestisce la creazione dei dati fondamentali per la formazione del disegno
* quali ad esempio la larghezza delle barre, l'altezza.
* Algoritmo per la creazione dei rettangoli fornito dal professor Marko Bertogna
*/
void crea_disegno(GtkWidget *widget,cairo_t *cr,int num_gc)
{
	D1(cout<<"INIZIO DEBUG CREAZIONE DISEGNO: "<<endl);
	guint larghezza_barre = gtk_widget_get_allocated_width(widget) / 
    							num_gc ;
    	D1(cout<<"larghezza barre: "<<larghezza_barre<<endl);
    	
 	larghezza_barre = larghezza_barre > 2 ? larghezza_barre - 2 : 1 ;
    
     guint altezza_max_barre = 
	static_cast<gint>(0.9 * gtk_widget_get_allocated_height(widget)) ;

	D1(cout<<"altezza rettangoli: "<<altezza_max_barre<<endl);
	
	int max_val = testa->giocatore.valore_residuo;
   	turno_t *ultimo=ultimo_turno(testa);
   	ultimo->pross=NULL;
   	turno_t *app1=testa;
   	while(app1!=NULL)
   	{
	max_val = max_val > (app1->giocatore.valore_residuo) ?
	           max_val : (app1->giocatore.valore_residuo);
	       app1=app1->pross;
	}
	ultimo->pross=testa;
	
	D1(cout<<"Max Val: "<<max_val<<endl);
	
	app1=testa;
	
     for (int i = 0 ; i < num_gc ; i++)
     {
		disegna_rettangolo(widget, cr, i * (larghezza_barre + 2) + 1, 0, 
        	larghezza_barre, 
		altezza_max_barre * (app1->giocatore.valore_residuo / 
		    static_cast<double>(max_val)),i,app1);
		app1=app1->pross;
	}
}
/** E' la vera funzione che disegna il rettangolo di ogni giocatore
* inoltre questa funzioni sopra ogni rettangolo (colorato di un differente colore in base al 
* giocatore) inserisce il nome del giocatore e il suo credito residuo e all'interno
* del rettangolo stesso visualizza l'insieme delle proprieta di un determinato giocatore
*/
void disegna_rettangolo(GtkWidget *widget, cairo_t *cr, double x, 
				double y, double larghezza, double altezza,int i,turno_t *app1)
{
    int max_altezza = gtk_widget_get_allocated_height(widget);
    cairo_rectangle(cr, x, max_altezza - 1 - y - altezza, larghezza, altezza);
    
    cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, 20);
    char str[DIM_MESSAGGIO];
    strcpy(str,app1->giocatore.nome);
    strcat(str," ");
    ostringstream os;
    os<<app1->giocatore.valore_residuo;
    strcat(str,os.str().c_str());
    os.str("");
    os.clear(); 
    int posi=max_altezza - 1 - y - altezza+20;  
    
    D1(cout<<"posizione scritta: "<<posi<<endl);
     
    switch(i)
    {
    		case 0:
    			cairo_set_source_rgb (cr, 0, 0, 1);
   			cairo_show_text (cr, str);
   			cairo_fill(cr);
   			cairo_set_source_rgb(cr,0,0,0);
   			
   			cairo_move_to(cr,x,posi);
   			
   			cairo_show_text(cr,"Proprietà:");
   			
   			scrivi_nome_carta(app1,cr,posi,x);
   			
   			break;
   		case 1:
   			cairo_set_source_rgb (cr,255,0,0);
   			cairo_show_text (cr, str);
   			cairo_fill(cr);
   			cairo_set_source_rgb(cr,0,0,0);
   			
   			cairo_move_to(cr,x,posi);
   			
   			cairo_show_text(cr,"Proprietà:");
   			
   			scrivi_nome_carta(app1,cr,posi,x);
   			
   			break;
   		case 2:
   			cairo_set_source_rgb (cr,0,255,0);
   			cairo_show_text (cr, str);
   			cairo_fill(cr);
   			cairo_set_source_rgb(cr,0,0,0);
   			
   			cairo_move_to(cr,x,posi);
   			
   			cairo_show_text(cr,"Proprietà:");
   			
   			scrivi_nome_carta(app1,cr,posi,x);
   			break;
   		case 3:
   			cairo_set_source_rgb (cr,255,255,0);
   			cairo_show_text (cr, str);
   			cairo_fill(cr);
   			cairo_set_source_rgb(cr,0,0,0);
   			
   			cairo_move_to(cr,x,posi);
   			
   			cairo_show_text(cr,"Proprietà:");
   			
   			scrivi_nome_carta(app1,cr,posi,x);
   			break;
   	}
   		cairo_fill(cr);
}
/** scrive il nome delle proprietà di un giocatore dentro il rettangolo */
void scrivi_nome_carta(turno_t *app1,cairo_t *cr,int posi,int x)
{
	for(int i=0;i<MAX_BOARD;i++)
   		if(tavola[i].prop==&(app1->giocatore))
   		{
   			posi+=20;
   			cairo_move_to(cr,x,posi);
   			cairo_show_text(cr,tavola[i].nome_carta);
   		}
}
