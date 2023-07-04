// gcc `pkg-config gtk+-3.0 --cflags` t.c -o t `pkg-config gtk+-3.0 --libs`

// ToDo:
// implement relativity in graph ... 117 against 17 should look much smaller
// delete function ... one card left in deck ... add a blank slide maybe
// create an executable desktop application

#include<gtk/gtk.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include "fileIO.h"
#include "wiki.h"
#include "ctype.h"
#define MAX_QUESTIONS 100

typedef struct{
	GtkWidget *window; // actual flashcards
	GtkWidget *window2; // input window
	GtkWidget *window3; // menu window
	GtkWidget *window4; // submit form window
	GtkWidget *window5; // more information window
	GtkWidget *fixed;
	GtkWidget *fixed2;
	GtkWidget *fixed3;
	GtkWidget *fixed4;
	GtkWidget *fixed5;
	GtkWidget *button[18];
	GtkWidget *entry[3];
	GtkWidget *label[11];
	GtkWidget *labelforbutton0;
	GtkWidget *labeldisplaymoreinfo;
}flashcardType;

typedef struct {
	GtkWidget *window;
	GtkWidget *fixed;
	GtkWidget *drawing_area;
	GtkWidget *label[10]; //Labels to display 5 names, 5 scores
} fs;

GtkApplication *app;

const int MAX_SCORE_SIZE = 5;
const char* scoreFileName = "leaderboard.txt";
const char* questionFileName = "questions.txt";
int currentNumberCards;
bool qanda;
int pointer;
int score;
flashcardType fwidget;
nameScoreLists *scoreLists;
int flashCardCorrect[MAX_QUESTIONS];

questionAnswerLists *questionLists;
char** nameArray;
int* scoreArray;
fs fwidget2;
int widthAxis = 420;
int heightAxis = 700;

void draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data) {
	guint width;
	guint height;
	GdkRGBA color;
	GtkStyleContext *context;

	context = gtk_widget_get_style_context (widget);

	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);

	gtk_render_background (context, cr, 0, 0, width, height);
	gtk_style_context_get_color (context, gtk_style_context_get_state (context), &color);
	gdk_cairo_set_source_rgba (cr, &color);

	for (int i = 0; i < NUM_SCORES; i++) {
		cairo_set_source_rgb(cr, 0.2 + (i/10.0), 0.3 + (i/10.0), 0.4 + (i/10.0));
		cairo_rectangle(cr, (i*150) + 103, widthAxis, -100, -(scoreArray[i] * 25));
		cairo_fill(cr);
	}
}

void closeAllWindowsAndQuit() {
        gtk_widget_destroy(fwidget.window);
        gtk_widget_destroy(fwidget.window2);
        gtk_widget_destroy(fwidget.window3);
        gtk_widget_destroy(fwidget.window4);
        gtk_widget_destroy(fwidget.window5);
        g_application_quit(G_APPLICATION(app));
}



void setButtonSize(GtkWidget *label, char *inp) {
	GtkCssProvider *provider = gtk_css_provider_new();
	const gchar* size = strdup(inp);

	gchar* css_string = g_strdup_printf("button { font-size: %spx; }", size);
	gtk_css_provider_load_from_data(provider, css_string, -1, NULL);

	g_free(css_string);

	GtkStyleContext *context = gtk_widget_get_style_context(label);
	gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void setLabelSize(GtkWidget *label, char *inp) {
	GtkCssProvider *provider = gtk_css_provider_new();
	const gchar* size = strdup(inp);

	gchar* css_string = g_strdup_printf("label { font-size: %spx; }", size);
	gtk_css_provider_load_from_data(provider, css_string, -1, NULL);

	g_free(css_string);

	GtkStyleContext *context = gtk_widget_get_style_context(label);
	gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

 void setColor(GtkWidget *widget, char *inp) {
	// Set the CSS style for the button[0]
	GtkCssProvider *provider = gtk_css_provider_new();
	const gchar* color = strdup(inp);

	gchar* css_string = g_strdup_printf(".colored-button { background: %s; color: #ffffff; }", color);
	gtk_css_provider_load_from_data(provider, css_string, -1, NULL);

	g_free(css_string);

	// Apply the CSS provider to the button's style context
	GtkStyleContext *context = gtk_widget_get_style_context(widget);
	gtk_style_context_add_provider(context,
		GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	// Change the background color of the button
	gtk_style_context_add_class(context, "colored-button");
}

 void setForeColor(GtkWidget *widget, char *inp, char *inpBack) {
	// Set the CSS style for the button[0]
	GtkCssProvider *provider = gtk_css_provider_new();
	const gchar* color = strdup(inp);
	const gchar* colorBack = strdup(inpBack);

	gchar* css_string = g_strdup_printf(".colored-button { background: %s; color: %s; }", colorBack, color);
	gtk_css_provider_load_from_data(provider, css_string, -1, NULL);

	g_free(css_string);

	// Apply the CSS provider to the button's style context
	GtkStyleContext *context = gtk_widget_get_style_context(widget);
	gtk_style_context_add_provider(context,
		GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	// Change the background color of the button
	gtk_style_context_add_class(context, "colored-button");
}

 void callback(GtkWidget *widget, gpointer data) {
  if (qanda) {
	//this means its a question
	qanda = false;
	questionAnswerLists* qaList = questionRead(questionFileName);
	gtk_label_set_text(GTK_LABEL(fwidget.labelforbutton0), qaList->questions[pointer]);

	setColor(widget, "#77867f");

	char* str2 = g_strdup_printf("Question: %d / %d", pointer + 1, currentNumberCards);
	gtk_label_set_markup((gpointer)(fwidget.label[1]), str2);

  } else {
	//this means its an answer
	qanda = true;
	questionAnswerLists* qaList = questionRead(questionFileName);
	gtk_label_set_text(GTK_LABEL(fwidget.labelforbutton0), qaList->answers[pointer]);
	setColor(widget, "#9fd1d0"); // torquise
	char* str2 = g_strdup_printf("Answer: %d / %d", pointer + 1, currentNumberCards);
	gtk_label_set_markup((gpointer)(fwidget.label[1]), str2);
}
}

void leftCallback(GtkWidget *widget, gpointer data) {
  if (pointer > 0) {
	pointer--;
  } else {
	pointer = 0;
  }
  questionAnswerLists* qaList = questionRead(questionFileName);
	gtk_label_set_text(GTK_LABEL(fwidget.labelforbutton0), qaList->questions[pointer]);
  char* str2 = g_strdup_printf("Question: %d / %d", pointer + 1, currentNumberCards);
  gtk_label_set_markup((gpointer)(fwidget.label[1]), str2);

  setColor(data, "#77867f");
  qanda = false;
}

 void rightCallback(GtkWidget *widget, gpointer data) {
  if(pointer == currentNumberCards - 1) {
		g_print("MAX");
  } else {
	pointer++;
  }
  questionAnswerLists* qaList = questionRead(questionFileName);
	gtk_label_set_text(GTK_LABEL(fwidget.labelforbutton0), qaList->questions[pointer]);
  char* str2 = g_strdup_printf("Question: %d / %d", pointer + 1, currentNumberCards);
  gtk_label_set_markup((gpointer)(fwidget.label[1]), str2);

  setColor(data, "#77867f");
  qanda = false;
}

 void submit(GtkWidget *widget, gpointer data) {
  const char *inputQ = gtk_entry_get_text((gpointer) fwidget.entry[0]);
  const char *inputA = gtk_entry_get_text((gpointer) fwidget.entry[1]);
  if(strcmp(inputQ, "") != 0 && strcmp(inputA, "") != 0) {
	//do

		if (currentNumberCards < MAX_QUESTIONS) {
			fileWrite(questionFileName, inputQ, inputA);
			currentNumberCards++;
		}

  } else { // don't accept value
		g_print("Null\n");
  }

	// if worked
	gtk_widget_hide(fwidget.window2);
	gtk_widget_show_all(fwidget.window3);
}

 void correctAns(GtkWidget *widget, gpointer data) {

	if (pointer == currentNumberCards - 1) {
		if(flashCardCorrect[pointer] == 0) {
			score++;
			flashCardCorrect[pointer] = 1;
		}
	} else {
		if(flashCardCorrect[pointer] == 0) {
			flashCardCorrect[pointer] = 1;
			pointer++;
			score++;
		}
	}

	questionAnswerLists* qaList = questionRead(questionFileName);
	gtk_label_set_text(GTK_LABEL(fwidget.labelforbutton0), qaList->questions[pointer]);
	char* str2 = g_strdup_printf("Question: %d / %d", pointer + 1, currentNumberCards);
	gtk_label_set_markup((gpointer)(fwidget.label[1]), str2);

	char* str = g_strdup_printf("Score: %d", score);
	gtk_label_set_markup((gpointer)(fwidget.label[0]), str);
	setColor(data, "#77867f"); // light red
	qanda = false;
}

 void openFlashcards(GtkWidget *widget, gpointer data) {
	gtk_widget_hide(data);
	char* str2 = g_strdup_printf("Question: %d / %d", pointer + 1, currentNumberCards);
	gtk_label_set_markup((gpointer)(fwidget.label[1]), str2);
	gtk_widget_show_all(fwidget.window);
}

 void openInput(GtkWidget *widget, gpointer data) {
	gtk_widget_hide(data);
	gtk_widget_show_all(fwidget.window2);
}

 void writeToFile(GtkApplication *app, gpointer data) {
	char scoreString[MAX_SCORE_SIZE];
	sprintf(scoreString, "%d", score);

	const char *entry2text = gtk_entry_get_text(fwidget.entry[2]);

	char *str = entry2text;
	bool invalid_characters = false;
	while (*str) {
    if (!isalpha(*str)) {
      invalid_characters = true;
    }
    str++;
	}

	if(!invalid_characters) {
		fileWrite(scoreFileName, entry2text, scoreString);
	} else {
		// display invalid entry
		g_print("Invalid entry\n");
	}

	if(!invalid_characters) {

	  gtk_widget_hide(data);
	  gtk_widget_show_all(fwidget.window3);
	} else {
	  g_print("Error writing to file\n");
	}
}

 void openWindow3(GtkWidget *widget, gpointer data) {
	gtk_widget_hide(data);
	gtk_widget_show_all(fwidget.window3);
}

 void openWindow4(GtkWidget *widget, gpointer data) {
	char* str = g_strdup_printf("Current score: %d", score);
	gtk_label_set_markup((gpointer)(fwidget.label[10]), str);
	gtk_widget_hide(data);
	gtk_widget_show_all(fwidget.window4);
}

// web scraping function
 void moreInformation(GtkWidget *widget, gpointer data) {
	gtk_widget_hide(data);
	gtk_widget_show_all(fwidget.window5);

	questionAnswerLists* qaList = questionRead(questionFileName);
	char *currQuest = qaList->answers[pointer];
	char *title = getTarget(add20(currQuest));
	if(title == NULL) {
	  g_print("Does not work F\n");
		gtk_label_set_text(GTK_LABEL(fwidget.labeldisplaymoreinfo), "No Information found for the requested flashcard.");
	} else {
		char *text = getBody(add20(title));
		if(text == NULL) {
			gtk_label_set_text(GTK_LABEL(fwidget.labeldisplaymoreinfo), "No Information found for the requested flashcard.");
		} else {
			int MAX_CHARS = 1000;
			if (g_utf8_strlen(text, -1) > MAX_CHARS)
			{
				gchar *truncated_text = g_utf8_substring(text, 0, MAX_CHARS);
				gchar *final_text = g_strconcat(truncated_text, "...", NULL);
				gtk_label_set_text(GTK_LABEL(fwidget.labeldisplaymoreinfo), final_text);
				g_free(truncated_text);
				g_free(final_text);
			} else {
				gtk_label_set_text(GTK_LABEL(fwidget.labeldisplaymoreinfo), text);
			}
		}
	}
}

 void openWindowFromMoreInfo(GtkWidget *widget, gpointer data) {
	gtk_widget_hide(data);
	gtk_widget_show_all(fwidget.window);
}

void openGraph(GtkWidget *widget, gpointer data) {
	gtk_widget_hide(data);
	g_signal_connect(G_OBJECT(fwidget2.drawing_area), "draw", G_CALLBACK(draw_callback), NULL);
	g_signal_connect(fwidget2.drawing_area, "realize", G_CALLBACK(gtk_widget_queue_draw), NULL);
	gtk_widget_show_all(fwidget2.window);
}

void deleteFlashcard(GtkWidget *widget, gpointer data) {
	questionAnswerLists* qaList = questionRead(questionFileName);
	const char *questionToDelete = qaList->questions[pointer];
	char *fileName = "questions.txt";
	if(pointer > 0) {
		pointer--;
		currentNumberCards--;
		qanda = true;
		deleteQuestionAnswerPair(fileName, questionToDelete);
		callback(fwidget.button[0], NULL);
	} else if(currentNumberCards > 1) { // atleast 2 cards in the deck
		currentNumberCards--;
		qanda = true;
		deleteQuestionAnswerPair(fileName, questionToDelete);
		callback(fwidget.button[0], NULL);
	} else { // one card in the deck

	}
}

 void activate(GtkApplication *app, gpointer user_data) {
	// initializing variables
	questionLists = questionRead(questionFileName);

	scoreLists = getTopScores(scoreFileName);

	// initializing scoreAtSlide
	for(int i = 0; i < MAX_QUESTIONS; i++) {
		flashCardCorrect[i] = 0;
	}

	nameArray = scoreLists->names;
	scoreArray = scoreLists->scores;
	currentNumberCards = questionLists->size;

	pointer = 0;
	qanda = false;
	score = 0;

	fwidget2.window = gtk_application_window_new(app);
  gtk_window_set_position(GTK_WINDOW(fwidget2.window),GTK_WIN_POS_CENTER);
  gtk_window_set_title(GTK_WINDOW(fwidget2.window),"Leaderboard");
  gtk_window_set_default_size(GTK_WINDOW(fwidget2.window), 800, 750);
  gtk_container_set_border_width(GTK_CONTAINER(fwidget2.window),0);

	fwidget2.fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(fwidget2.window), fwidget2.fixed);

	fwidget2.drawing_area = gtk_drawing_area_new();
	gtk_widget_set_size_request(fwidget2.drawing_area, 780, 440);

	gtk_fixed_put(GTK_FIXED(fwidget2.fixed), fwidget2.drawing_area, 10, 130);

	g_signal_connect(G_OBJECT(fwidget2.drawing_area), "draw", G_CALLBACK(draw_callback), NULL);
	g_signal_connect(fwidget2.drawing_area, "realize", G_CALLBACK(gtk_widget_queue_draw), NULL);

	for (int i = 0; i < NUM_SCORES; i++) {
		fwidget2.label[i] = gtk_label_new(NULL);
		gtk_label_set_markup((gpointer)(fwidget2.label[i]), g_strdup_printf("%s", nameArray[i]));
		gtk_widget_set_size_request(fwidget2.label[i], 200, 200);
		gtk_fixed_put(GTK_FIXED(fwidget2.fixed),fwidget2.label[i], (150 * i) - 30 ,490);
	}

	for (int i = 0; i < NUM_SCORES; i++) {
		fwidget2.label[i + 5] = gtk_label_new(NULL);
		gtk_label_set_markup((gpointer)(fwidget2.label[i+5]), g_strdup_printf("%d", scoreArray[i]));
		gtk_widget_set_size_request(fwidget2.label[i+5], 200, 200);
		gtk_fixed_put(GTK_FIXED(fwidget2.fixed),fwidget2.label[i+5], (150 * i) - 30 ,420);
	}



	fwidget.window = gtk_application_window_new(app);
	gtk_window_set_position(GTK_WINDOW(fwidget.window),GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(fwidget.window),"flashcard app");
	gtk_window_set_default_size(GTK_WINDOW(fwidget.window), 800, 750);
	gtk_container_set_border_width(GTK_CONTAINER(fwidget.window),0);

	fwidget.window2 = gtk_application_window_new(app);
	gtk_window_set_position(GTK_WINDOW(fwidget.window2),GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(fwidget.window2),"Input Flashcard app");
	gtk_window_set_default_size(GTK_WINDOW(fwidget.window2), 800, 750);
	gtk_container_set_border_width(GTK_CONTAINER(fwidget.window2),0);

	fwidget.window3 = gtk_application_window_new(app);
	gtk_window_set_position(GTK_WINDOW(fwidget.window3),GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(fwidget.window3),"Main Menu");
	gtk_window_set_default_size(GTK_WINDOW(fwidget.window3), 700, 735);
	gtk_container_set_border_width(GTK_CONTAINER(fwidget.window3),0);

	fwidget.window4 = gtk_application_window_new(app);
	gtk_window_set_position(GTK_WINDOW(fwidget.window4),GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(fwidget.window4),"Submit Form");
	gtk_window_set_default_size(GTK_WINDOW(fwidget.window4), 800, 650);
	gtk_container_set_border_width(GTK_CONTAINER(fwidget.window4),0);

	fwidget.window5 = gtk_application_window_new(app);
	gtk_window_set_position(GTK_WINDOW(fwidget.window5),GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(fwidget.window5),"More Information");
	gtk_window_set_default_size(GTK_WINDOW(fwidget.window5), 800, 750);
	gtk_container_set_border_width(GTK_CONTAINER(fwidget.window5),0);


	// initialized fixed widgets
	fwidget.fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(fwidget.window), fwidget.fixed);

	fwidget.fixed2 = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(fwidget.window2), fwidget.fixed2);

	fwidget.fixed3 = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(fwidget.window3), fwidget.fixed3);

	fwidget.fixed4 = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(fwidget.window4), fwidget.fixed4);

	fwidget.fixed5 = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(fwidget.window5), fwidget.fixed5);


	// stops program when closed 
	g_signal_connect(fwidget.window, "delete-event", G_CALLBACK(closeAllWindowsAndQuit), NULL);
	g_signal_connect(fwidget.window2, "delete-event", G_CALLBACK(closeAllWindowsAndQuit), NULL);
	g_signal_connect(fwidget.window3, "delete-event", G_CALLBACK(closeAllWindowsAndQuit), NULL);
	g_signal_connect(fwidget.window4, "delete-event", G_CALLBACK(closeAllWindowsAndQuit), NULL);
	g_signal_connect(fwidget.window5, "delete-event", G_CALLBACK(closeAllWindowsAndQuit), NULL);





	questionAnswerLists* qaList = questionRead(questionFileName);
	fwidget.button[0] = gtk_button_new_with_label(qaList->questions[pointer]);
	fwidget.button[1] = gtk_button_new_with_label("<---");
	fwidget.button[2] = gtk_button_new_with_label("--->");

	// setting text for buttons
	fwidget.button[0] = gtk_button_new();
	fwidget.button[1] = gtk_button_new_with_label("<---");
	fwidget.button[2] = gtk_button_new_with_label("--->");
	fwidget.button[3] = gtk_button_new_with_label("Submit"); // window2
	fwidget.button[4] = gtk_button_new_with_label("Correct");
	fwidget.button[5] = gtk_button_new_with_label("Incorrect");
	fwidget.button[6] = gtk_button_new_with_label("Open Flashcards");
	fwidget.button[7] = gtk_button_new_with_label("Input Flashcards");
	fwidget.button[8] = gtk_button_new_with_label("Submit"); // window4
	fwidget.button[9] = gtk_button_new_with_label("Open File Writer Window");
	fwidget.button[10] = gtk_button_new_with_label("Back to home page");
	fwidget.button[11] = gtk_button_new_with_label("Back to home page");
	fwidget.button[12] = gtk_button_new_with_label("Back to home page");
	fwidget.button[13] = gtk_button_new_with_label("More Information");
	fwidget.button[14] = gtk_button_new_with_label("Back to home page");
	fwidget.button[15] = gtk_button_new_with_label("Open Leaderboard");
	fwidget.button[16] = gtk_button_new_with_label("Back to home page");
	fwidget.button[17] = gtk_button_new_with_label("X"); // delete flashcard button

	gtk_widget_set_size_request(fwidget.button[0], 780, 410);
	gtk_widget_set_size_request(fwidget.button[1], 150, 80);
	gtk_widget_set_size_request(fwidget.button[2], 150, 80);
	gtk_widget_set_size_request(fwidget.button[3], 300, 100);
	gtk_widget_set_size_request(fwidget.button[4], 327, 80);
	gtk_widget_set_size_request(fwidget.button[5], 150, 80);
	gtk_widget_set_size_request(fwidget.button[6], 300, 120);
	gtk_widget_set_size_request(fwidget.button[7], 300, 120);
	gtk_widget_set_size_request(fwidget.button[8], 300, 100);
	gtk_widget_set_size_request(fwidget.button[9], 300, 120);
	gtk_widget_set_size_request(fwidget.button[10], 780, 100);
	gtk_widget_set_size_request(fwidget.button[11], 780, 100);
	gtk_widget_set_size_request(fwidget.button[12], 780, 100);
	gtk_widget_set_size_request(fwidget.button[13], 675, 80);
	gtk_widget_set_size_request(fwidget.button[14], 780, 100);
	gtk_widget_set_size_request(fwidget.button[15], 300, 120);
	gtk_widget_set_size_request(fwidget.button[16], 780, 100);
	gtk_widget_set_size_request(fwidget.button[17], 50, 50);


	// initialized entry widgets
	fwidget.entry[0] = gtk_entry_new(); // question entry ... window2
	gtk_editable_set_editable(GTK_EDITABLE(fwidget.entry[0]), TRUE);
	fwidget.entry[1] = gtk_entry_new(); // answer entry ... window2
	gtk_editable_set_editable(GTK_EDITABLE(fwidget.entry[1]), TRUE);
	fwidget.entry[2] = gtk_entry_new(); // name entry to submit progress... window4
	gtk_editable_set_editable(GTK_EDITABLE(fwidget.entry[2]), TRUE);

	gtk_widget_set_size_request(fwidget.entry[0], 450, 50);
	gtk_widget_set_size_request(fwidget.entry[1], 450, 50);
	gtk_widget_set_size_request(fwidget.entry[2], 450, 50);


	// initialized label widgets
	fwidget.label[0] = gtk_label_new(NULL);
	char* str = g_strdup_printf("Score: %d", score);
	gtk_label_set_markup((gpointer)(fwidget.label[0]), str); // score ... window
	fwidget.label[1] = gtk_label_new(NULL);
	char* str2 = g_strdup_printf("Question: %d / %d", pointer + 1, currentNumberCards);
	gtk_label_set_markup((gpointer)(fwidget.label[1]), str2); // Progress ... window
	fwidget.label[2] = gtk_label_new(NULL);
	char* str3 = g_strdup_printf("FlashCard App");
	gtk_label_set_markup((gpointer)(fwidget.label[2]), str3); // App name ... window3
	fwidget.label[3] = gtk_label_new(NULL);
	char* str4 = g_strdup_printf("Input A FlashCard");
	gtk_label_set_markup((gpointer)(fwidget.label[3]), str4); // Input a flashcard ... window2
	fwidget.label[4] = gtk_label_new(NULL);
	char* str5 = g_strdup_printf("Update Your Progress");
	gtk_label_set_markup((gpointer)(fwidget.label[4]), str5); // submit label ... window4
	fwidget.label[5] = gtk_label_new(NULL);
	char* str6 = g_strdup_printf("Question");
	gtk_label_set_markup((gpointer)(fwidget.label[5]), str6); // question label ... window2
	fwidget.label[6] = gtk_label_new(NULL);
	char* str7 = g_strdup_printf("Answer");
	gtk_label_set_markup((gpointer)(fwidget.label[6]), str7); // answer label ... window2
	fwidget.label[7] = gtk_label_new(NULL);
	char* str8 = g_strdup_printf("User name");
	gtk_label_set_markup((gpointer)(fwidget.label[7]), str8); // user name ... window4
	fwidget.label[8] = gtk_label_new(NULL);
	char* str9 = g_strdup_printf("More Information");
	gtk_label_set_markup((gpointer)(fwidget.label[8]), str9); // more information ... window5
	fwidget.label[9] = gtk_label_new(NULL);
	char* str10 = g_strdup_printf("Leaderboard");
	gtk_label_set_markup((gpointer)(fwidget.label[9]), str10); // leaderboard ... fwidget2.window
	fwidget.label[10] = gtk_label_new(NULL);
	char* str11 = g_strdup_printf("Current Score: 0");
	gtk_label_set_markup((gpointer)(fwidget.label[10]), str11); // score ... window4

	gtk_widget_set_size_request(fwidget.label[0], 100, 50);
	gtk_widget_set_size_request(fwidget.label[1], 175, 25);
	gtk_widget_set_size_request(fwidget.label[2], 250, 50);
	gtk_widget_set_size_request(fwidget.label[3], 780, 100);
	gtk_widget_set_size_request(fwidget.label[4], 780, 100);
	gtk_widget_set_size_request(fwidget.label[5], 250, 50);
	gtk_widget_set_size_request(fwidget.label[6], 250, 50);
	gtk_widget_set_size_request(fwidget.label[7], 250, 50);
	gtk_widget_set_size_request(fwidget.label[8], 780, 100);
	gtk_widget_set_size_request(fwidget.label[9], 780, 100);
	gtk_widget_set_size_request(fwidget.label[10], 250, 50);


	// adding to window
	gtk_fixed_put(GTK_FIXED(fwidget.fixed), fwidget.button[0], 10, 10); // Question or Answer
	gtk_fixed_put(GTK_FIXED(fwidget.fixed), fwidget.button[1], 62, 537); // Left
	gtk_fixed_put(GTK_FIXED(fwidget.fixed), fwidget.button[2], 587,537); // Right
	gtk_fixed_put(GTK_FIXED(fwidget.fixed), fwidget.button[4],237,537); // correct
	gtk_fixed_put(GTK_FIXED(fwidget.fixed), fwidget.button[11],10,642); // Goes back to home page
	gtk_fixed_put(GTK_FIXED(fwidget.fixed), fwidget.button[13], 62, 445); // More Information
	gtk_fixed_put(GTK_FIXED(fwidget.fixed), fwidget.button[17], 735, 10); // Delete flashcard
	gtk_fixed_put(GTK_FIXED(fwidget.fixed), fwidget.label[0],686.5,370); // Score
	gtk_fixed_put(GTK_FIXED(fwidget.fixed), fwidget.label[1],5,20); // Question Number

	// adding to window2
	gtk_fixed_put(GTK_FIXED(fwidget.fixed2), fwidget.button[3], 252, 440); // Submit question answer
	gtk_fixed_put(GTK_FIXED(fwidget.fixed2), fwidget.button[10], 10, 642); // Goes back to home page
	gtk_fixed_put(GTK_FIXED(fwidget.fixed2), fwidget.label[3],10,10); // Input a flashcard display
	gtk_fixed_put(GTK_FIXED(fwidget.fixed2), fwidget.label[5],264,170); // Question Input
	gtk_fixed_put(GTK_FIXED(fwidget.fixed2), fwidget.label[6],265,290); // Answer Input
	gtk_fixed_put(GTK_FIXED(fwidget.fixed2), fwidget.entry[0],176,220); // question entry
	gtk_fixed_put(GTK_FIXED(fwidget.fixed2), fwidget.entry[1],176,340); // answer entry

	// adding to window3
	gtk_fixed_put(GTK_FIXED(fwidget.fixed3),fwidget.label[2],160,25); // Display the app name
	gtk_fixed_put(GTK_FIXED(fwidget.fixed3), fwidget.button[6], 200, 125); // Open flashcard
	gtk_fixed_put(GTK_FIXED(fwidget.fixed3), fwidget.button[7], 200, 280); // Input Flashcard
	gtk_fixed_put(GTK_FIXED(fwidget.fixed3), fwidget.button[9], 200, 435); // Open Window4
	gtk_fixed_put(GTK_FIXED(fwidget.fixed3), fwidget.button[15], 200, 590); // Open Leaderboard

	// adding to window4
	gtk_fixed_put(GTK_FIXED(fwidget.fixed4),fwidget.label[7],264,180); // user name ...
	gtk_fixed_put(GTK_FIXED(fwidget.fixed4),fwidget.label[10],265,320); // current score ...
	gtk_fixed_put(GTK_FIXED(fwidget.fixed4),fwidget.label[4],10,10); // Submit your progress label ...
	gtk_fixed_put(GTK_FIXED(fwidget.fixed4), fwidget.entry[2], 176, 240); // name ...
	gtk_fixed_put(GTK_FIXED(fwidget.fixed4), fwidget.button[8], 252, 390); // Write to file ...
	gtk_fixed_put(GTK_FIXED(fwidget.fixed4), fwidget.button[12], 10, 542); // Goes back to home page ...

	// adding to window5
	gtk_fixed_put(GTK_FIXED(fwidget.fixed5), fwidget.button[14], 10, 642); // back to home page
	gtk_fixed_put(GTK_FIXED(fwidget.fixed5), fwidget.label[8], 10, 10); // More information label at top

	// adding to fwidget2.window
	gtk_fixed_put(GTK_FIXED(fwidget2.fixed), fwidget.button[16], 10, 642); // back to home page
	gtk_fixed_put(GTK_FIXED(fwidget2.fixed), fwidget.label[9], 10, 10); // More information label at top


	// setting color for windows
	setColor(fwidget.window, "#e0ddd5");
	setColor(fwidget.window2, "#e0ddd5");
	setColor(fwidget.window3, "#c8e9a0");
	setColor(fwidget.window4, "#e0ddd5");
	setColor(fwidget.window5, "#e0ddd5");
	setColor(fwidget2.window, "#e0ddd5");

	// setting color for buttons
	setColor(fwidget.button[0], "#77867f");
	setColor(fwidget.button[3], "#ADD8E6");
	setColor(fwidget.button[6], "#77867f");
	setColor(fwidget.button[7], "#77867f");
	setColor(fwidget.button[8], "#ADD8E6");
	setColor(fwidget.button[9], "#77867f");
	setColor(fwidget.button[10], "#8ea3ad");
	setColor(fwidget.button[11], "#8ea3ad");
	setColor(fwidget.button[12], "#8ea3ad");
	setColor(fwidget.button[14], "#8ea3ad");
	setColor(fwidget.button[15], "#77867f");
	setColor(fwidget.button[16], "#8ea3ad");
	setColor(fwidget.button[17], "#77867f");

	// setting color for labels
	setForeColor(fwidget.label[2], "#472c4c", "#c8e9a0");
	setForeColor(fwidget.label[3], "#f8f8ff", "#e2725b");
	setForeColor(fwidget.label[4], "#f8f8ff", "#e2725b");
	setForeColor(fwidget.label[5], "#333333", "#e0ddd5");
	setForeColor(fwidget.label[6], "#333333", "#e0ddd5");
	setForeColor(fwidget.label[7], "#333333", "#e0ddd5");
	setForeColor(fwidget.label[8], "#f8f8ff", "#e2725b");
	setForeColor(fwidget.label[9], "#f8f8ff", "#e2725b");
	setForeColor(fwidget.label[10], "#333333", "#e0ddd5");


	// setting label size
	setLabelSize(fwidget.label[10], "20");
	setLabelSize(fwidget.label[9], "60");
	setLabelSize(fwidget.label[8], "60");
	setLabelSize(fwidget.label[7], "40");
	setLabelSize(fwidget.label[6], "40");
	setLabelSize(fwidget.label[5], "40");
	setLabelSize(fwidget.label[4], "60");
	setLabelSize(fwidget.label[3], "60");
	setLabelSize(fwidget.label[2], "60");
	setLabelSize(fwidget.label[1], "20");
	setLabelSize(fwidget.label[0], "20");

	// setting button size
	setButtonSize(fwidget.button[0], "50");
	setButtonSize(fwidget.button[3], "30");
	setButtonSize(fwidget.button[6], "20");
	setButtonSize(fwidget.button[7], "20");
	setButtonSize(fwidget.button[8], "30");
	setButtonSize(fwidget.button[9], "20");
	setButtonSize(fwidget.button[10], "20");
	setButtonSize(fwidget.button[11], "20");
	setButtonSize(fwidget.button[12], "20");
	setButtonSize(fwidget.button[13], "20");
	setButtonSize(fwidget.button[14], "20");
	setButtonSize(fwidget.button[15], "20");
	setButtonSize(fwidget.button[16], "20");
	setButtonSize(fwidget.button[17], "30");


	// Creating label to display button[0] ... wrapping of text
	fwidget.labelforbutton0 = gtk_label_new(NULL);
	gtk_label_set_line_wrap(GTK_LABEL(fwidget.labelforbutton0), TRUE); // Enable line wrapping
	gtk_label_set_max_width_chars(GTK_LABEL(fwidget.labelforbutton0), 10); // Set character limit
	gtk_label_set_ellipsize(GTK_LABEL(fwidget.labelforbutton0), PANGO_ELLIPSIZE_NONE); // Disable ellipsizing
	gtk_label_set_text(GTK_LABEL(fwidget.labelforbutton0), qaList->questions[pointer]);
	gtk_container_add(GTK_CONTAINER(fwidget.button[0]), fwidget.labelforbutton0);


	// More info display label ... wrapping of text
	fwidget.labeldisplaymoreinfo = gtk_label_new(NULL);
	gtk_label_set_line_wrap(GTK_LABEL(fwidget.labeldisplaymoreinfo), TRUE); // Enable line wrapping
	gtk_label_set_max_width_chars(GTK_LABEL(fwidget.labeldisplaymoreinfo), 10); // Set character limit
	gtk_label_set_ellipsize(GTK_LABEL(fwidget.labeldisplaymoreinfo), PANGO_ELLIPSIZE_NONE); // Disable ellipsizing

	gtk_label_set_width_chars(GTK_LABEL(fwidget.labeldisplaymoreinfo), 60);
	gtk_label_set_max_width_chars(GTK_LABEL(fwidget.labeldisplaymoreinfo), 60);

	setLabelSize(fwidget.labeldisplaymoreinfo, "22");
	gtk_widget_set_size_request(fwidget.labeldisplaymoreinfo, 300, 450); // display the information
	setForeColor(fwidget.labeldisplaymoreinfo, "#333333", "#e0ddd5");

	gtk_fixed_put(GTK_FIXED(fwidget.fixed5), fwidget.labeldisplaymoreinfo, 50, 130); // More information label at top


	// adding functionality to buttons
	g_signal_connect(fwidget.button[0], "clicked", G_CALLBACK(callback), NULL);
	g_signal_connect(fwidget.button[1], "clicked", G_CALLBACK(leftCallback), fwidget.button[0]);
	g_signal_connect(fwidget.button[2], "clicked", G_CALLBACK(rightCallback), fwidget.button[0]);
	g_signal_connect(fwidget.button[3], "clicked", G_CALLBACK(submit), NULL);
	g_signal_connect(fwidget.button[4], "clicked", G_CALLBACK(correctAns), fwidget.button[0]);
	g_signal_connect(fwidget.button[6], "clicked", G_CALLBACK(openFlashcards), fwidget.window3);
	g_signal_connect(fwidget.button[7], "clicked", G_CALLBACK(openInput), fwidget.window3);
	g_signal_connect(fwidget.button[8], "clicked", G_CALLBACK(writeToFile), fwidget.window4);
	g_signal_connect(fwidget.button[9], "clicked", G_CALLBACK(openWindow4), fwidget.window3);
	g_signal_connect(fwidget.button[10], "clicked", G_CALLBACK(openWindow3), fwidget.window2);
	g_signal_connect(fwidget.button[11], "clicked", G_CALLBACK(openWindow3), fwidget.window);
	g_signal_connect(fwidget.button[12], "clicked", G_CALLBACK(openWindow3), fwidget.window4);
	g_signal_connect(fwidget.button[13], "clicked", G_CALLBACK(moreInformation), fwidget.window);
	g_signal_connect(fwidget.button[14], "clicked", G_CALLBACK(openWindowFromMoreInfo), fwidget.window5);
	g_signal_connect(fwidget.button[15], "clicked", G_CALLBACK(openGraph), fwidget.window3);
	g_signal_connect(fwidget.button[16], "clicked", G_CALLBACK(openWindow3), fwidget2.window);
	g_signal_connect(fwidget.button[17], "clicked", G_CALLBACK(deleteFlashcard), NULL);


	// displaying the main menu
	gtk_widget_show_all(fwidget.window3);
}


int main(int argc, char **argv){
  gtk_init(&argc, &argv);

  int status;
  app = gtk_application_new("org.gtk.flashCard", G_APPLICATION_FLAGS_NONE);

  //app = object pointing to the object emitting the signal (usually a GTK widget)
  //activate = name of signal to connect to
  //callback function that is invoked when the signal is emitted
  //NULL = activate does not need any user defined data
  g_signal_connect(app,"activate", G_CALLBACK(activate), NULL);

  //run the application event loop
  status = g_application_run(G_APPLICATION(app), argc, argv);

	//performs cleanup and finalises application
  g_object_unref(app);

	return status;
}