/************************************************************************
 * 8080 Emulator Display Peripheral v0.0.0				*
 * Pramuka Perera							*
 * June 23, 2017							*
 * VT100 Monitor						 	*
 ************************************************************************/

#ifndef INCLUDE
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <string.h>
	#include "common.h"

	#define INCLUDE
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
//#include <X11/Xos.h> 

#define ORIGIN_X 0
#define ORIGIN_Y 0

#define WIDTH 400
#define HEIGHT 400

//gcc -I /usr/X11/include -L /usr/X11/lib -o x1 x1.cc -lX11
Display *v100;
Window  win;
XEvent evt;
GC gc;

void StartMonitor()
{
	int screen;
	unsigned long 	white,
			black;
	long eventMask;
	

	//define window variables
	v100 = XOpenDisplay(NULL);
	if(v100 == NULL)
	{
		printf("Display failed to start\n");
		exit(EXIT_FAILURE);
	}

	screen = DefaultScreen(v100);
	white = WhitePixel(v100, screen);
	black = BlackPixel(v100, screen);

	//create window in memory
	win = XCreateSimpleWindow(	v100, 
					DefaultRootWindow(v100),
					ORIGIN_X, ORIGIN_Y,		//location of top-left corner of window
					WIDTH, HEIGHT,			//size of window
					0, white,			//border
					black);				//background

	//show window on physical monitor
	XMapWindow(v100, win);	

	//tell x-server to notify client when mapping procedure has completed successfully	
	eventMask = StructureNotifyMask;
	XSelectInput(v100, win, eventMask);

	//wait until mapping procedure is complete
	do
	{
		XNextEvent( v100, &evt);
	}
	while(evt.type != MapNotify);

	//draw in window
	gc = XCreateGC( 	v100, win,
				0,
				NULL);
	XSetForeground( v100, gc, white);	

		
	eventMask = ButtonPressMask|ButtonReleaseMask;
	XSelectInput(v100,win,eventMask); // override prev
 
  	do
	{
    		XNextEvent(v100, &evt);   // calls XFlush()
  	}
	while( evt.type != ButtonRelease );
	
}

void DrawLines()
{	
	XDrawLine(v100, win, gc, 10, 10,190,190); //from-to
	XDrawLine(v100, win, gc, 10,190,190, 10); //from-to
}

void CloseMonitor()
{
	XFreeGC( v100, gc);
 	XDestroyWindow(v100, win);
  	XCloseDisplay(v100);
}
