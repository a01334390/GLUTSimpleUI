// Please don't change lines 9-31 (It helps me to grade)
#ifdef __APPLE__
// For XCode only: New C++ terminal project. Build phases->Compile with libraries: add OpenGL and GLUT
// Import this whole code into a new C++ file (main.cpp, for example). Then run.
// Reference: https://en.wikibooks.org/wiki/OpenGL_Programming/Installation/Mac
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <GLUT/glut.h>
#endif
#ifdef _WIN32
// For VS on Windows only: Download CG_Demo.zip. UNZIP FIRST. Double click on CG_Demo/CG_Demo.sln
// Run
	#include "freeglut.h"
#endif
#ifdef __unix__
// For Linux users only: g++ CG_Demo.cpp -lglut -lGL -o CG_Demo
// ./CG_Demo
// Reference: https://www.linuxjournal.com/content/introduction-opengl-programming
	#include "GL/freeglut.h"
	#include "GL/gl.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/** Include handmade Classes */
#include "Mouse.hpp"

/** Create a global mouse */
Mouse* MickeyMouse;

/** Window size */
int winw = 640;
int winh = 480;

/** Button Callback */
typedef void (*ButtonCallback)();

/** Button Stuff */
struct Button {
    int id; /** ID of the button */
    int x; /** Top-left x-coord */
    int y; /** Top-right y-coord */
    int w; /** Button Width */
    int h; /** Height of the button */
    int state; /** 0 -> Idle, 1 -> Pressed */
    int highlighted; /** Pointer over the button */
    char *label; /** text label */
    ButtonCallback callbackFunction; /** Function to call */
    struct Button* next; /** Next button */
};

typedef struct Button Button;

/** Linked-List of buttons */
Button* pButtonList = NULL;
int globalref = 0;


/**
    Create a button
    label *char Text to carry
    cb ButtonCallback The function to callback
    x int -> Initial x position
    y int -> Initial y position
    w int -> Width
    h int -> Height
*/

int CreateButton(char *label,ButtonCallback cb,int x,int y,int w,int h){
    Button *p = (Button*)malloc(sizeof(Button));
    assert(p); /** Checks if the structure initialized correctly */
    memset(p,0,sizeof(Button)); /** Fills the block */
    p->x = x;
	p->y = y;
	p->w = w;
	p->h = h;
    p->callbackFunction = cb;
	p->label = (char*)malloc( strlen(label)+1 );
    if(p->label){
        sprintf(p->label,label); //Pass the label to the struct
    }
    
    // Add the button to the top of the list
    p->next = pButtonList;
    pButtonList = p;

    return p->id = ++globalref;
}

/**
    Deletes a button based on its label
    char *label The label to search for
*/

int DeleteButtonByLabel(char *label){
    Button* previous = NULL, *curr=pButtonList;
    while(curr != NULL){
        if (strcmp(label,curr->label)){
            if(previous){
                previous->next = curr->next;
            } else {
                pButtonList = curr->next;
            }
            // If the label is empty, free it
            if(curr->label){
                free(curr->label);
            }
            free(curr);
            return 1;
        }
        previous = curr;
        curr = curr-> next;
    }
    return 0;
}

/**
    Deletes a button based on its ID
    id int The button ID
*/

int DeleteButtonByLabel(int id){
    Button *previous = NULL,*curr = pButtonList;
	while (curr!=NULL) {
		if ( id == curr->id ){
			if(previous){
				previous->next = curr->next;
            }else{
				pButtonList = curr->next;
            }

			if (curr->label) {
				free(curr->label);
			}
			free(curr);
			return 1;
		}
		previous = curr;
		curr = curr->next;
	}
	return 0;
}

static void TheButtonCallback(){
	printf("I have been called\n");
}