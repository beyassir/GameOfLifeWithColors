#include <gtk/gtk.h>
#include <math.h>
#define cell_rows 81
#define cell_cols 155

GtkBuilder *builder = NULL; //builder pour l'associer a GTK
static cairo_surface_t *surface,*grid; // la grille et la surface 
static cairo_matrix_t matrice;//la matrice du grille
static int cols,rows, // nombre des columns dans le jeu
	langueur,largeur,	//height et largeur du grille
	courant_line,max_color = 8;
//static int game_w=100,game_h=100;
double	cell_width = 20,cell_height = 20;// longueur et largeur de cellule  
//widgets utilisé dans le projet
GtkWidget 	*popover,*draw_area,*flow,*flow2,*flowR,*color_set;
gint timer , valeur = 100; // timmer et valeur de progression du jeu en ms
//les cellules du jeu;
int Cellule[cell_rows][cell_cols];
int regles[4][4][4];
int reglecpt = 0;
//tableau de couleurs
GdkRGBA Colors[4];
gint colorcpt = 1;





/*fonction qui permet d'initialiser les cellules de la grille*/
static void init_cellule()
	{
		for(int i = 0;i < cell_rows;i++)//parcour des lignes
			{
				for(int j = 0;j < cell_cols;j++)//parcour de column
					Cellule[i][j] = 0; //initialiser toutes les cellules à 0
			}
			system("clear");	
	}

/*fonction qui permet de tracer la grille*/
static void
init_grid(void)
{
	int i;
	cairo_t *cr = cairo_create(grid);//creer la grid

	cairo_scale(cr,cell_width,cell_height);//
	cairo_get_matrix(cr,&matrice);//associer le matrice au grid

	cairo_set_line_width(cr,0.1);//la largeur du ligne 
	cairo_set_source_rgb(cr,0.8,0.8,0.8);//la ligne va etre de couleur{0.8,0.8,0.8}en rgba

	for(i=0;i<cols;++i)
		{
			cairo_move_to(cr,i,0);//passer au point (i,0)
			cairo_rel_line_to(cr,0,rows);//le point courant va etre (0,rows)
		}
	for(i=0;i<rows;++i)
		{
			cairo_move_to(cr,0,i);//passer au point (0,i)
			cairo_rel_line_to(cr,cols,0);//le point courant va etre (cols,0)
		}

	cairo_stroke(cr);//sauvgarde de tracage du grille
	cairo_destroy(cr);//destroy
	system("clear");	
}

/*fonction qui permet de configurer le jeu*/
static gboolean
configuration(GtkWidget *widget,
	GdkEventConfigure *event,
	gpointer data)
{
	GtkAllocation dems;
	gtk_widget_get_allocation(widget,&dems);
	if(surface)
		cairo_surface_destroy(surface);

	largeur=gtk_widget_get_allocated_width(widget);
	langueur=gtk_widget_get_allocated_height(widget);
	cell_width = ((double)largeur)/cols;
	cell_height = ((double)langueur)/rows;
	
	/* creer la surface */
	cairo_surface_t *temp_surface = gdk_window_create_similar_surface(
		gtk_widget_get_window(widget),
		CAIRO_CONTENT_COLOR_ALPHA,
		largeur,langueur);
	surface = cairo_surface_create_for_rectangle(temp_surface,dems.x,dems.y,dems.width,dems.height);
	cairo_surface_destroy(temp_surface);
	/* creer la grille */
	grid = cairo_surface_create_similar(
		surface,
		CAIRO_CONTENT_COLOR_ALPHA,
		largeur,langueur);
		largeur = dems.width;
		langueur = dems.height; 
	init_grid();
	system("clear");	
	return TRUE;
}

static gboolean
drawing(GtkWidget *widget,
	cairo_t *cr,
	gpointer data)
{
	GtkAllocation dems;
	gtk_widget_get_allocation(widget,&dems);
	
	
	int i,j;
	cairo_set_source_surface(cr,surface,0,0);
	cairo_set_source_rgb(cr,1,1,1);
	cairo_paint(cr);

	// draw la grille
	cairo_set_source_surface(cr,grid,0,0);
	cairo_paint(cr);
	//matrice dans laquelle on va dessiner
	cairo_set_matrix(cr,&matrice);

	cairo_matrix_t temp=matrice;
	temp.y0 = dems.y;
	cairo_set_matrix(cr,&temp);
	// draw les cellules
	for(i = 0;i < rows; i++)
	{
		for(j = 0;j < cols; j++)
		{
				if(Cellule[i][j] > 0)
				{
				gdk_cairo_set_source_rgba(cr,&Colors[Cellule[i][j]]);
				cairo_rectangle(cr,j,i,1,1);
				cairo_fill(cr);
				}
		}
	}
	system("clear");	
	return TRUE;
}

/*fonction du progression du jeu*/
gboolean game_progress_color(GtkWidget *draw_area)
{
	int Vois_G,Vois_D;
	if(courant_line < rows-1)
	{
		for(int j =0;j < cols; j++)
		{
				Vois_G = j > 0 ?  Cellule[courant_line][j-1]:0;
				Vois_D = j < cols-1 ?  Cellule[courant_line][j+1]:0;
			Cellule[courant_line+1][j] = regles[Vois_G][Cellule[courant_line][j]][Vois_D];
				
		}
			courant_line++;
			gtk_widget_queue_draw(draw_area);
		return TRUE;
	}
	else
	{
		printf("Game over.");
		return FALSE;
	}
}

/*fonction de libele selectione du flowbox */
void get_selected(GtkFlowBox *f,GtkFlowBoxChild *c,gpointer dt){
	*(int*)dt = gtk_flow_box_child_get_index(c);
	system("clear");	
}

/*fonction qui recupere les cordonnées du cellule et la colorié*/
void on_motion_notify(GtkWidget *w,GdkEventButton *ev,gpointer data)
{
	int v=0;
	printf("%g %g\n",ev->x,ev->y);
	
	flow = GTK_WIDGET(gtk_builder_get_object(builder, "color_set"));
	gtk_flow_box_selected_foreach(flow,get_selected,&v);
	
	Cellule[(int)(ev->y/cell_height)][(int)(ev->x/cell_width)]=v;

	gtk_widget_queue_draw(w);
	system("clear");	
}

//
static void
prog(GtkApplication *app,gpointer data)
{
	/* la fenetre de l'application */
	GtkWidget *window1;

	/* Creer et configurer la fenetre */
	window1 = GTK_WIDGET(gtk_builder_get_object(builder, "window2"));
	g_signal_connect(window1,"destroy",G_CALLBACK(gtk_main_quit),NULL);
	


	draw_area = GTK_WIDGET(gtk_builder_get_object(builder, "drawarea"));
	
	gtk_widget_set_size_request(draw_area,100,100);
	g_signal_connect(draw_area,"draw",
		G_CALLBACK(drawing),NULL);
	
	g_signal_connect(draw_area,"configure-event",
		G_CALLBACK(configuration),NULL);

	gtk_widget_set_events(draw_area,gtk_widget_get_events(draw_area)|GDK_POINTER_MOTION_MASK| GDK_BUTTON_PRESS_MASK);

	g_signal_connect(draw_area,"button-press-event",G_CALLBACK(on_motion_notify),NULL);

	/* afficher la fenetre */
	gtk_widget_show_all(window1);	
	system("clear");	
} 
//fonction hide 
void hide(GtkWidget *w,gchar *obj){
	GtkWidget *fen;
	fen = GTK_WIDGET(gtk_builder_get_object(builder,obj));
	gtk_widget_hide(fen);
	system("clear");	
}
//fonction show
void show(GtkWidget *w,gchar *obj)
{
	GtkWidget *fen;
	fen = GTK_WIDGET(gtk_builder_get_object(builder,obj));
	gtk_widget_show(fen);
	system("clear");	
}
//fonction new pour nouvelle partie
void new()
	{
		flow = GTK_WIDGET(gtk_builder_get_object(builder, "flow"));
		/*--supprimer les couleur des flow regle quand la redefinition des couleurs*/
		GList *children, *iter;

		children = gtk_container_get_children(GTK_CONTAINER(flow));
		for(iter = g_list_next(children); iter != NULL; iter = g_list_next(iter))
		  gtk_widget_destroy(GTK_WIDGET(iter->data));
		g_list_free(children);		
		colorcpt = 1;
		combinaison();
		reload();
		system("clear");	
	}
//fonction start pour commencer le jeu	
void start()
	{
		int test = 0;
		for(int i = 0;i<cols;i++)
		{

			if(Cellule[0][i] != 0) test = 1;
		}
		if(test == 0)
			for(int i = 0;i<cols;i++)
			{
				Cellule[0][i] = rand()%colorcpt;
			}
		if(timer==0 && colorcpt>1 ) timer = g_timeout_add(valeur,game_progress_color,draw_area);
		system("clear");	
	}
//fonction start pour interompre le jeu
void stop()
	{
		if(timer != 0)
		{
				g_source_remove(timer);
				timer = 0;
		}
		system("clear");	
	}
////fonction start pour recommencer le jeu
void reload()
	{
		stop();
		init_cellule();
		courant_line = 0;
		gtk_widget_queue_draw(draw_area);
		system("clear");	
	}
// fonction pour ajuster la vitesse de progression du jeu
static void speedfct (GtkAdjustment *adjust,
              gpointer  user_data)
{
   valeur = gtk_adjustment_get_value (adjust);
	if(timer != 0)
		{
			stop();
			start();
		}
		system("clear");	
}
//fonction qui colorie un label 
void coloriage_flow2_lbl(gint col)
	{
		GdkRGBA couleur;
		GtkWidget *label1= gtk_label_new("      ");
		gtk_widget_set_visible(label1,1);
		
		couleur = Colors[col];
		gtk_widget_override_background_color(label1,GTK_STATE_FLAG_NORMAL,&couleur);
		gtk_container_add (flow2,label1);
		system("clear");		
	}

//fonction de relativite du popover au button
void on_toggled(GtkWidget *btn)
	{
		gtk_popover_set_relative_to (popover,btn);
		system("clear");	
	}
//fonction qui colorie un button 
void coloriage_flow2_btn(gint col,int j,int k)
	{	
		
		GdkRGBA couleur;
		GtkWidget *mb = gtk_menu_button_new ();
		GtkWidget *label = gtk_label_new("      ");
		gtk_container_add (GTK_CONTAINER(mb),label);
		gtk_widget_set_visible(label,1);
		gtk_widget_set_size_request (mb, 15, 15);
		gint val = rand()%colorcpt;
		gtk_widget_override_background_color(label,GTK_STATE_FLAG_NORMAL,&Colors[val]);
		regles[col][j][k] = val;
		gtk_widget_set_visible(mb,1);
		g_signal_connect (G_OBJECT (mb), "toggled", (GCallback)on_toggled,NULL);
		gtk_menu_button_set_popover(mb,popover);	
		gtk_container_add (flow2,mb);	
		system("clear");	
	}
//combinaison des regles possible dans le jeu
void combinaison()	
	{

		flow2 = GTK_WIDGET(gtk_builder_get_object(builder, "flow2"));
		/*--supprimer les couleur des flow regle quand la redefinition des couleurs*/
		GList *children, *iter;

		children = gtk_container_get_children(GTK_CONTAINER(flow2));
		for(iter = children; iter != NULL; iter = g_list_next(iter))
		  gtk_widget_destroy(GTK_WIDGET(iter->data));
		g_list_free(children);		
		/*--supprimer les couleur des popover quand la redefinition des couleurs*/
		children = gtk_container_get_children(GTK_CONTAINER(flowR));
		for(iter = children; iter != NULL; iter = g_list_next(iter))
		  gtk_widget_destroy(GTK_WIDGET(iter->data));
		g_list_free(children);

		/*--supprimer les couleur des flow de select color quand la redefinition des couleurs*/
		color_set = GTK_WIDGET(gtk_builder_get_object(builder, "color_set"));
		children = gtk_container_get_children(GTK_CONTAINER(color_set));
		for(iter = children; iter != NULL; iter = g_list_next(iter))
		  gtk_widget_destroy(GTK_WIDGET(iter->data));
		g_list_free(children);

		for(int j = 0;j< colorcpt;j++)
		{
			GtkLabel *label= gtk_label_new("      "); 
			gtk_widget_set_visible(label,1);
			gtk_widget_override_background_color(label,GTK_STATE_FLAG_NORMAL,&Colors[j]);
			gtk_container_add (flowR,label);
			
			label= gtk_label_new("      "); 
			gtk_widget_set_visible(label,1);
			gtk_widget_override_background_color(label,GTK_STATE_FLAG_NORMAL,&Colors[j]);
			gtk_container_add (color_set,label);
		}
		for(int i = 0;i<colorcpt ;i++)
			{
				for(int j = 0;j<colorcpt ;j++)
					{
						for(int k = 0;k<colorcpt ;k++)
							{
								coloriage_flow2_lbl(i);
								coloriage_flow2_lbl(j);
								coloriage_flow2_lbl(k);
								gtk_flow_box_set_column_spacing(flow2,70);
								coloriage_flow2_btn(i,j,k);
							}
					}
			}
			system("clear");	
	}	

//fonction qui colorie un button 
void coloriage_flow2_btn1(gint col,int j,int k)
	{	
		
		GdkRGBA couleur;
		GtkWidget *mb = gtk_menu_button_new ();
		GtkWidget *label = gtk_label_new("      ");
		gtk_container_add (GTK_CONTAINER(mb),label);
		gtk_widget_set_visible(label,1);
		gtk_widget_set_size_request (mb, 15, 15);
		gint val = regles[col][j][k];
		gtk_widget_override_background_color(label,GTK_STATE_FLAG_NORMAL,&Colors[val]);
		gtk_widget_set_visible(mb,1);
		g_signal_connect (G_OBJECT (mb), "toggled", (GCallback)on_toggled,NULL);
		gtk_menu_button_set_popover(mb,popover);	
		gtk_container_add (flow2,mb);	
		system("clear");	
	}
//pour la suppression de flows et regeneration lors de l'open du file
void combinaison1()	
	{

		flow2 = GTK_WIDGET(gtk_builder_get_object(builder, "flow2"));
		/*--supprimer les couleur des flow regle quand la redefinition des couleurs*/
		GList *children, *iter;
			

		for(int iter = 1; iter < colorcpt; iter ++)
		 	{
				GtkWidget *label= gtk_label_new("      "); 
				gtk_widget_set_visible(label,1);
				flow = GTK_WIDGET(gtk_builder_get_object(builder, "flow"));
				gtk_widget_override_background_color(label,GTK_STATE_FLAG_NORMAL,&Colors[iter]);
				gtk_container_add (flow,label);
		 	}
		g_list_free(children);		

		children = gtk_container_get_children(GTK_CONTAINER(flow2));
		for(iter = children; iter != NULL; iter = g_list_next(iter))
		  gtk_widget_destroy(GTK_WIDGET(iter->data));
		g_list_free(children);		
		/*--supprimer les couleur des popover quand la redefinition des couleurs*/
		children = gtk_container_get_children(GTK_CONTAINER(flowR));
		for(iter = children; iter != NULL; iter = g_list_next(iter))
		  gtk_widget_destroy(GTK_WIDGET(iter->data));
		g_list_free(children);

		/*--supprimer les couleur des flow de select color quand la redefinition des couleurs*/
		color_set = GTK_WIDGET(gtk_builder_get_object(builder, "color_set"));
		children = gtk_container_get_children(GTK_CONTAINER(color_set));
		for(iter = children; iter != NULL; iter = g_list_next(iter))
		  gtk_widget_destroy(GTK_WIDGET(iter->data));
		g_list_free(children);

		for(int j = 0;j< colorcpt;j++)
		{
			GtkLabel *label= gtk_label_new("      "); 
			gtk_widget_set_visible(label,1);
			gtk_widget_override_background_color(label,GTK_STATE_FLAG_NORMAL,&Colors[j]);
			gtk_container_add (flowR,label);
			
			label= gtk_label_new("      "); 
			gtk_widget_set_visible(label,1);
			gtk_widget_override_background_color(label,GTK_STATE_FLAG_NORMAL,&Colors[j]);
			gtk_container_add (color_set,label);
		}
		for(int i = 0;i<colorcpt ;i++)
			{
				for(int j = 0;j<colorcpt ;j++)
					{
						for(int k = 0;k<colorcpt ;k++)
							{
								coloriage_flow2_lbl(i);
								coloriage_flow2_lbl(j);
								coloriage_flow2_lbl(k);
								gtk_flow_box_set_column_spacing(flow2,70);
								coloriage_flow2_btn1(i,j,k);
							}
					}
			}
			system("clear");	
	}	


/*fonction qui definit les regles du jeu*/
void drawmb(GtkWidget *flow,GtkFlowBoxChild *child)
	{
		gint index = gtk_flow_box_child_get_index (child);
	
		GtkWidget *mb = gtk_popover_get_relative_to(popover);GTK_MENU_BUTTON(mb);
		GtkWidget *label = gtk_bin_get_child(GTK_BIN(mb));		
		gtk_widget_override_background_color(label,GTK_STATE_FLAG_NORMAL,&Colors[index]);

		int in = gtk_flow_box_child_get_index(GTK_FLOW_BOX_CHILD (gtk_widget_get_parent(mb)));
		in/=4;
		
		regles[in/(colorcpt*colorcpt)][(in/colorcpt)%colorcpt][in%colorcpt]=index;
		gtk_widget_hide(popover);
		system("clear");	
	}

/* fonction d'ajout des couleurs */
void adcolor(GtkColorButton *widget,
               gpointer        user_data)
	{
		if(colorcpt >= 4) return;
		GtkWidget *label= gtk_label_new("      "); 
		gtk_widget_set_visible(label,1);
		flow = GTK_WIDGET(gtk_builder_get_object(builder, "flow"));
		GdkRGBA couleur;
		gtk_color_button_get_rgba (widget,&couleur);
		Colors[colorcpt++] = couleur;
		gtk_widget_override_background_color(label,GTK_STATE_FLAG_NORMAL,&couleur);
		gtk_container_add (flow,label);
		reglecpt = colorcpt*colorcpt*colorcpt;
		system("clear");	
}

/*suppression du coulour du flow box*/
void delcolor()
	{	
		GList *children, *iter;
		gint index;
		flow = GTK_WIDGET(gtk_builder_get_object(builder, "flow"));
		children = gtk_flow_box_get_selected_children(GTK_CONTAINER(flow));
		for(iter = children; iter != NULL; iter = g_list_next(iter))
			{
		  		
				index = gtk_flow_box_child_get_index (iter->data);
				if(index !=0)
				{
					memmove(Colors+index,Colors+index+1,sizeof(Colors[0])*(colorcpt-index-1));
					colorcpt--;
					gtk_widget_destroy(GTK_WIDGET(iter->data));
				}
			}
		g_list_free(children);	
		system("clear");	
	}
	
static void
save_game(char *fichiername)
{
	stop();

	FILE* fichier = NULL;

    fichier = fopen(fichiername, "wb");

    if (fichier != NULL)
    {
    	fwrite(&colorcpt,sizeof colorcpt,1,fichier);
		fwrite(Colors,sizeof(Colors),1,fichier);
		fwrite(&reglecpt,sizeof reglecpt,1,fichier);
		fwrite(regles,sizeof regles,1,fichier);

		fwrite(&cols,sizeof cols,1,fichier);
		fwrite(&rows,sizeof rows,1,fichier);
		fwrite(&courant_line,sizeof courant_line,1,fichier);
		fwrite(Cellule,sizeof Cellule,1,fichier);
    	
        // On peut lire et écrire dans le fichier
    }
    else
    {
        // On affiche un message d'erreur si on veut
        printf("Impossible d'ouvrir le fichier test.txt");
    }
	fclose(fichier);
	system("clear");
}

static void
open_game(char *fichiername)
{
	stop();
	new();
	FILE* fichier = NULL;

    fichier = fopen(fichiername, "rb");

    if (fichier != NULL)
    {
    	fread(&colorcpt,sizeof colorcpt,1,fichier);
		fread(Colors,sizeof(Colors),1,fichier);
		fread(&reglecpt,sizeof reglecpt,1,fichier);
		fread(regles,sizeof regles,1,fichier);

		fread(&cols,sizeof cols,1,fichier);
		fread(&rows,sizeof rows,1,fichier);
		fread(&courant_line,sizeof courant_line,1,fichier);
		fread(Cellule,sizeof Cellule,1,fichier);
    	// On peut lire et écrire dans le fichier
    }
    else
    {
        // On affiche un message d'erreur si on veut
        printf("Impossible d'ouvrir le fichier test.txt");
    }
	fclose(fichier);
	combinaison1();
	system("clear");
	gtk_widget_queue_draw(draw_area);
}

void on_open(GtkWidget *widget)
	{
		GtkWidget *dialog;
		stop();
GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
gint res;

dialog = gtk_file_chooser_dialog_new ("Open File",
                                      gtk_widget_get_window(widget),
                                      action,
                                      ("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      ("_Open"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

res = gtk_dialog_run (GTK_DIALOG (dialog));
if (res == GTK_RESPONSE_ACCEPT)
  {
    char *filename;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
    filename = gtk_file_chooser_get_filename (chooser);
    open_game (filename);
    g_free (filename);
  }
system("clear");
gtk_widget_destroy (dialog);
}

void on_save(GtkWidget *widget)
	{
		stop();
		GtkWidget *dialog;
GtkFileChooser *chooser;
GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
gint res;

dialog = gtk_file_chooser_dialog_new ("Save File",
                                      gtk_widget_get_window(widget),
                                      action,
                                      ("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      ("_Save"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);
chooser = GTK_FILE_CHOOSER (dialog);

gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);


res = gtk_dialog_run (GTK_DIALOG (dialog));
if (res == GTK_RESPONSE_ACCEPT)
  {
    char *filename;

    filename = gtk_file_chooser_get_filename (chooser);
    save_game(filename);
    g_free (filename);
  }
  system("clear");
gtk_widget_destroy (dialog);
}



void
main(int argc,char **argv)
{
    GtkWidget       *window; //l'interface principale du jeu
    GtkWidget		*button; //button de l'interface

    GtkAdjustment speed; //l'acceleration du jeu
	GtkApplication *app; // notre application

	//initialisation des variables
	largeur = 800;	//largeur du fenetre
	langueur = 800;	//longueur du fenetre
	cols = cell_cols; //nombre de column du grille
	rows = cell_rows; //nombre de ligne du grille
	Colors[0]=(GdkRGBA){1,1,1,1}; //premiere couleur est blanche
	
	//initialisation de programme
	gtk_init(&argc, &argv);

	//builder d'association avec le GLADE
	builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "game-of-life-with-colors.glade", NULL);
	
	/* Recuperation du fenetre principale */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    
	/* creer l'application */
	app = gtk_application_new("Menu",G_APPLICATION_FLAGS_NONE);
	
	//recuperation du button jeu de la vie
	button = GTK_WIDGET(gtk_builder_get_object(builder, "button1"));
	g_signal_connect (G_OBJECT (button), "clicked", (GCallback)hide,"window1");
	/* connecter le signal d'activation d'application */
	g_signal_connect(button,"clicked",G_CALLBACK(prog),NULL);

	//recuperation du flowbox ds couleurs
	flow = GTK_WIDGET(gtk_builder_get_object(builder, "flow"));

	//recuperation du button OPEN pour ouvrire un fichier
	button = GTK_WIDGET(gtk_builder_get_object(builder, "open"));
	g_signal_connect(button,"clicked",G_CALLBACK(on_open),NULL);
	g_signal_connect(button,"clicked",G_CALLBACK(prog),NULL);

	//recuperation du button SAVE pour sauvegarder l'etat
	button = GTK_WIDGET(gtk_builder_get_object(builder, "save"));
	g_signal_connect(button,"clicked",G_CALLBACK(on_save),NULL);

	//recuperation du button NEW pour commencer une nouvelle partie
	button = GTK_WIDGET(gtk_builder_get_object(builder, "new"));
	g_signal_connect(button,"clicked",G_CALLBACK(new),NULL);

	//recuperation du button STOP
	button = GTK_WIDGET(gtk_builder_get_object(builder, "stop"));
	g_signal_connect(button,"clicked",G_CALLBACK(stop),NULL);

	//recuperation du button START
	button = GTK_WIDGET(gtk_builder_get_object(builder, "start"));
	g_signal_connect(button,"clicked",G_CALLBACK(start),NULL);

	//recuperation du button RELOAD
	button = GTK_WIDGET(gtk_builder_get_object(builder, "reload"));
	g_signal_connect(button,"clicked",G_CALLBACK(reload),NULL);

	//recuperation du button MENU pour aller au menu principale
	button = GTK_WIDGET(gtk_builder_get_object(builder, "menu"));
	g_signal_connect (G_OBJECT (button), "clicked", (GCallback)hide,"window2");
	g_signal_connect (G_OBJECT (button), "clicked", (GCallback)show,"window1");

	//recuperation du button QUITTER pour quitter le jeu
	button = GTK_WIDGET(gtk_builder_get_object(builder, "quitter_menu"));
	g_signal_connect (G_OBJECT (button), "clicked", (GCallback)gtk_main_quit,"window1");

	//recuperation du button QUITTER pour quitter le jeu
	button = GTK_WIDGET(gtk_builder_get_object(builder, "quitter_part1"));
	g_signal_connect (G_OBJECT (button), "clicked", (GCallback)gtk_main_quit,"window2");

	//recuperation du SCALL pour ajuster la vitesse de progression 
	GtkAdjustment *button1 = (gtk_builder_get_object(builder, "speed"));
	g_signal_connect(button1,"value-changed",(GCallback)speedfct,NULL);
	
	//recuperation du button COULEUR pour saisir les couleus du jeu
	button = GTK_WIDGET(gtk_builder_get_object(builder, "couleur"));
	g_signal_connect (G_OBJECT (button), "clicked", (GCallback)hide,"window2");
	g_signal_connect (G_OBJECT (button), "clicked", (GCallback)show,"window3");
	
	//recuperation du button VALIDER pour ajouter la couleur selectioné
	button = GTK_WIDGET(gtk_builder_get_object(builder, "valider"));
	g_signal_connect (G_OBJECT (button), "color-set", (GCallback)adcolor,NULL);
	
	//recuperation du button SUPPRIMER pour supprimer la couleur selectioné
	button = GTK_WIDGET(gtk_builder_get_object(builder, "supprimer"));
	g_signal_connect (G_OBJECT (button), "clicked", (GCallback)delcolor,NULL);
	
	//recuperation du button REGLAGE pour metre le reglage du jeu
	button = GTK_WIDGET(gtk_builder_get_object(builder, "reglage"));
	g_signal_connect (G_OBJECT (button), "clicked", (GCallback)show,"window4");

	//recuperation du button VALIDER pour valider les regles
	button = GTK_WIDGET(gtk_builder_get_object(builder, "valide"));
	g_signal_connect (G_OBJECT (button), "clicked", (GCallback)combinaison,"window3");

	gtk_builder_connect_signals(builder,NULL);
	
	//recuperation du popover des couleurs
	popover = GTK_WIDGET(gtk_builder_get_object(builder, "popover"));

	//recuperation du flowbox de reglage
	flowR = GTK_WIDGET(gtk_builder_get_object(builder, "flowR"));
	g_signal_connect (G_OBJECT (flowR), "child-activated", (GCallback)drawmb,NULL);

	combinaison();
	system("clear");
    
    gtk_widget_show(window);                
    
    gtk_main();
			
}
