/* See LICENSE file for copyright and license details. */
/* X Dummy for interaction with X11 environment
 * Copyright 2020,2021 evv42.
*/
# ifndef __XDUMMY_H__
# define __XDUMMY_H__

#define MAX_WINDOWS_DISPLAYED 21
#define MAX_WINDOW_NAME_SIZE  32
#include <X11/Xlib.h>

struct XDummy{
    Display* dpy;
    char** wnames;
    unsigned long* wids;
    unsigned char* wstates;
    int wfocused;
    int wn;
	unsigned long checksum;
};
typedef struct XDummy XDummy;

/* Creates a dummy. */
XDummy* CreateXDummy();

/* Destroy a dummy. */
void XDummyDummy(XDummy* dmy);

void XDummyGetWindows(XDummy* dmy);

void XDummyToggleWindow(XDummy* dmy, int window);

void XDummyHideWindow(XDummy* dmy, int window);

int XDummyGetWindowByName(XDummy* dmy, char* name);

void XDummySetWindowName(XDummy* dmy, int window, char* name);

void XDummyMoveWindow(XDummy* dmy, int window, int x, int y);

# endif
