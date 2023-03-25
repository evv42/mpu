/* See LICENSE file for copyright and license details. */
/* X Dummy for interaction with X11 environment
 * Copyright 2020,2021,2022 evv42.
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "xdummy.h"

char** init_window_name_array(){
	char** array=malloc(MAX_WINDOWS_DISPLAYED*sizeof(char**));//sizeof is nessesary for all types other than char, evv !
	if(array == NULL){perror("malloc");exit(12);}
	for(int i=0; i<MAX_WINDOWS_DISPLAYED; i++){
		array[i]=malloc(MAX_WINDOW_NAME_SIZE+1);
		if(array[i] == NULL){perror("malloc");exit(13);}
		*array[i]=0;
	}
	return array;
}

int XDunGoofedUp(Display* disp, XErrorEvent* xe){
        printf("X11 broke, but it refused.\n");
		printf("Seriously, we got error code %d, op-code %d,%d (M,m). Hope that helps.",xe->error_code,xe->request_code,xe->minor_code);
        return 0;
}

XDummy* CreateXDummy(){
    XDummy* dm = malloc(sizeof(XDummy));
    if(dm == NULL){
        perror("malloc");
        exit(1);
    }

    char *dn = getenv("DISPLAY");
    dm->dpy = XOpenDisplay(dn);
    dm->wnames = init_window_name_array();

    XSetErrorHandler( XDunGoofedUp );

    dm->wids = malloc(MAX_WINDOWS_DISPLAYED*sizeof(unsigned long*));
    if(dm->wids == NULL){perror("malloc");exit(12);}

    dm->wstates = malloc(MAX_WINDOWS_DISPLAYED*sizeof(unsigned long*));
    if(dm->wstates == NULL){perror("malloc");exit(12);}

    dm->wfocused = 0;

    return dm;
}

void XDummyDummy(XDummy* dmy){
    free(dmy->wids);
    for(int i=0; i<MAX_WINDOWS_DISPLAYED; i++){
        free(dmy->wnames[i]);
    }
    free(dmy->wnames);
    XCloseDisplay(dmy->dpy);
}

static unsigned int sum(char* str){
	const uint32_t MOD_ADLER = 65521;
	unsigned int size = strlen(str);
    uint16_t sum = 1;
    for (uint32_t i=0; i < size; i++)sum = (sum + str[i]) % MOD_ADLER;
    return sum;
}

void XDummyCopyWindowName(XDummy* dmy, Window win, int pos){
    Atom a = XInternAtom(dmy->dpy, "_NET_WM_NAME" , False), actualType;
    int format;
    unsigned long numItems, bytesAfter;
    unsigned char* name = NULL;

    int status = XGetWindowProperty(dmy->dpy, win,a,0L,(~0L),False,AnyPropertyType,&actualType,&format,&numItems,&bytesAfter,&name);

    if(status >= Success && name != NULL){
        strncpy(dmy->wnames[pos],(const char *)name,MAX_WINDOW_NAME_SIZE);
        dmy->wnames[pos][MAX_WINDOW_NAME_SIZE]=0;
		dmy->checksum += sum(dmy->wnames[pos]);
    }else{
		a = XInternAtom(dmy->dpy, "WM_NAME" , False);
		status = XGetWindowProperty(dmy->dpy, win,a,0L,(~0L),False,AnyPropertyType,&actualType,&format,&numItems,&bytesAfter,&name);
        if(status >= Success && name != NULL){
			strncpy(dmy->wnames[pos],(const char *)name,MAX_WINDOW_NAME_SIZE);
			dmy->wnames[pos][MAX_WINDOW_NAME_SIZE]=0;
			dmy->checksum += sum(dmy->wnames[pos]);
		}else{
			strncpy(dmy->wnames[pos],"Untitled",MAX_WINDOW_NAME_SIZE);
		}
    }
    XFree(name);
}

char XDummyGetWindowState(XDummy* dmy, Window win){
    char r=1;
    XWindowAttributes attr = { 0 };

    int status = XGetWindowAttributes(dmy->dpy, win, &attr);

    if(status >= Success /*&& attr != NULL*/){
        r=(attr.map_state == 0);
        //XFree(attr);
    }
    return r;
}

void XDummyGetWindows(XDummy* dmy){
    //get windows names, ids, states and focus from window manager
    Atom a = XInternAtom(dmy->dpy, "_NET_CLIENT_LIST" , False), actualType;
    int format;
    unsigned long numItems, bytesAfter;
    unsigned char* data = NULL;

    int status = XGetWindowProperty(dmy->dpy, DefaultRootWindow(dmy->dpy), a, 0L, (~0L), False, AnyPropertyType, &actualType, &format, &numItems, &bytesAfter, &data);

	//reset checksum
	dmy->checksum = 0;

    if (status >= Success && numItems){
        //get focused window
        Window focused;
        int revert;
        XGetInputFocus(dmy->dpy, &focused, &revert);

        //put windows names in char** and ids in unsigned long*
        unsigned long* array = (unsigned long*) data;
		memcpy(dmy->wids,array,numItems*sizeof(unsigned long));
        
		int k;
        for (k = 0; k < MAX_WINDOWS_DISPLAYED-1 && k<numItems; k++){
            // get window Id:
            Window w = (Window) dmy->wids[k];
//            printf("%ld,%ld\n",w,focused);
            if(w == focused || w == focused-1)dmy->wfocused = k;
			XDummyCopyWindowName(dmy,w,k);
            dmy->wstates[k] = XDummyGetWindowState(dmy,w);
        }
		dmy->checksum += dmy->wfocused;
        dmy->wn=k;
    }
}

void XDummyToggleWindow(XDummy* dmy, int window){
    //if not focused, raise, else iconify
    if(window != dmy->wfocused){
        XMapRaised(dmy->dpy, (Window) dmy->wids[window]);
    }else{
        XIconifyWindow(dmy->dpy, (Window) dmy->wids[window], XDefaultScreen(dmy->dpy));
    }
}

void XDummyHideWindow(XDummy* dmy, int window){
	XIconifyWindow(dmy->dpy, (Window) dmy->wids[window], XDefaultScreen(dmy->dpy));

}

int XDummyGetWindowByName(XDummy* dmy, char* name){
	for(int i=0; i<dmy->wn; i++){
		if(strcmp(dmy->wnames[i],name) == 0)return i;
	}
	return -1;
}

void XDummySetWindowName(XDummy* dmy, int window, char* name){
	XStoreName(dmy->dpy, dmy->wids[window], name);
}

void XDummyMoveWindow(XDummy* dmy, int window, int x, int y){
	XMoveWindow(dmy->dpy, dmy->wids[window], x, y);
}
