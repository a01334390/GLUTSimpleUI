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
    Checks if the button was clicked between bounds
    b Button* The pointer to the button
    x int x-coord to test
    y int y-coord to test
*/

int ButtonClickTest(Button* b,int x,int y){
    if (b) {
        if(x > b->x 
        && x < b->x + b->w 
        && y > b->y
        && y < b->y + b->h){
            return 1;
        }
    }

    return 0;
}

/** 
    Checks for released buttons
    x int x-coord to test
    y int y-coord to test
*/

void ButtonRelease(int x,int y) {
    Button* b = pButtonList;
    while(b){
        // If the button was clicked within the button area and being released
        if( ButtonClickTest(b,MickeyMouse->xpress,MickeyMouse->ypress) 
        && ButtonClickTest(b,x,y)){
			if (b->callbackFunction) {
				b->callbackFunction();
			}
		}
        // Return state
		b->state = 0;
		b=b->next;
    }
}

/** 
    Checks for button presses
    b Button* The pointer to the button
    x int x-coord to test
    y int y-coord to test
*/
void ButtonPress(int x,int y){
    Button* b = pButtonList;
	while(b) {
		if(ButtonClickTest(b,x,y)){
			b->state = 1;
		}
		b=b->next;
	}
}

/** Draws the button */
void ButtonPassive(int x,int y){
    int needRedraw = 0;
	Button* b = pButtonList;
	while(b) {
        // Is the mouse over the button?
		if(ButtonClickTest(b,x,y)){
			if( b->highlighted == 0 ) { //If not highlighted, please do
				b->highlighted = 1;
				needRedraw=1;
			}
		} else if( b->highlighted == 1 ){ // If not, then if highlighted, dont draw
			b->highlighted = 0;
			needRedraw=1;
		}
		b=b->next;
		
	}
	if (needRedraw) {
		glutPostRedisplay();
	}
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

/** 
    This button draws a text string, we could use:
        GLUT_BITMAP_9_BY_15			
        GLUT_BITMAP_TIMES_ROMAN_10		
        GLUT_BITMAP_HELVETICA_18 
    It also receives:
        text *char Text to display
        x int x-coordinate
        y int y-coordinate
*/

void Font(void *font,char *text,int x,int y){
    glRasterPos2i(x,y); //Specifies the raster position of two pixel ops
    while(*text != '\0'){
        glutBitmapCharacter(font,*text);
        ++text;
    }
}

void ButtonDraw(){
    int fontx, fonty;
    Button* b = pButtonList;
    while(b){
        // To indicate the mouse is over a button, we change the color
        if(b->highlighted){
            glColor3f(0.7f,0.7f,0.8f);
        } else {
            glColor3f(0.6f,0.6f,0.6f);
        }
        // We draw a background
        glBegin(GL_QUADS);
        {
			glVertex2i( b->x, b->y);
			glVertex2i( b->x, b->y+b->h );
			glVertex2i( b->x+b->w, b->y+b->h );
			glVertex2i( b->x+b->w, b->y);
        }
		glEnd();
        // We draw an outline
        glLineWidth(3);
        // We reverse the outline colours when pressed
        if (b->state){
			glColor3f(0.4f,0.4f,0.4f);
        }else{ 
			glColor3f(0.8f,0.8f,0.8f);
        }

        glBegin(GL_LINE_STRIP);
        {
			glVertex2i( b->x+b->w, b->y);
			glVertex2i( b->x, b->y);
			glVertex2i( b->x, b->y+b->h);
        }
		glEnd();

        if (b->state){
			glColor3f(0.8f,0.8f,0.8f);
        }else{ 
			glColor3f(0.4f,0.4f,0.4f);
        }

		glBegin(GL_LINE_STRIP);
        {
			glVertex2i( b->x,b->y+b->h );
			glVertex2i( b->x+b->w,b->y+b->h );
			glVertex2i( b->x+b->w, b->y);
        }
		glEnd();

		glLineWidth(1);

        /** We calculate the coordinates to center it */
		fontx = b->x + (b->w - glutBitmapLength(GLUT_BITMAP_HELVETICA_10,b->label)) / 2 ;
		fonty = b->y + (b->h+10)/2;

		/** Move it a bit to change the appearance when pressed */
		if (b->state) {
			fontx+=2;
			fonty+=2;
		}

		/** if Highlited, change appearance again */
		if(b->highlighted){
			glColor3f(0,0,0);
			Font(GLUT_BITMAP_HELVETICA_10,b->label,fontx,fonty);
			fontx--;
			fonty--;
		}

		glColor3f(1,1,1);
		Font(GLUT_BITMAP_HELVETICA_10,b->label,fontx,fonty);

		b = b->next;
    }
}



/*----------------------------------------------------------------------------------------
 *	\brief	This function is called to initialise opengl.
 */

void Init()
{
	glEnable(GL_LIGHT0);

	CreateButton("Button1",TheButtonCallback,5,5,100,30);
	CreateButton("Button2",TheButtonCallback,5,45,100,30);
}

/*----------------------------------------------------------------------------------------
 *	This function will be used to draw the 3D scene
 */
void Draw3D()
{
	gluLookAt(0,1,5,0,0,0,0,1,0);
	glutSolidTeapot(1);
}

/*----------------------------------------------------------------------------------------
 *	This function will be used to draw an overlay over the 3D scene.
 *	This will be used to draw our fonts, buttons etc......
 */
void Draw2D()
{
	ButtonDraw();
}

/*----------------------------------------------------------------------------------------
 *	This is the main display callback function. It sets up the drawing for 
 *	The 3D scene first then calls the Draw3D() function. After that it switches to 
 *	an orthographic projection and calls Draw2D().
 */
void Draw()
{
	/*
	 *	Clear the background
	 */
	glClear( GL_COLOR_BUFFER_BIT |
			 GL_DEPTH_BUFFER_BIT );

	/*
	 *	Enable lighting and the z-buffer
	 */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	/*
	 *	Set perspective viewing transformation
	 */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,(winh==0)?(1):((float)winw/winh),1,100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/*
	 *	Draw the 3D elements in the scene
	 */
	Draw3D();

	/*
	 *	Disable depth test and lighting for 2D elements
	 */
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	/*
	 *	Set the orthographic viewing transformation
	 */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,winw,winh,0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/*
	 *	Draw the 2D overlay
	 */
	Draw2D();

	/*
	 *	Bring the back buffer to the front and vice-versa.
	 */
	glutSwapBuffers();
}

/*----------------------------------------------------------------------------------------
 *	This function is called when the window is resized. All this does is simply 
 *	store the new width and height of the window which are then referenced by
 *	the draw function to set the correct viewing transforms 
 */
void Resize(int w, int h)
{
	winw = w;
	winh = h;

	/*
	 *	Allow drawing in full region of the screen
	 */
	glViewport(0,0,w,h);
}


/*----------------------------------------------------------------------------------------
 *	\brief	This function is called whenever a mouse button is pressed or released
 *	\param	button	-	GLUT_LEFT_BUTTON, GLUT_RIGHT_BUTTON, or GLUT_MIDDLE_BUTTON
 *	\param	state	-	GLUT_UP or GLUT_DOWN depending on whether the mouse was released
 *						or pressed respectivly. 
 *	\param	x		-	the x-coord of the mouse cursor.
 *	\param	y		-	the y-coord of the mouse cursor.
 */
void MouseButton(int button,int state,int x, int y)
{
	/*
	 *	update the mouse position
	 */
	MickeyMouse->x = x;
	MickeyMouse->y = y;

	/*
	 *	has the button been pressed or released?
	 */
	if (state == GLUT_DOWN) 
	{
		/*
		 *	This holds the location of the first mouse click
		 */
		//if ( !(MickeyMouse.lmb || MickeyMouse.mmb || MickeyMouse.rmb) ) {
			MickeyMouse->xpress = x;
			MickeyMouse->ypress = y;
		//}

		/*
		 *	Which button was pressed?
		 */
		switch(button) 
		{
		case GLUT_LEFT_BUTTON:
			MickeyMouse->lmb = 1;
			ButtonPress(x,y);
		case GLUT_MIDDLE_BUTTON:
			MickeyMouse->mmb = 1;
			break;
		case GLUT_RIGHT_BUTTON:
			MickeyMouse->rmb = 1;
			break;
		}
	}
	else 
	{
		/*
		 *	Which button was released?
		 */
		switch(button) 
		{
		case GLUT_LEFT_BUTTON:
			MickeyMouse->lmb = 0;
			ButtonRelease(x,y);
			break;
		case GLUT_MIDDLE_BUTTON:
			MickeyMouse->mmb = 0;
			break;
		case GLUT_RIGHT_BUTTON:
			MickeyMouse->rmb = 0;
			break;
		}
	}

	/*
	 *	Force a redraw of the screen. If we later want interactions with the mouse
	 *	and the 3D scene, we will need to redraw the changes.
	 */
	glutPostRedisplay();
}

/*----------------------------------------------------------------------------------------
 *	\brief	This function is called whenever the mouse cursor is moved AND A BUTTON IS HELD.
 *	\param	x	-	the new x-coord of the mouse cursor.
 *	\param	y	-	the new y-coord of the mouse cursor.
 */
void MouseMotion(int x, int y)
{
	/*
	 *	Calculate how much the mouse actually moved
	 */
	int dx = x - MickeyMouse->x;
	int dy = y - MickeyMouse->y;

	/*
	 *	update the mouse position
	 */
	MickeyMouse->x = x;
	MickeyMouse->y = y;


	/*
	 *	Check MyButton to see if we should highlight it cos the mouse is over it
	 */
	ButtonPassive(x,y);

	/*
	 *	Force a redraw of the screen
	 */
	glutPostRedisplay();
}

/*----------------------------------------------------------------------------------------
 *	\brief	This function is called whenever the mouse cursor is moved AND NO BUTTONS ARE HELD.
 *	\param	x	-	the new x-coord of the mouse cursor.
 *	\param	y	-	the new y-coord of the mouse cursor.
 */
void MousePassiveMotion(int x, int y)
{
	/*
	 *	Calculate how much the mouse actually moved
	 */
	int dx = x - MickeyMouse->x;
	int dy = y - MickeyMouse->y;

	/*
	 *	update the mouse position
	 */
	MickeyMouse->x = x;
	MickeyMouse->y = y;

	/*
	 *	Check MyButton to see if we should highlight it cos the mouse is over it
	 */
	ButtonPassive(x,y);

	/*
	 *	Note that I'm not using a glutPostRedisplay() call here. The passive motion function 
	 *	is called at a very high frequency. We really don't want much processing to occur here.
	 *	Redrawing the screen every time the mouse moves is a bit excessive. Later on we 
	 *	will look at a way to solve this problem and force a redraw only when needed. 
	 */
}

/*----------------------------------------------------------------------------------------
 *
 */
int main(int argc,char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowSize(winw,winh);
	glutInitWindowPosition(200,100);
	glutCreateWindow("03 - Mouse Motion");

	glutDisplayFunc(Draw);
	glutReshapeFunc(Resize);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MousePassiveMotion);

	Init();

	glutMainLoop();
}