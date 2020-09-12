/*
************************************************************************************************************
* Programa originalmente   (modo texto) escrito por Angelito Monteiro Goulart                              *
* Modificado para GTK+     (modo grafico)       por mazinsw <mazin.z@hotmail.com>                          *
************************************************************************************************************
* Este programa acende leds conectados na porta paralela do pino 2 até o 8.                                *
* os leds são associados a resistores de 470R para segurança da porta, e o GND vai do pino 18 até o 25     *
************************************************************************************************************
*                                      TERMO DE RESPONSABILIDADE										   *
*                     Não me responsabilizo por nenhum dano causado tanto no equipamento,                  *
*                           como na montagem/instalação inadequada dos circuitos.                          *
************************************************************************************************************
* para compilar é necessário a biblioteca GTK+ 2.0 ca so não tenha 'apt-get install libgtk2.0-dev'         *
* comando para compilar gcc -Wall -o gparport gparport.c `pkg-config --libs --cflags gtk+-2.0`             *
* para executar use sudo ./gparport																		   *
************************************************************************************************************
***********************(<-Por favor matenha os creditos do programa caso o modifique->)*********************
************************************************************************************************************
* veja o script original do Angelito Monteiro Goulart.                                                     *
* http://www.vivaolinux.com.br/script/Acendedor-de-leds-pela-porta-paralela                                *
************************************************************************************************************
*/

#include <math.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include <sys/io.h>

GtkWidget *ledbutton[8];
int led[8];

static gboolean on_expose_event(GtkWidget *widget, 
	GdkEventExpose *event, gpointer data)
{
	//biblioteca cairo responsável pelo desenho na janela
	cairo_t *cr;
	cr = gdk_cairo_create(widget->window);
	cairo_set_source_rgb(cr, 0, 0, 0);
	int i;
	for(i = 0; i < 8; i++)
	{
		//desenhando os leds
		if(led[i])
		{
			cairo_set_source_rgb(cr, 255, 0, 0);
		}
		else
		{
			cairo_set_source_rgb(cr, 0, 0, 0);
		}
		cairo_arc(cr, 60*i + 30, 25, 15, 0, 2*M_PI);
		cairo_fill(cr);
	}
	cairo_destroy(cr);
	return FALSE;
}

void buttonclicked(GtkWidget *widget, gpointer button)
{
	GdkRegion *region;
	int porta;
	int i;
	
	for(i = 0; i < 8; i++)
	{
		if(ledbutton[i] == button)
		{
			porta = inb(0x378);
			//caso esteja ligado
			if(led[i])
			{
				led[i] = 0;
				outb(porta - pow(2, i), 0x378);//desliga o led (i + 1)
			}
			else
			{
				led[i] = 1;//led ligado
				outb(porta + pow(2, i), 0x378);//liga o led (i + 1)
			}	
			//chama o evento de desenho dos leds
			region = gdk_drawable_get_clip_region(widget->window);
			gdk_window_invalidate_region(widget->window, region, TRUE);
		}
	}
}

int main( int argc, char *args[])
{
	GtkWidget *window;
	GtkWidget *frame;
	
	ioperm(0x378, 3, 1);//inicia a porta paralela
	outb(0x00, 0x378);
	gtk_init(&argc, &args);//inicia o gtk

	//criando a janela
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Porta paralela");
	gtk_window_set_default_size(GTK_WINDOW(window), 480, 90);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	
	//criando o painel
	frame = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), frame);
	
	char label[6];
	int i;
	for(i = 0; i < 8; i++)
	{
		//criando os botoes
		sprintf(label, "Led %d", i + 1);
		ledbutton[i] = gtk_button_new_with_label(label);
		gtk_widget_set_size_request(ledbutton[i], 60, 35);
		gtk_fixed_put(GTK_FIXED(frame), ledbutton[i], 60*i, 55);
		
		led[i] = 0; //led desligado		
		//comando dos botoes
		g_signal_connect(ledbutton[i], "clicked",
			G_CALLBACK(buttonclicked), ledbutton[i]);
	}
	
	//evento acionado assim que a janela é exposta
	g_signal_connect(G_OBJECT(window), "expose-event",
      G_CALLBACK(on_expose_event), NULL);
	//evento acionado quando clicamos em fechar
	g_signal_connect_swapped(G_OBJECT(window), "destroy",
		G_CALLBACK(gtk_main_quit), NULL);
		
	gtk_widget_set_app_paintable(window, TRUE);
	gtk_widget_show_all(window);
	
	gtk_main();
	
	return 0;
}
